#pragma once

#ifdef __cplusplus

#include <switch.h>

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
            virtual void PrepareForInstall() = 0;
            virtual void Install() = 0;
    };
}

#endif