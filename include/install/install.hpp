#pragma once

#ifdef __cplusplus

#include <memory>
#include <switch.h>
#include "nx/ipc/tin_ipc.h"
#include "install/simple_filesystem.hpp"

extern "C" {
#endif

#include "install/install_source.h"

Result installTitle(InstallContext *context);

#ifdef __cplusplus
}

namespace tin::install
{
    class IInstallTask
    {
        protected:
            const FsStorageId m_destStorageId;
        
            IInstallTask(FsStorageId destStorageId);

        public:
            virtual Result PrepareForInstall() = 0;
            virtual Result Install() = 0;
    };

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

#endif