#pragma once

#ifdef __cplusplus

#include <memory>
#include <switch.h>
#include "install/game_container.hpp"
#include "nx/ipc/tin_ipc.h"

extern "C" {
#endif

#include "install/install_source.h"

Result installTitle(InstallContext *context);

#ifdef __cplusplus
}

namespace tin::install
{
    class InstallTask final
    {
        public:
            InstallTask(std::unique_ptr<IGameContainer>& gameContainer, FsStorageId destStorageId);

            // Prepare records and perform verification, ready for installation
            Result PrepareForInstall();
            Result Install();

        private:
            const std::unique_ptr<IGameContainer> m_gameContainer;
            const FsStorageId m_destStorageId;

            Result InstallNCA(const NcmNcaId& ncaId);
            Result WriteRecords(const NcmMetaRecord *metaRecord, NcmContentRecord* records, size_t numRecords);
            Result InstallTicketCert();
    };
};

#endif