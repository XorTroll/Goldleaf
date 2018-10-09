#pragma once

extern "C"
{
#include <switch/services/fs.h>
}

#include <memory>
#include <tuple>
#include <vector>

#include "install/simple_filesystem.hpp"
#include "data/byte_buffer.hpp"

#include "nx/content_meta.hpp"
#include "nx/ipc/tin_ipc.h"

namespace tin::install
{
    class Install
    {
        protected:
            const FsStorageId m_destStorageId;
            bool m_ignoreReqFirmVersion = false;

            nx::ncm::ContentMeta m_contentMeta;

            Install(FsStorageId destStorageId, bool ignoreReqFirmVersion);
            ~Install();

            virtual std::tuple<nx::ncm::ContentMeta, nx::ncm::ContentRecord> ReadCNMT() = 0;

            virtual void InstallContentMetaRecords(tin::data::ByteBuffer& installContentMetaBuf);
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