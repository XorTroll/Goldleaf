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
            std::vector<u8> m_headerBytes;

        public:
            tin::network::HTTPDownload m_download;

            RemoteNSP(std::string url);

            void RetrieveHeader();
            void RetrieveAndProcessNCA(NcmNcaId ncaId, std::function<void (void* blockBuf, size_t bufSize, size_t blockStartOffset, size_t ncaSize)> processBlockFunc, std::function<void (size_t sizeRead)> progressFunc);
    
            const PFS0BaseHeader* GetBaseHeader();
            u64 GetDataOffset();

            const PFS0FileEntry* GetFileEntry(unsigned int index);
            const PFS0FileEntry* GetFileEntryByName(std::string name);
            const PFS0FileEntry* GetFileEntryByNcaId(const NcmNcaId& ncaId);
            const PFS0FileEntry* GetFileEntryByExtension(std::string extension);

            const char* GetFileEntryName(const PFS0FileEntry* fileEntry);
    };
}