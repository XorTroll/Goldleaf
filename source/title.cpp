#include "title.hpp"

Title::Title(u64 titleId, FsStorageId storageId) :
    m_titleId(titleId), m_storageId(storageId), m_name(""), m_applicationControlData(nullptr)
{

}

std::string Title::getName()
{
    if (this->m_name != "")
        return this->m_name;

    auto appControlData = this->getApplicationControlData();
    NacpLanguageEntry *languageEntry;
    Result rc = 0;

    if (R_FAILED(rc = nacpGetLanguageEntry(&appControlData->nacp, &languageEntry)))
    {
        printf("Title::getName: Failed to get language entry. Error code: 0x%08x\n", rc);
        return "";
    }

    if (languageEntry == NULL)
    {
        printf("Title::getName: language entry cannot be null\n");
        return "";
    }

    this->m_name = languageEntry->name;
    return this->m_name;
}

std::shared_ptr<NsApplicationControlData> Title::getApplicationControlData()
{
    if (this->m_applicationControlData != nullptr)
        return this->m_applicationControlData;

    Result rc = 0;
    size_t sizeRead;

    this->m_applicationControlData = std::make_shared<NsApplicationControlData>();

    if (R_FAILED(rc = nsGetApplicationControlData(0x1, this->m_titleId, this->m_applicationControlData.get(), sizeof(NsApplicationControlData), &sizeRead)))
    {
        printf("Title::getApplicationControlData: Failed to get application control data. Error code: 0x%08x\n", rc);
        // TODO: Error properly
        this->m_applicationControlData = nullptr;
    }
    else if (sizeRead < sizeof(this->m_applicationControlData->nacp))
    {
        printf("Incorrect size for nsacp\n");
        // TODO: Error properly
        this->m_applicationControlData = nullptr;
    }

    return this->m_applicationControlData;
}