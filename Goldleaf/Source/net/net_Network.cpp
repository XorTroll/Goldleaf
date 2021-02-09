
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2020  XorTroll

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

namespace net
{
    std::size_t CurlStrWrite(const char* in, std::size_t size, std::size_t num, std::string* out)
    {
        const size_t totalBytes(size * num);
        out->append(in, totalBytes);
        return totalBytes;
    }

    std::size_t CurlFileWrite(const char* in, std::size_t size, std::size_t num, FILE* out)
    {
        fwrite(in, size, num, out);
        return (size * num);
    }

    std::function<void(double, double)> tmpcb = [](double, double){};

    int CurlProgress(void* ptr, double TotalToDownload, double NowDownloaded, double TotalToUpload, double NowUploaded)
    {
        tmpcb(NowDownloaded, TotalToDownload);
        return 0;
    }

    std::string RetrieveContent(std::string URL, std::string MIMEType)
    {
        socketInitializeDefault();
        std::string cnt;
        CURL *curl = curl_easy_init();
        if(!MIMEType.empty())
        {
            curl_slist *headerdata = nullptr;
            headerdata = curl_slist_append(headerdata, ("Content-Type: " + MIMEType).c_str());
            headerdata = curl_slist_append(headerdata, ("Accept: " + MIMEType).c_str());
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerdata);
        }
        curl_easy_setopt(curl, CURLOPT_URL, URL.c_str());
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "Goldleaf");
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlStrWrite);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &cnt);
        curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        socketExit();
        return cnt;
    }

    void RetrieveToFile(std::string URL, std::string Path, std::function<void(double Done, double Total)> Callback)
    {
        socketInitializeDefault();
        FILE *f = fopen(Path.c_str(), "wb");
        if(f)
        {
            tmpcb = Callback;
            CURL *curl = curl_easy_init();
            curl_easy_setopt(curl, CURLOPT_URL, URL.c_str());
            curl_easy_setopt(curl, CURLOPT_USERAGENT, "Goldleaf");
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlFileWrite);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, f);
            curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
            curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, CurlProgress);
            curl_easy_perform(curl);
            curl_easy_cleanup(curl);
        }
        fclose(f);
        socketExit();
    }
    
    bool HasConnection()
    {
        u32 id = gethostid();
        return (id == INADDR_LOOPBACK);
    }
}