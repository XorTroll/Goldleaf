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

        protected:
            void ReadCNMT() override;
            void InstallNCA(const NcmNcaId& ncaId) override;
            void InstallTicketCert() override;
            void InstallCNMT() override;

        public:
            NSPInstallTask(tin::install::nsp::SimpleFileSystem& simpleFileSystem, FsStorageId destStorageId, bool ignoreReqFirmVersion);
    };
};

