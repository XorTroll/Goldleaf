#pragma once

#include <string>
#include <memory>
#include <switch.h>

class Title
{
    public:
        const u64 m_titleId;
        const FsStorageId m_storageId;

        Title(u64 titleId, FsStorageId storageId);

        std::string getName();
        std::shared_ptr<NsApplicationControlData> getApplicationControlData();

    private:
        std::string m_name;
        std::shared_ptr<NsApplicationControlData> m_applicationControlData;
};