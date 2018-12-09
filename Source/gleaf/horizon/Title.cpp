#include <gleaf/horizon/Title.hpp>
#include <sstream>
#include <iomanip>
#include <sys/stat.h>
#include <dirent.h>

namespace gleaf::horizon
{
    std::string Title::GetExportedIconPath()
    {
        return ("sdmc:/switch/.gleaf/title/" + FormatApplicationId(this->ApplicationId) + ".jpg");
    }

    u64 Ticket::GetApplicationId()
    {
        return __bswap64(*(u64*)(this->RId.RId));
    }

    u64 Ticket::GetKeyGeneration()
    {
        return __bswap64(*(u64*)(this->RId.RId + 8));
    }

    std::string Ticket::ToString()
    {
        u64 appid = this->GetApplicationId();
        u64 kgen = this->GetKeyGeneration();
        std::string tostr = FormatApplicationId(appid) + FormatApplicationId(kgen);
        return tostr;
    }

    std::string FormatApplicationId(u64 ApplicationId)
    {
        std::stringstream strm;
        strm << std::uppercase << std::setfill('0') << std::setw(16) << std::hex << ApplicationId;
        return strm.str();
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
        if(rc == 0)
        {
            rc = ncmContentMetaDatabaseListApplication(metadb, 0x80, metas, size, &written, &total);
            if(rc == 0)
            {
                if(total > 0) for(u32 i = 0; i < total; i++)
                {
                    Title title = { 0 };
                    title.ApplicationId = metas[i].metaRecord.titleId;
                    title.Location = Storage::GameCart;
                    NsApplicationControlData* cdata = (NsApplicationControlData*)malloc(sizeof(NsApplicationControlData));
                    size_t csize = 0;
                    rc = nsGetApplicationControlData(1, metas[i].metaRecord.titleId, cdata, sizeof(NsApplicationControlData), &csize);
                    if((rc == 0) && !(csize < sizeof(cdata->nacp)))
                    {
                        NacpLanguageEntry *lent;
                        nacpGetLanguageEntry(&cdata->nacp, &lent);
                        title.Version = std::string((const char*)&cdata->nacp.version);
                        title.Name = std::string(lent->name);
                        title.Author = std::string(lent->author);
                        title.NACP = cdata->nacp;
                        std::string fappid = FormatApplicationId(title.ApplicationId);
                        FILE *f = fopen(("sdmc:/switch/.gleaf/title/" + fappid + ".jpg").c_str(), "wb");
                        fwrite(cdata->icon, sizeof(u8), sizeof(cdata->icon), f);
                        fclose(f);
                    }
                    titles.push_back(title);
                }
            }
            serviceClose(&metadb->s);
        }
        rc = ncmOpenContentMetaDatabase(FsStorageId_NandUser, metadb);
        if(rc == 0)
        {
            rc = ncmContentMetaDatabaseListApplication(metadb, 0x80, metas, size, &written, &total);
            if(rc == 0)
            {
                if(total > 0) for(u32 i = 0; i < total; i++)
                {
                    Title title = { 0 };
                    title.ApplicationId = metas[i].metaRecord.titleId;
                    title.Location = Storage::NAND;
                    NsApplicationControlData* cdata = (NsApplicationControlData*)malloc(sizeof(NsApplicationControlData));
                    size_t csize = 0;
                    rc = nsGetApplicationControlData(1, metas[i].metaRecord.titleId, cdata, sizeof(NsApplicationControlData), &csize);
                    if((rc == 0) && !(csize < sizeof(cdata->nacp)))
                    {
                        NacpLanguageEntry *lent;
                        nacpGetLanguageEntry(&cdata->nacp, &lent);
                        title.Version = std::string((const char*)&cdata->nacp.version);
                        title.Name = std::string(lent->name);
                        title.Author = std::string(lent->author);
                        title.NACP = cdata->nacp;
                        std::string fappid = FormatApplicationId(title.ApplicationId);
                        FILE *f = fopen(("sdmc:/switch/.gleaf/title/" + fappid + ".jpg").c_str(), "wb");
                        fwrite(cdata->icon, sizeof(u8), sizeof(cdata->icon), f);
                        fclose(f);
                    }
                    titles.push_back(title);
                }
            }
            serviceClose(&metadb->s);
        }
        rc = ncmOpenContentMetaDatabase(FsStorageId_SdCard, metadb);
        if(rc == 0)
        {
            rc = ncmContentMetaDatabaseListApplication(metadb, 0x80, metas, size, &written, &total);
            if(rc == 0)
            {
                if(total > 0) for(u32 i = 0; i < total; i++)
                {
                    Title title = { 0 };
                    title.ApplicationId = metas[i].metaRecord.titleId;
                    title.Location = Storage::SdCard;
                    NsApplicationControlData* cdata = (NsApplicationControlData*)malloc(sizeof(NsApplicationControlData));
                    size_t csize = 0;
                    rc = nsGetApplicationControlData(1, metas[i].metaRecord.titleId, cdata, sizeof(NsApplicationControlData), &csize);
                    if((rc == 0) && !(csize < sizeof(cdata->nacp)))
                    {
                        NacpLanguageEntry *lent;
                        nacpGetLanguageEntry(&cdata->nacp, &lent);
                        title.Version = std::string((const char*)&cdata->nacp.version);
                        title.Name = std::string(lent->name);
                        title.Author = std::string(lent->author);
                        title.NACP = cdata->nacp;
                        std::string fappid = FormatApplicationId(title.ApplicationId);
                        FILE *f = fopen(("sdmc:/switch/.gleaf/title/" + fappid + ".jpg").c_str(), "wb");
                        fwrite(cdata->icon, sizeof(u8), sizeof(cdata->icon), f);
                        fclose(f);
                    }
                    titles.push_back(title);
                }
            }
            serviceClose(&metadb->s);
        }
        return titles;
    }

    std::vector<Ticket> GetAllSystemTickets()
    {
        std::vector<Ticket> tickets;
        std::tuple<Result, u8> t1 = es::CountCommonTicket();
        Result rc = std::get<0>(t1);
        u8 cc = std::get<1>(t1);
        std::tuple<Result, u32, es::RightsId*> t2 = es::ListCommonTicket(cc * sizeof(es::RightsId));
        rc = std::get<0>(t2);
        u32 written = std::get<1>(t2);
        es::RightsId *crids = std::get<2>(t2);
        std::vector<es::RightsId> vcrids;
        if(cc > 0) vcrids = std::vector<es::RightsId>(crids, crids + cc);
        t1 = es::CountPersonalizedTicket();
        rc = std::get<0>(t1);
        u8 pc = std::get<1>(t1);
        t2 = es::ListPersonalizedTicket(pc * sizeof(es::RightsId));
        rc = std::get<0>(t2);
        written = std::get<1>(t2);
        es::RightsId *prids = std::get<2>(t2);
        std::vector<es::RightsId> vprids;
        if(pc > 0) vprids = std::vector<es::RightsId>(prids, prids + pc); 
        if(cc > 0) for(u32 i = 0; i < vcrids.size(); i++) tickets.push_back({ vcrids[i], gleaf::horizon::TicketType::Common });
        if(pc > 0) for(u32 i = 0; i < vprids.size(); i++) tickets.push_back({ vprids[i], gleaf::horizon::TicketType::Personalized });
        return tickets;
    }

    u64 GetBaseApplicationId(u64 ApplicationId, ncm::ContentMetaType Type)
    {
        u64 appid = ApplicationId;
        switch(Type)
        {
            case ncm::ContentMetaType::Patch:
                appid = (ApplicationId ^ 0x800);
                break;
            case ncm::ContentMetaType::AddOnContent:
                appid = ((ApplicationId ^ 0x1000) & ~0xfff);
                break;
            default:
                appid = ApplicationId;
                break;
        }
        return appid;
    }
}