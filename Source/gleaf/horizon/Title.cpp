#include <gleaf/horizon/Title.hpp>

namespace gleaf::horizon
{
    void ExtendedTitle::DumpIconAndNACP(std::string IconPath, std::string NACPPath)
    {

    }

    std::vector<Title> GetAllSystemTitles()
    {
        std::vector<Title> titles;
        NcmContentMetaDatabase *metadb = (NcmContentMetaDatabase*)malloc(sizeof(*metadb));
        size_t size = sizeof(NcmApplicationContentMetaKey) * 32;
        NcmApplicationContentMetaKey *metas = (NcmApplicationContentMetaKey*)malloc(size);
        u32 total = 0;
        u32 written = 0;
        Result rc = ncmOpenContentMetaDatabase(FsStorageId_GameCard, metadb);
        if(rc != 0) ThrowError("Error opening gamecard content meta database.");
        rc = ncmContentMetaDatabaseListApplication(metadb, 0x80, metas, size, &written, &total);
        if(rc != 0) ThrowError("Error listing gamecard content meta database application.");
        if(total > 0) for(u32 i = 0; i < total; i++)
        {
            Title title = { metas[i].metaRecord.titleId, Storage::GameCart };
            titles.push_back(title);
        }
        rc = ncmOpenContentMetaDatabase(FsStorageId_NandUser, metadb);
        if(rc != 0) ThrowError("Error opening NAND content meta database.");
        rc = ncmContentMetaDatabaseListApplication(metadb, 0x80, metas, size, &written, &total);
        if(rc != 0) ThrowError("Error listing NAND content meta database application.");
        if(total > 0) for(u32 i = 0; i < total; i++)
        {
            Title title = { metas[i].metaRecord.titleId, Storage::NAND };
            titles.push_back(title);
        }
        rc = ncmOpenContentMetaDatabase(FsStorageId_SdCard, metadb);
        if(rc != 0) ThrowError("Error opening SD card content meta database.");
        rc = ncmContentMetaDatabaseListApplication(metadb, 0x80, metas, size, &written, &total);
        if(rc != 0) ThrowError("Error listing SD card content meta database application.");
        if(total > 0) for(u32 i = 0; i < total; i++)
        {
            Title title = { metas[i].metaRecord.titleId, Storage::SdCard };
            titles.push_back(title);
        }
        return titles;
    }

    ExtendedTitle LoadExtendedData(Title BaseTitle)
    {
        ExtendedTitle tdata;
        tdata.Base = BaseTitle;
        NsApplicationControlData* cdata = (NsApplicationControlData*)malloc(sizeof(NsApplicationControlData));
        size_t csize = 0;
        Result rc = nsGetApplicationControlData(1, BaseTitle.ApplicationId, cdata, sizeof(NsApplicationControlData), &csize);
        if(rc != 0) ThrowError("Error getting application control data from title Id.");
        if(!(csize < sizeof(cdata->nacp)))
        {
            NacpLanguageEntry *lent;
            rc = nacpGetLanguageEntry(&cdata->nacp, &lent);
            if(rc != 0) ThrowError("Error getting NACP language entry.");
            tdata.Version = std::string((const char*)&cdata->nacp.version);
            tdata.Name = std::string(lent->name);
            tdata.Author = std::string(lent->author);
            tdata.Icon = cdata->icon;
            tdata.NACP = cdata->nacp;
        }
        return tdata;
    }

    u64 GetBaseApplicationId(u64 ApplicationId, ncm::ContentMetaType Type)
    {
        switch(Type)
        {
            case ncm::ContentMetaType::Patch:
                return (ApplicationId ^ 0x800);
            case ncm::ContentMetaType::AddOnContent:
                return ((ApplicationId ^ 0x1000) & ~0xfff);
        }
        return ApplicationId;
    }
}