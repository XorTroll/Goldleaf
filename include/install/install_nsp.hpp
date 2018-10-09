#pragma once

#include <switch/services/fs.h>
#include "install/install.hpp"
#include "install/simple_filesystem.hpp"
#include "nx/content_meta.hpp"
#include "nx/ipc/tin_ipc.h"

namespace tin::install::nsp
{
    class NSPInstallTask : public Install
    {
        private:
            tin::install::nsp::SimpleFileSystem* const m_simpleFileSystem;

        protected:
            std::tuple<nx::ncm::ContentMeta, nx::ncm::ContentRecord> ReadCNMT() override;
            void InstallNCA(const NcmNcaId& ncaId) override;
            void InstallTicketCert() override;

        public:
            NSPInstallTask(tin::install::nsp::SimpleFileSystem& simpleFileSystem, FsStorageId destStorageId, bool ignoreReqFirmVersion);
    };
};

