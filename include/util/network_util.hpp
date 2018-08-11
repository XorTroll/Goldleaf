#pragma once

#include <switch.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/errno.h>
#include <fcntl.h>
#include <unistd.h>

#include <string>
#include <vector>

namespace tin::network
{
    class HTTPHeader
    {
        private:
            std::string m_url;

        public:
            std::string m_test = "HI";

            HTTPHeader(std::string url);

            void PerformRequest();
            std::string GetValue();
            static size_t ParseHTMLHeader(char* bytes, size_t size, size_t numItems, void* userData);
    };

    size_t WaitReceiveNetworkData(int sockfd, void* buf, size_t len);
    size_t WaitSendNetworkData(int sockfd, void* buf, size_t len);
}