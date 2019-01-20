#include <gleaf/horizon/Title.hpp>
#include <gleaf/fs.hpp>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <sys/stat.h>
#include <dirent.h>

namespace gleaf::horizon
{
    std::string Title::GetExportedIconPath()
    {
        return ("sdmc:/goldleaf/title/" + FormatApplicationId(this->ApplicationId) + ".jpg");
    }

    std::string Title::GetExportedNACPPath()
    {
        return ("sdmc:/goldleaf/title/" + FormatApplicationId(this->ApplicationId) + ".nacp");
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
        size_t size = sizeof(NcmApplicationContentMetaKey) * 128;
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
                        if(cdata->icon != NULL)
                        {
                            fs::DeleteFile("sdmc:/goldleaf/title/" + fappid + ".jpg");
                            FILE *f = fopen(("sdmc:/goldleaf/title/" + fappid + ".jpg").c_str(), "wb");
                            fwrite(cdata->icon, sizeof(u8), sizeof(cdata->icon), f);
                            fclose(f);
                        }
                        if(&cdata->nacp != NULL)
                        {
                            fs::DeleteFile("sdmc:/goldleaf/title/" + fappid + ".nacp");
                            FILE *f = fopen(("sdmc:/goldleaf/title/" + fappid + ".nacp").c_str(), "wb");
                            fwrite(&cdata->nacp, sizeof(NacpStruct), 1, f);
                            fclose(f);
                        }
                    }
                    free(cdata);
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
                        if(cdata->icon != NULL)
                        {
                            fs::DeleteFile("sdmc:/goldleaf/title/" + fappid + ".jpg");
                            FILE *f = fopen(("sdmc:/goldleaf/title/" + fappid + ".jpg").c_str(), "wb");
                            fwrite(cdata->icon, sizeof(u8), sizeof(cdata->icon), f);
                            fclose(f);
                        }
                        if(&cdata->nacp != NULL)
                        {
                            fs::DeleteFile("sdmc:/goldleaf/title/" + fappid + ".nacp");
                            FILE *f = fopen(("sdmc:/goldleaf/title/" + fappid + ".nacp").c_str(), "wb");
                            fwrite(&cdata->nacp, sizeof(NacpStruct), 1, f);
                            fclose(f);
                        }
                    }
                    free(cdata);
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
                        if(cdata->icon != NULL)
                        {
                            fs::DeleteFile("sdmc:/goldleaf/title/" + fappid + ".jpg");
                            FILE *f = fopen(("sdmc:/goldleaf/title/" + fappid + ".jpg").c_str(), "wb");
                            fwrite(cdata->icon, sizeof(u8), sizeof(cdata->icon), f);
                            fclose(f);
                        }
                        if(&cdata->nacp != NULL)
                        {
                            fs::DeleteFile("sdmc:/goldleaf/title/" + fappid + ".nacp");
                            FILE *f = fopen(("sdmc:/goldleaf/title/" + fappid + ".nacp").c_str(), "wb");
                            fwrite(&cdata->nacp, sizeof(NacpStruct), 1, f);
                            fclose(f);
                        }
                    }
                    free(cdata);
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
        u8 cc = std::get<1>(t1);
        std::tuple<Result, u32, es::RightsId*> t2 = es::ListCommonTicket(cc * sizeof(es::RightsId));
        es::RightsId *crids = std::get<2>(t2);
        std::vector<es::RightsId> vcrids;
        if(cc > 0) vcrids = std::vector<es::RightsId>(crids, crids + cc);
        t1 = es::CountPersonalizedTicket();
        u8 pc = std::get<1>(t1);
        t2 = es::ListPersonalizedTicket(pc * sizeof(es::RightsId));
        es::RightsId *prids = std::get<2>(t2);
        std::vector<es::RightsId> vprids;
        if(pc > 0) vprids = std::vector<es::RightsId>(prids, prids + pc); 
        if(cc > 0) for(u32 i = 0; i < vcrids.size(); i++) tickets.push_back({ vcrids[i], gleaf::horizon::TicketType::Common });
        if(pc > 0) for(u32 i = 0; i < vprids.size(); i++) tickets.push_back({ vprids[i], gleaf::horizon::TicketType::Personalized });
        free(crids);
        free(prids);
        return tickets;
    }

    std::string GetTitleNCAPath(u64 ApplicationId)
    {
        std::string pth = "";
        LrLocationResolver lres;
        Result rc = lrOpenLocationResolver(FsStorageId_NandSystem, &lres);
        if(rc != 0) return pth;
        char cpath[FS_MAX_PATH] = { 0 };
        rc = lrLrResolveProgramPath(&lres, ApplicationId, cpath);
        if(rc == 0) pth = gleaf::fs::GetFileName(std::string(cpath));
        serviceClose(&lres.s);
        return pth;
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

    ApplicationIdMask IsValidApplicationId(u64 ApplicationId)
    {
        ApplicationIdMask mas = ApplicationIdMask::Invalid;
        std::string fappid = FormatApplicationId(ApplicationId);
        char fst = fappid[0];
        if(fst != '0') return mas;
        char mask = fappid[1];
        if(mask == '1') mas = ApplicationIdMask::Official;
        else if(mask == '5') mas = ApplicationIdMask::Homebrew;
        return mas;
    }

    TicketData ReadTicket(std::string Path)
    {
        TicketData tik;
        std::ifstream ifs(Path, std::ios::binary);
        if(ifs.good())
        {
            u32 tiksig = 0;
            ifs.read((char*)&tiksig, sizeof(u32));
            tik.Signature = static_cast<TicketSignature>(tiksig);
            u32 sigsz = 0;
            u32 padsz = 0;
            switch(tiksig)
            {
                case 0x10000:
                    sigsz = 0x200;
                    padsz = 0x3c;
                    break;
                case 0x10001:
                    sigsz = 0x100;
                    padsz = 0x3c;
                    break;
                case 0x10002:
                    sigsz = 0x3c;
                    padsz = 0x40;
                    break;
                case 0x10003:
                    sigsz = 0x200;
                    padsz = 0x3c;
                    break;
                case 0x10004:
                    sigsz = 0x100;
                    padsz = 0x3c;
                    break;
                case 0x10005:
                    sigsz = 0x3c;
                    padsz = 0x40;
                    break;
            }
            u32 tikdata = (4 + sigsz + padsz);
            ifs.seekg(tikdata + 0x40, std::ios::beg);
            u8 tkey[0x10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
            ifs.read((char*)tkey, 0x10);
            std::stringstream strm;
            strm << std::uppercase << std::setfill('0') << std::hex;
            for(u32 i = 0; i < 0x10; i++) strm << (u32)tkey[i];
            tik.TitleKey = strm.str();
            ifs.seekg(tikdata + 0x160 + 0xf, std::ios::beg);
            u8 kgen = 0;
            ifs.read((char*)&kgen, 1);
            tik.KeyGeneration = kgen;
        }
        ifs.close();
        return tik;
    }
}