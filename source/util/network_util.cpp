#include "util/network_util.hpp"

#include <curl/curl.h>
#include <algorithm>
#include <cstring>
#include "error.hpp"

namespace tin::network
{
    size_t HTTPHeader::ParseHTMLHeader(char* bytes, size_t size, size_t numItems, void* userData)
    {
        HTTPHeader* header = reinterpret_cast<HTTPHeader*>(userData);
        size_t numBytes = size * numItems;
        std::string line(bytes, numBytes);

        // Remove any newlines or carriage returns
        line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());
        line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());

        // Split into key and value
        if (!line.empty())
        {
            auto keyEnd = line.find(": ");

            if (keyEnd != 0)
            {
                std::string key = line.substr(0, keyEnd);
                std::string value = line.substr(keyEnd + 2);

                // Make key lowercase
                std::transform(key.begin(), key.end(), key.begin(), ::tolower);
                header->m_values[key] = value;
            }
        }

        return numBytes;
    }

    // HTTPHeader
    HTTPHeader::HTTPHeader(std::string url) :
        m_url(url)
    {

    }

    void HTTPHeader::PerformRequest()
    {
        // We don't want any existing values to get mixed up with this request
        m_values.clear();

        CURL* curl = curl_easy_init();
        CURLcode rc = (CURLcode)0;

        if (!curl)
        {
            THROW_FORMAT("Failed to initialize curl\n");
        }

        curl_easy_setopt(curl, CURLOPT_URL, m_url.c_str());
        curl_easy_setopt(curl, CURLOPT_NOBODY, true);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "tinfoil");
        curl_easy_setopt(curl, CURLOPT_HEADERDATA, this);
        curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, &tin::network::HTTPHeader::ParseHTMLHeader);

        rc = curl_easy_perform(curl);
        if (rc != CURLE_OK)
        {
            THROW_FORMAT("Failed to retrieve HTTP Header: %s\n", curl_easy_strerror(rc));
        }

        u64 httpCode = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
        curl_easy_cleanup(curl);

        if (httpCode != 200 && httpCode != 204)
        {
            THROW_FORMAT("Unexpected HTTP response code when retrieving header: %lu\n", httpCode);
        }
    }

    bool HTTPHeader::HasValue(std::string key)
    {
        return m_values.count(key);
    }

    std::string HTTPHeader::GetValue(std::string key)
    {
        return m_values[key];
    }

    /*
        def __init__(self, url):
        self.values = {}
        self.url = url

        curl = pycurl.Curl()
        curl.setopt(curl.URL, self.url)
        curl.setopt(curl.NOBODY, True) 
        curl.setopt(curl.SSL_VERIFYPEER, False)
        curl.setopt(curl.HEADERFUNCTION, self.parse_header_line)
        curl.perform()

        http_code = curl.getinfo(curl.HTTP_CODE)
        curl.close()

        if http_code != 200 and http_code != 204:
            raise Exception('Unexpected HTTP code when retrieving header: {}'.format(http_code))

    def parse_header_line(self, line):
        header_line = line.decode('iso-8859-1')

        if ':' not in header_line:
            return

        name, value = header_line.split(':', 1)
        
        # Remove newlines and whitespace
        name = name.strip()
        value = value.strip()

        name = name.lower()
        self.values[name] = value
    */

    // End HTTPHeader



    size_t WaitReceiveNetworkData(int sockfd, void* buf, size_t len)
    {
        size_t blockSizeRead = 0;
        size_t sizeRead = 0;

        while ((sizeRead += (blockSizeRead = recv(sockfd, (u8*)buf + sizeRead, len - sizeRead, 0)) < len) && (blockSizeRead > 0 || errno == EAGAIN) && !(hidKeysDown(CONTROLLER_P1_AUTO) & KEY_B))
        {
            hidScanInput();
            errno = 0;
        }

        return sizeRead;
    }

    size_t WaitSendNetworkData(int sockfd, void* buf, size_t len)
    {
        size_t blockSizeWritten = 0;
        size_t sizeWritten = 0;

        while ((sizeWritten += (blockSizeWritten = send(sockfd, (u8*)buf + sizeWritten, len - sizeWritten, 0)) < len) && (blockSizeWritten > 0 || errno == EAGAIN) && !(hidKeysDown(CONTROLLER_P1_AUTO) & KEY_B))
        {
            hidScanInput();
            errno = 0;
        }

        return sizeWritten;
    }
}