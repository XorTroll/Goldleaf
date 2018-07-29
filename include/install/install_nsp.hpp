#pragma once

#include <switch.h>
#include "install/content_meta.hpp"
#include "install/install.hpp"
#include "install/simple_filesystem.hpp"
#include "nx/ipc/tin_ipc.h"

namespace tin::install::nsp
{
    class NSPInstallTask : public IInstallTask
    {
        private:
            tin::install::nsp::SimpleFileSystem* const m_simpleFileSystem;
            tin::install::ContentMeta m_contentMeta;

            NcmMetaRecord m_metaRecord;
            NcmContentRecord m_cnmtContentRecord;
            std::vector<u8> m_installContentMetaData;

        public:
            NSPInstallTask(tin::install::nsp::SimpleFileSystem& simpleFileSystem, FsStorageId destStorageId);

            void PrepareForInstall() override;
            void Install() override;

        private:
            void InstallNCA(const NcmNcaId& ncaId);
            void WriteRecords();
            void InstallTicketCert();
    };
};

