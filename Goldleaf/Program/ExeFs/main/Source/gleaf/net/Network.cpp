#include <gleaf/net/Network.hpp>

namespace gleaf::net
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

    std::function<void(u8)> tmpcb = [&](u8 pc){};

    int CurlProgress(void* ptr, double TotalToDownload, double NowDownloaded, double TotalToUpload, double NowUploaded)
    {
        tmpcb((NowDownloaded / TotalToDownload) * 100.0);
        return 0;
    }

    std::string RetrieveContent(std::string URL, std::string MIMEType)
    {
        std::string cnt;
        CURL *curl = curl_easy_init();
        if(!MIMEType.empty())
        {
            curl_slist *headerdata = NULL;
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
        return cnt;
    }

    void RetrieveToFile(std::string URL, std::string Path, std::function<void(u8)> Callback)
    {
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
    }

    bool CheckVersionDiff()
    {
        bool diff = false;
        if(HasConnection())
        {
            std::string js = RetrieveContent("https://api.github.com/repos/xortroll/goldleaf/releases", "application/json");
            json j = json::parse(js);
            std::string latestid = j[0]["tag_name"].get<std::string>();
            Version latestv = Version::FromString(latestid);
            Version currentv = Version::FromString("0.3");
            diff = latestv.IsLower(currentv);
        }
        return diff;
    }
    
    bool HasConnection()
    {
        u32 strg = 0;
        nifmGetInternetConnectionStatus(NULL, &strg, NULL);
        return (strg > 0);
    }
}