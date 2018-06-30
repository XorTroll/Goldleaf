#pragma once

#include <string>
#include <memory>
#include <switch.h>
#include "ncm.h"

std::string getContentRecordTypeName(u8 type);

class Title
{
    public:
        const NCMMetaRecord m_baseMetaRecord;
        const NCMMetaRecord m_updateMetaRecord;
        const FsStorageId m_storageId;

        Title(NCMMetaRecord baseMetaRecord, NCMMetaRecord updateMetaRecord, FsStorageId storageId);

        std::string getName();
        std::shared_ptr<NsApplicationControlData> getApplicationControlData();

    private:
        std::string m_name;
        std::shared_ptr<NsApplicationControlData> m_applicationControlData;
};