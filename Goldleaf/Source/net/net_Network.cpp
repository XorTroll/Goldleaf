
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2021 XorTroll

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

#include <net/net_Network.hpp>

namespace net {

    namespace {

        std::size_t StringWriteImpl(const char* in, std::size_t size, std::size_t num, std::string *out) {
            const auto total_size = size * num;
            out->append(in, total_size);
            return total_size;
        }

        std::size_t FileWriteImpl(const char* in, std::size_t size, std::size_t num, FILE *out) {
            fwrite(in, size, num, out);
            return size * num;
        }

        std::function<void(double, double)> g_CurrentCallback = [](double, double){};

        int ProgressImpl(void* ptr, double TotalToDownload, double NowDownloaded, double TotalToUpload, double NowUploaded) {
            g_CurrentCallback(NowDownloaded, TotalToDownload);
            return 0;
        }

    }

    std::string RetrieveContent(const std::string &url, const std::string &mime_type) {
        socketInitializeDefault();
        std::string content;
        auto curl = curl_easy_init();
        if(!mime_type.empty()) {
            curl_slist *header_data = nullptr;
            header_data = curl_slist_append(header_data, ("Content-Type: " + mime_type).c_str());
            header_data = curl_slist_append(header_data, ("Accept: " + mime_type).c_str());
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_data);
        }
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "Goldleaf");
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, StringWriteImpl);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &content);
        curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        socketExit();
        return content;
    }

    void RetrieveToFile(const std::string &url, const std::string &path, std::function<void(double Done, double Total)> cb_fn)
    {
        socketInitializeDefault();
        auto f = fopen(path.c_str(), "wb");
        if(f) {
            g_CurrentCallback = cb_fn;
            auto curl = curl_easy_init();
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_USERAGENT, "Goldleaf");
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, FileWriteImpl);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, f);
            curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
            curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, ProgressImpl);
            curl_easy_perform(curl);
            curl_easy_cleanup(curl);
        }
        fclose(f);
        socketExit();
    }
    
    bool HasConnection() {
        return gethostid() == INADDR_LOOPBACK;
    }

}