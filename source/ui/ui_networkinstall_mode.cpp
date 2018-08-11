#include "ui/ui_networkinstall_mode.hpp"

#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sstream>

#include "nx/ipc/tin_ipc.h"
#include "ui/console_view.hpp"
#include "ui/console_options_view.hpp"
#include "util/network_util.hpp"
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
    }

    NetworkInstallMode::~NetworkInstallMode()
    {
        if (m_serverSocket != 0)
        {
            close(m_serverSocket);
            m_serverSocket = 0;
        }
    }

    void NetworkInstallMode::InitializeServerSocket() try
    {
        // Create a socket
        m_serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    
        if (m_serverSocket < -1)
            THROW_FORMAT("Failed to create a server socket. Error code: %u\n", errno);

        struct sockaddr_in server;
        server.sin_family = AF_INET;
        server.sin_port = htons(REMOTE_INSTALL_PORT);
        server.sin_addr.s_addr = htonl(INADDR_ANY);

        if (bind(m_serverSocket, (struct sockaddr*) &server, sizeof(server)) < 0)
            THROW_FORMAT("Failed to bind server socket. Error code: %u\n", errno);

        // Set as non-blocking
        fcntl(m_serverSocket, F_SETFL, fcntl(m_serverSocket, F_GETFL, 0) | O_NONBLOCK);

        if (listen(m_serverSocket, 5) < 0) 
            THROW_FORMAT("Failed to listen on server socket. Error code: %u\n", errno);
    }
    catch (std::exception& e)
    {
        printf("Failed to initialize server socket!\n");
        fprintf(stdout, "%s", e.what());

        if (m_serverSocket != 0)
        {
            close(m_serverSocket);
            m_serverSocket = 0;
        }
    }

    void NetworkInstallMode::OnSelected()
    {
        tin::ui::ViewManager& manager = tin::ui::ViewManager::Instance();
        auto view = std::make_unique<tin::ui::ConsoleView>();
        manager.PushView(std::move(view));

        try
        {
            #ifndef NXLINK_DEBUG
            ASSERT_OK(socketInitializeDefault(), "Failed to initialize socket");
            #endif

            this->InitializeServerSocket();

            if (m_serverSocket <= 0)
                THROW_FORMAT("Server socket failed to initialize.\n");

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
                    tin::network::WaitReceiveNetworkData(m_clientSocket, &size, sizeof(u32));
                    size = ntohl(size);

                    printf("Received url buf size: 0x%x\n", size);

                    if (size > MAX_URL_SIZE * MAX_URLS)
                        THROW_FORMAT("URL size %x is too large!\n", size);

                    // Make sure the last string is null terminated
                    auto urlBuf = std::make_unique<char[]>(size+1);
                    memset(urlBuf.get(), 0, size+1);

                    tin::network::WaitReceiveNetworkData(m_clientSocket, urlBuf.get(), size);

                    // Split the string up into individual URLs
                    std::stringstream urlStream(urlBuf.get());
                    std::string segment;
                    std::vector<std::string> urls;

                    while (std::getline(urlStream, segment, '\n'))
                    {
                        urls.push_back(segment);
                    }

                    std::string nspExt = ".nsp";
                    
                    for (auto& url : urls)
                    {
                        if (url.compare(url.size() - nspExt.size(), nspExt.size(), nspExt) == 0)
                        {
                            printf("Received NSP URL: %s\n", url.c_str());
                        }
                    }

                    // Send 1 byte ack to close the server
                    u8 ack = 0;
                    tin::network::WaitSendNetworkData(m_clientSocket, &ack, sizeof(u8));

                    break;
                }
                else if (errno != EAGAIN)
                {
                    THROW_FORMAT("Failed to open client socket with code %u\n", errno);
                }

                // Break on input pressed
                u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);

                if (kDown & KEY_B)
                    break;
            }


        }
        catch (std::exception& e)
        {
            printf("Failed to perform remote install!\n");
            LOG_DEBUG("Failed to perform remote install");
            LOG_DEBUG("%s", e.what());
            fprintf(stdout, "%s", e.what());
        }

        if (m_clientSocket != 0)
        {
            close(m_clientSocket);
            m_clientSocket = 0;
        }

        #ifndef NXLINK_DEBUG
        socketExit();
        #endif
    }
}