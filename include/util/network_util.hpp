#pragma once

#include <switch.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/errno.h>
#include <fcntl.h>
#include <unistd.h>

#include <map>
#include <string>
#include <vector>

namespace tin::network
{
    class HTTPHeader
    {
        private:
            std::string m_url;
            std::map<std::string, std::string> m_values;

            static size_t ParseHTMLHeader(char* bytes, size_t size, size_t numItems, void* userData);

        public:
            HTTPHeader(std::string url);

            void PerformRequest();

            bool HasValue(std::string key);
            std::string GetValue(std::string key);
    };

    size_t WaitReceiveNetworkData(int sockfd, void* buf, size_t len);
    size_t WaitSendNetworkData(int sockfd, void* buf, size_t len);
}