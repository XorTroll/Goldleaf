#pragma once

#include <string>
#include <memory>
#include <switch.h>
#include "ncm.h"

std::string getMetaRecordTypeName(u8 type);
std::string getContentRecordTypeName(u8 type);

class Title
{
    public:
        NCMMetaRecord m_baseMetaRecord;
        NCMMetaRecord m_updateMetaRecord;
        FsStorageId m_storageId;

        Title(NCMMetaRecord baseMetaRecord, NCMMetaRecord updateMetaRecord, FsStorageId storageId);

        std::string getName();
        std::shared_ptr<NsApplicationControlData> getApplicationControlData();

    private:
        std::string m_name;
        std::shared_ptr<NsApplicationControlData> m_applicationControlData;
};