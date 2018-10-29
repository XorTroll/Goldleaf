#pragma once

#include <string>
#include "install/remote_nsp.hpp"

namespace tin::install::nsp
{
    static const size_t PADDING_SIZE = 0x1000;
    static const u32 CMD_ID_FILE_RANGE = 1;
    static const u32 CMD_ID_FILE_RANGE_PADDED = 2;
    class USBNSP : public RemoteNSP
    {
        private:
            std::string m_nspName;

        public:
            USBNSP(std::string nspName);

            virtual void StreamToPlaceholder(nx::ncm::ContentStorage& contentStorage, NcmNcaId placeholderId) override;
            virtual void BufferData(void* buf, off_t offset, size_t size) override;
    };
}