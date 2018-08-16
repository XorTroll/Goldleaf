#pragma once

#include <switch.h>
#include <vector>

#include "install/content_meta.hpp"
#include "install/install.hpp"
#include "install/simple_filesystem.hpp"
#include "nx/ipc/tin_ipc.h"

namespace tin::install
{
    class IInstallTask
    {
        protected:
            const FsStorageId m_destStorageId;
            bool m_ignoreReqFirmVersion = false;

            std::vector<u8> m_cnmtByteBuf;
            NcmContentRecord m_cnmtContentRecord;
            tin::install::ContentMeta m_contentMeta;
            
            NcmMetaRecord m_metaRecord;
            std::vector<u8> m_installContentMetaData;

            IInstallTask(FsStorageId destStorageId, bool ignoreReqFirmVersion);

            virtual void ReadCNMT() = 0;
            virtual void ParseCNMT();

            virtual void WriteRecords();
            virtual void InstallTicketCert() = 0;
            virtual void InstallNCA(const NcmNcaId &ncaId) = 0;
            virtual void InstallCNMT() = 0;

        public:
            virtual void PrepareForInstall();
            virtual void Install();

            virtual void DebugPrintInstallData();
    };
}