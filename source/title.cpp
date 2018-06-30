#include "title.hpp"

#include "error.hpp"

std::string getContentRecordTypeName(u8 type)
{
    switch (type)
    {
        case 0x0:
            return "Meta";

        case 0x1:
            return "Program";

        case 0x2:
            return "Data";

        case 0x3:
            return "Control";

        case 0x4:
            return "Offline Manual HTML";

        case 0x5:
            return "Legal HTML";

        case 0x6:
            return "Game Update RomFS Patches";
    }

    return "Unknown";
}

Title::Title(NCMMetaRecord baseMetaRecord, NCMMetaRecord updateMetaRecord, FsStorageId storageId) :
    m_baseMetaRecord(baseMetaRecord), m_updateMetaRecord(updateMetaRecord), m_storageId(storageId), 
    m_name(""), m_applicationControlData(nullptr)
{

}

std::string Title::getName()
{
    if (this->m_name != "")
        return this->m_name;

    auto appControlData = this->getApplicationControlData();

    if (appControlData == nullptr)
    {
        error::log(error::LogLevel::ERROR, "Title::getName", "NsApplicationControlData is null", -1);
        return "Unknown";
    }

    NacpLanguageEntry *languageEntry;
    Result rc = 0;

    if (R_FAILED(rc = nacpGetLanguageEntry(&appControlData->nacp, &languageEntry)))
    {
        error::log(error::LogLevel::ERROR, "Title::getName", "Failed to get language entry", rc);
        return "Unknown";
    }

    if (languageEntry == NULL)
    {
        error::log(error::LogLevel::ERROR, "Title::getName", "Language entry is null", -1);
        return "Unknown";
    }

    return this->m_name = languageEntry->name;
}

std::shared_ptr<NsApplicationControlData> Title::getApplicationControlData()
{
    if (this->m_applicationControlData != nullptr)
        return this->m_applicationControlData;

    Result rc = 0;
    size_t sizeRead;

    this->m_applicationControlData = std::make_shared<NsApplicationControlData>();

    if (R_FAILED(rc = nsGetApplicationControlData(0x1, this->m_baseMetaRecord.titleID, this->m_applicationControlData.get(), sizeof(NsApplicationControlData), &sizeRead)))
    {
        error::log(error::LogLevel::ERROR, "Title::getApplicationControlData", "Failed to get application control data", rc);
        this->m_applicationControlData = nullptr;
    }
    else if (sizeRead < sizeof(this->m_applicationControlData->nacp))
    {
        error::log(error::LogLevel::ERROR, "Title::getApplicationControlData", "Incorrect size for nsacp", -1);
        this->m_applicationControlData = nullptr;
    }

    return this->m_applicationControlData;
}