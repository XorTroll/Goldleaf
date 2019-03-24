#include <gleaf/horizon/Title.hpp>
#include <gleaf/horizon/NCAId.hpp>
#include <gleaf/fs.hpp>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <sys/stat.h>
#include <dirent.h>

namespace gleaf::horizon
{
    std::string ContentId::GetFileName()
    {
        return horizon::GetStringFromNCAId(this->NCAId) + ".nca";
    }

    std::string ContentId::GetFullPath()
    {
        std::string path;
        NcmContentStorage cst;
        Result rc = ncmOpenContentStorage(static_cast<FsStorageId>(this->Location), &cst);
        if(rc == 0)
        {
            char pout[FS_MAX_PATH] = { 0 };
            rc = ncmContentStorageGetPath(&cst, &this->NCAId, pout, FS_MAX_PATH);
            if(rc == 0) path = std::string(pout);
        }
        serviceClose(&cst.s);
        return path;
    }

    u64 TitleContents::GetTotalSize()
    {
        return (this->Meta.Size + this->Program.Size + this->Data.Size + this->Control.Size + this->HtmlDocument.Size + this->LegalInfo.Size);
    }

    std::string TitleContents::GetFormattedTotalSize()
    {
        return fs::FormatSize(this->GetTotalSize());
    }

    NacpStruct *Title::TryGetNACP()
    {
        NacpStruct *nacp = NULL;
        NsApplicationControlData *ctdata = (NsApplicationControlData*)malloc(sizeof(NsApplicationControlData));
        size_t acsz = 0;
        Result rc = nsGetApplicationControlData(1, this->ApplicationId, ctdata, sizeof(NsApplicationControlData), &acsz);
        if((rc == 0) && !(acsz < sizeof(ctdata->nacp)))
        {
            nacp = (NacpStruct*)malloc(sizeof(NacpStruct));
            memcpy(nacp, &ctdata->nacp, sizeof(NacpStruct));
        }
        free(ctdata);
        return nacp;
    }

    u8 *Title::TryGetIcon()
    {
        u8 *icon = NULL;
        NsApplicationControlData *ctdata = (NsApplicationControlData*)malloc(sizeof(NsApplicationControlData));
        size_t acsz = 0;
        Result rc = nsGetApplicationControlData(1, this->ApplicationId, ctdata, sizeof(NsApplicationControlData), &acsz);
        if((rc == 0) && !(acsz < sizeof(ctdata->nacp)))
        {
            icon = (u8*)malloc(0x20000);
            memcpy(icon, ctdata->icon, 0x20000);
        }
        free(ctdata);
        return icon;
    }

    bool Title::DumpControlData()
    {
        bool hicon = false;
        std::string fappid = FormatApplicationId(this->ApplicationId);
        NacpStruct *nacp = this->TryGetNACP();
        if(nacp != NULL)
        {
            fs::DeleteFile("sdmc:/goldleaf/title/" + fappid + ".nacp");
            FILE *f = fopen(("sdmc:/goldleaf/title/" + fappid + ".nacp").c_str(), "wb");
            fwrite(nacp, sizeof(NacpStruct), 1, f);
            fclose(f);
            free(nacp);
        }
        u8 *jpg = this->TryGetIcon();
        if(jpg != NULL)
        {
            hicon = true;
            fs::DeleteFile("sdmc:/goldleaf/title/" + fappid + ".jpg");
            FILE *f = fopen(("sdmc:/goldleaf/title/" + fappid + ".jpg").c_str(), "wb");
            fwrite(jpg, 0x20000, 1, f);
            fclose(f);
            free(jpg);
        }
        return hicon;
    }

    TitleContents Title::GetContents()
    {
        TitleContents cnts;
        memset(&cnts, 0, sizeof(cnts));
        NcmContentMetaDatabase metadb;
        NcmContentStorage cst;
        Result rc = ncmOpenContentMetaDatabase(static_cast<FsStorageId>(this->Location), &metadb);
        if(rc == 0)
        {
            rc = ncmOpenContentStorage(static_cast<FsStorageId>(this->Location), &cst);
            if(rc == 0) for(u32 i = 0; i < 6; i++)
            {
                ContentId cntid;
                memset(&cntid, 0, sizeof(cntid));
                cntid.Type = static_cast<ncm::ContentType>(i);
                cntid.Empty = true;
                cntid.Size = 0;
                cntid.Location = this->Location;
                NcmNcaId ncaid;
                rc = ncmContentMetaDatabaseGetContentIdByType(&metadb, (NcmContentType)i, &this->Record, &ncaid);
                if(rc == 0)
                {
                    cntid.Empty = false;
                    cntid.NCAId = ncaid;
                    ncmContentStorageGetSize(&cst, &ncaid, &cntid.Size);
                }
                if(i == 0) cnts.Meta = cntid;
                else if(i == 1) cnts.Program = cntid;
                else if(i == 2) cnts.Data = cntid;
                else if(i == 3) cnts.Control = cntid;
                else if(i == 4) cnts.HtmlDocument = cntid;
                else if(i == 5) cnts.LegalInfo = cntid;
            }
        }
        serviceClose(&cst.s);
        serviceClose(&metadb.s);
        return cnts;
    }

    bool Title::IsBaseTitle()
    {
        return ((!this->IsUpdate()) && (!this->IsDLC()) &&(this->Type != ncm::ContentMetaType::SystemUpdate) && (this->Type != ncm::ContentMetaType::Delta));
    }

    bool Title::IsUpdate()
    {
        return (this->Type == ncm::ContentMetaType::Patch);
    }

    bool Title::IsDLC()
    {
        return (this->Type == ncm::ContentMetaType::AddOnContent);
    }

    bool Title::CheckBase(Title &Other)
    {
        return ((!Other.IsBaseTitle()) && (this->ApplicationId == GetBaseApplicationId(Other.ApplicationId, Other.Type)));
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

    std::vector<Title> SearchTitles(ncm::ContentMetaType Type, Storage Location)
    {
        std::vector<Title> titles;
        NcmContentMetaDatabase metadb;
        Result rc = ncmOpenContentMetaDatabase(static_cast<FsStorageId>(Location), &metadb);
        if(rc == 0)
        {
            u32 srecs = 128 * sizeof(NcmMetaRecord);
            NcmMetaRecord *recs = (NcmMetaRecord*)malloc(srecs);
            u32 wrt = 0;
            u32 total = 0;
            rc = ncmContentMetaDatabaseList(&metadb, static_cast<u32>(Type), 0, 0, UINT64_MAX, recs, srecs, &wrt, &total);
            if((rc == 0) && (wrt > 0))
            {
                titles.reserve(wrt);
                for(u32 i = 0; i < wrt; i++)
                {
                    Title t;
                    memset(&t, 0, sizeof(t));
                    t.ApplicationId = recs[i].titleId;
                    t.Type = static_cast<ncm::ContentMetaType>(recs[i].type);
                    t.Version = recs[i].version;
                    t.Location = Location;
                    t.Record = recs[i];
                    titles.push_back(t);
                }
            }
            free(recs);
            serviceClose(&metadb.s);
        }
        return titles;
    }

    Title Locate(u64 ApplicationId)
    {
        Title tit;
        memset(&tit, 0, sizeof(tit));
        std::vector<Title> titles = SearchTitles(ncm::ContentMetaType::Any, Storage::NANDSystem);
        if(!titles.empty()) for(u32 i = 0; i < titles.size(); i++)
        {
            if(titles[i].ApplicationId == ApplicationId)
            {
                tit = titles[i];
                break;
            }
        }
        if(tit.ApplicationId == 0)
        {
            titles.clear();
            titles = SearchTitles(ncm::ContentMetaType::Any, Storage::NANDUser);
            if(!titles.empty()) for(u32 i = 0; i < titles.size(); i++)
            {
                if(titles[i].ApplicationId == ApplicationId)
                {
                    tit = titles[i];
                    break;
                }
            }
        }
        if(tit.ApplicationId == 0)
        {
            titles.clear();
            titles = SearchTitles(ncm::ContentMetaType::Any, Storage::SdCard);
            if(!titles.empty()) for(u32 i = 0; i < titles.size(); i++)
            {
                if(titles[i].ApplicationId == ApplicationId)
                {
                    tit = titles[i];
                    break;
                }
            }
        }
        titles.clear();
        return tit;
    }

    bool ExistsTitle(ncm::ContentMetaType Type, Storage Location, u64 ApplicationId)
    {
        bool ex = false;
        std::vector<Title> ts = SearchTitles(Type, Location);
        for(u32 i = 0; i < ts.size(); i++) if(ts[i].ApplicationId == ApplicationId)
        {
            ex = true;
            break;
        }
        return ex;
    }

    Result RemoveTitle(Title &ToRemove)
    {
        NcmContentMetaDatabase metadb;
        Result rc = ncmOpenContentMetaDatabase(static_cast<FsStorageId>(ToRemove.Location), &metadb);
        if(rc == 0)
        {
            rc = ncmContentMetaDatabaseRemove(&metadb, &ToRemove.Record);
            if(rc == 0)
            {
                rc = ncmContentMetaDatabaseCommit(&metadb);
                if(rc == 0) if(ToRemove.Type == ncm::ContentMetaType::Application) rc = ns::DeleteApplicationRecord(ToRemove.ApplicationId);
            }
        }
        serviceClose(&metadb.s);
        return rc;
    }

    std::vector<Ticket> GetAllTickets()
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

    std::string GetExportedIconPath(u64 ApplicationId)
    {
        return ("sdmc:/goldleaf/title/" + FormatApplicationId(ApplicationId) + ".jpg");
    }

    std::string GetExportedNACPPath(u64 ApplicationId)
    {
        return ("sdmc:/goldleaf/title/" + FormatApplicationId(ApplicationId) + ".nacp");
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
        std::string fappid = FormatApplicationId(ApplicationId);
        std::string ids = fappid.substr(0, 2);
        if(ids == "01") return ApplicationIdMask::Official;
        else if(ids == "05") return ApplicationIdMask::Homebrew;
        return ApplicationIdMask::Invalid;
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

    std::string GetNACPName(NacpStruct *NACP)
    {
        NacpLanguageEntry *lent;
        nacpGetLanguageEntry(NACP, &lent);
        std::string ret;
        if(lent != NULL) ret = std::string(lent->name);
        return ret;
    }

    std::string GetNACPAuthor(NacpStruct *NACP)
    {
        NacpLanguageEntry *lent;
        nacpGetLanguageEntry(NACP, &lent);
        std::string ret;
        if(lent != NULL) ret = std::string(lent->author);
        return ret;
    }

    std::string GetNACPVersion(NacpStruct *NACP)
    {
        return std::string(NACP->version);
    }
}