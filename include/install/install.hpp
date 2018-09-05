#pragma once

#include <switch.h>
#include <memory>
#include <vector>

#include "install/install.hpp"
#include "install/simple_filesystem.hpp"
#include "util/byte_buffer.hpp"

#include "nx/content_meta.hpp"
#include "nx/ipc/tin_ipc.h"

namespace tin::install
{
    class Install
    {
        protected:
            const FsStorageId m_destStorageId;
            bool m_ignoreReqFirmVersion = false;

            std::unique_ptr<nx::ncm::ContentMeta> m_contentMeta;

            Install(FsStorageId destStorageId, bool ignoreReqFirmVersion);
            ~Install();

            virtual void ReadCNMT(nx::ncm::ContentRecord* cnmtContentRecordOut, tin::util::ByteBuffer& byteBuffer) = 0;

            virtual void InstallContentMetaRecords(tin::util::ByteBuffer& installContentMetaBuf);
            virtual void InstallApplicationRecord();
            virtual void InstallTicketCert() = 0;
            virtual void InstallNCA(const NcmNcaId &ncaId) = 0;

        public:
            virtual void Prepare();
            virtual void Begin();

            virtual u64 GetTitleId();
            virtual nx::ncm::ContentMetaType GetContentMetaType();

            virtual void DebugPrintInstallData();
    };
}