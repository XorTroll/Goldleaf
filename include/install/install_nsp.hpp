#pragma once

#include <switch.h>
#include "install/install.hpp"
#include "install/simple_filesystem.hpp"
#include "nx/ipc/tin_ipc.h"

namespace tin::install::nsp
{
    class NSPInstallTask : public IInstallTask
    {
        public:
            NSPInstallTask(tin::install::nsp::SimpleFileSystem& simpleFileSystem, FsStorageId destStorageId);

            Result PrepareForInstall() override;
            Result Install() override;

        private:
            tin::install::nsp::SimpleFileSystem* const m_simpleFileSystem;

            Result InstallNCA(const NcmNcaId& ncaId);
            Result WriteRecords(const NcmMetaRecord *metaRecord, NcmContentRecord* records, size_t numRecords);
            Result InstallTicketCert();
    };
};

