#pragma once

#include <switch.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/errno.h>
#include <fcntl.h>
#include <unistd.h>

#include <map>
#include <memory>
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

    class HTTPDownloadBuffer
    {
        public:
            size_t m_readSize = 0;
            const size_t m_totalSize; 
            void* m_buffer;

            HTTPDownloadBuffer(size_t size, void* buffer);
    };

    class HTTPDownload
    {
        private:
            std::string m_url;
            HTTPHeader m_header;

            static size_t ParseHTMLData(char* bytes, size_t size, size_t numItems, void* userData);

        public:
            HTTPDownload(std::string url);
    
            bool IsRangesSupported();
            void RequestDataRange(void* buffer, size_t offset, size_t size);
    };

    size_t WaitReceiveNetworkData(int sockfd, void* buf, size_t len);
    size_t WaitSendNetworkData(int sockfd, void* buf, size_t len);
}