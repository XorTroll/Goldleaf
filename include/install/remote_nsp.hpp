#pragma once

#include <functional>
#include <vector>

#include <switch.h>
#include "install/pfs0.hpp"
#include "util/network_util.hpp"

namespace tin::install::nsp
{
    class RemoteNSP
    {
        private:
            tin::network::HTTPDownload m_download;
            std::vector<u8> m_headerBytes;

        public:
            RemoteNSP(std::string url);

            void RetrieveHeader();
            void RetrieveAndProcessNCA(NcmNcaId ncaId, std::function<void (void* blockBuf, size_t bufSize, size_t blockStartOffset, size_t ncaSize)> processBlockFunc);
    
            const PFS0BaseHeader* GetBaseHeader();
            const PFS0FileEntry* GetFileEntry(unsigned int index);
            const PFS0FileEntry* GetFileEntryByName(std::string name);
            const PFS0FileEntry* GetFileEntryByExtension(std::string extension);
            const char* GetFileEntryName(const PFS0FileEntry* fileEntry);
    };
}