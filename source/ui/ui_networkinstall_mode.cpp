#include "ui/ui_networkinstall_mode.hpp"

#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/errno.h>
#include <fcntl.h>
#include <unistd.h>

#include "nx/ipc/tin_ipc.h"
#include "ui/console_view.hpp"
#include "ui/console_options_view.hpp"
#include "error.hpp"

namespace tin::ui
{
    const unsigned int MAX_URL_SIZE = 1024;
    const unsigned int MAX_URLS = 256;
    const int REMOTE_INSTALL_PORT = 5000;
    static int m_serverSocket = 0;
    static int m_clientSocket = 0;

    NetworkInstallMode::NetworkInstallMode() :
        IMode("Network Install NSP")
    {
        // Create a socket
        m_serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    
        if (m_serverSocket < -1)
        {
            printf("Failed to create a server socket. Error code: %u\n", errno);
            return;
        }

        struct sockaddr_in server;
        server.sin_family = AF_INET;
        server.sin_port = htons(REMOTE_INSTALL_PORT);
        server.sin_addr.s_addr = htonl(INADDR_ANY);

        if (bind(m_serverSocket, (struct sockaddr*) &server, sizeof(server)) < 0)
        {
            printf("Failed to bind server socket. Error code: %u\n", errno);
            close(m_serverSocket);
            return;
        }

        // Set as non-blocking
        fcntl(m_serverSocket, F_SETFL, fcntl(m_serverSocket, F_GETFL, 0) | O_NONBLOCK);

        if (listen(m_serverSocket, 5) < 0) 
        {
            printf("Failed to listen on server socket. Error code: %u\n", errno);
            close(m_serverSocket);
            return;
        }
    }

    NetworkInstallMode::~NetworkInstallMode()
    {
        if (m_serverSocket != 0)
        {
            close(m_serverSocket);
            m_serverSocket = 0;
        }
    }

    size_t NetworkInstallMode::WaitReceiveNetworkData(int sockfd, void* buf, size_t len)
    {
        int ret = 0;
        size_t blockSizeRead = 0;
        size_t sizeRead = 0;

        while ((sizeRead += (blockSizeRead = recv(sockfd, (u8*)buf + sizeRead, len - sizeRead, 0)) < len) && (blockSizeRead > 0 || errno == EAGAIN) && !(hidKeysDown(CONTROLLER_P1_AUTO) & KEY_B))
        {
            errno = 0;
        }

        return sizeRead;
    }

    void NetworkInstallMode::OnSelected()
    {
        tin::ui::ViewManager& manager = tin::ui::ViewManager::Instance();
        auto view = std::make_unique<tin::ui::ConsoleView>();
        manager.PushView(std::move(view));

        if (m_serverSocket < 0)
        {
            printf("Server socket failed to initialize.\n");
            return;
        }

        struct in_addr addr = {(in_addr_t) gethostid()};

        printf("Switch IP is %s\n", inet_ntoa(addr));
        printf("Waiting for connection...\n");

        while (true)
        {
            struct sockaddr_in client;
            socklen_t clientLen = sizeof(client);

            m_clientSocket = accept(m_serverSocket, (struct sockaddr*)&client, &clientLen);

            if (m_clientSocket >= 0)
            {
                printf("Accepted client connection\n");
                u32 size = 0;
                this->WaitReceiveNetworkData(m_clientSocket, &size, sizeof(u32));
                size = ntohl(size);

                printf("Received url buf size: 0x%x\n", size);

                if (size > MAX_URL_SIZE * MAX_URLS)
                {
                    printf("URL size is too large!\n");
                    break;
                }

                // Make sure the last string is null terminated
                auto urlBuf = std::make_unique<u8[]>(size+1);
                memset(urlBuf.get(), 0, size+1);

                this->WaitReceiveNetworkData(m_clientSocket, urlBuf.get(), size);
                printf("URLS: %s\n", urlBuf.get());

                break;
            }
            else if (errno != EAGAIN)
            {
                printf("Failed to open client socket with code %u\n", errno);
                break;
            }

            // Break on input pressed
            u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);

            if (kDown & KEY_B)
                break;
        }

        if (m_clientSocket != 0)
        {
            close(m_clientSocket);
            m_clientSocket = 0;
        }
    }
}