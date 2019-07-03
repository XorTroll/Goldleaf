#include <hos/hos_Titles.hpp>
#include <hos/hos_Content.hpp>
#include <fs/fs_Explorer.hpp>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <sys/stat.h>
#include <dirent.h>

namespace hos
{
    std::string ContentId::GetFileName()
    {
        return hos::ContentIdAsString(this->NCAId) + ".nca";
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
        else
        {
            rc = nsGetApplicationControlData(1, GetBaseApplicationId(this->ApplicationId, this->Type), ctdata, sizeof(NsApplicationControlData), &acsz);
            if((rc == 0) && !(acsz < sizeof(ctdata->nacp)))
            {
                nacp = (NacpStruct*)malloc(sizeof(NacpStruct));
                memcpy(nacp, &ctdata->nacp, sizeof(NacpStruct));
            }
        }
        free(ctdata);
        ctdata = NULL;
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
        else
        {
            rc = nsGetApplicationControlData(1, GetBaseApplicationId(this->ApplicationId, this->Type), ctdata, sizeof(NsApplicationControlData), &acsz);
            if((rc == 0) && !(acsz < sizeof(ctdata->nacp)))
            {
                icon = (u8*)malloc(0x20000);
                memcpy(icon, ctdata->icon, 0x20000);
            }
        }
        free(ctdata);
        ctdata = NULL;
        return icon;
    }

    bool Title::DumpControlData()
    {
        bool hicon = false;
        std::string fappid = FormatApplicationId(this->ApplicationId);
        NacpStruct *nacp = this->TryGetNACP();
        auto sdexp = fs::GetSdCardExplorer();
        if(nacp != NULL)
        {
            std::string fnacp = GoldleafDir + "/title/" + fappid + ".nacp";
            sdexp->DeleteFile(fnacp);
            sdexp->WriteFileBlock(fnacp, (u8*)nacp, sizeof(NacpStruct));
            free(nacp);
            nacp = NULL;
        }
        u8 *jpg = this->TryGetIcon();
        if(jpg != NULL)
        {
            std::string fjpg = GoldleafDir + "/title/" + fappid + ".jpg";
            sdexp->DeleteFile(fjpg);
            sdexp->WriteFileBlock(fjpg, jpg, 0x20000);
            free(jpg);
            jpg = NULL;
            hicon = true;
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
                    memcpy(&t.Record, &recs[i], sizeof(NcmMetaRecord));
                    titles.push_back(t);
                }
            }
            free(recs);
            recs = NULL;
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
        ts.clear();
        return ex;
    }

    Result RemoveTitle(Title &ToRemove)
    {
        auto cnts = ToRemove.GetContents();
        NcmContentStorage cst;
        Result rc = ncmOpenContentStorage(static_cast<FsStorageId>(ToRemove.Location), &cst);
        if(rc == 0)
        {
            if(!cnts.Meta.Empty) ncmContentStorageDelete(&cst, &cnts.Meta.NCAId);
            if(!cnts.Program.Empty) ncmContentStorageDelete(&cst, &cnts.Program.NCAId);
            if(!cnts.Data.Empty) ncmContentStorageDelete(&cst, &cnts.Data.NCAId);
            if(!cnts.Control.Empty) ncmContentStorageDelete(&cst, &cnts.Control.NCAId);
            if(!cnts.HtmlDocument.Empty) ncmContentStorageDelete(&cst, &cnts.HtmlDocument.NCAId);
            if(!cnts.LegalInfo.Empty) ncmContentStorageDelete(&cst, &cnts.LegalInfo.NCAId);
        }
        serviceClose(&cst.s);
        NcmContentMetaDatabase metadb;
        rc = ncmOpenContentMetaDatabase(static_cast<FsStorageId>(ToRemove.Location), &metadb);
        if(rc == 0)
        {
            rc = ncmContentMetaDatabaseRemove(&metadb, &ToRemove.Record);
            if(rc == 0) ncmContentMetaDatabaseCommit(&metadb);
        }
        serviceClose(&metadb.s);
        if(rc == 0) ns::DeleteApplicationRecord(ToRemove.ApplicationId);
        return rc;
    }

    Result RemoveTicket(Ticket &ToRemove)
    {
        return es::DeleteTicket(&ToRemove.RId, sizeof(es::RightsId));
    }

    std::vector<Ticket> GetAllTickets()
    {
        std::vector<Ticket> tickets;
        u32 wrt = 0;

        u32 cc = 0;
        es::CountCommonTicket(&cc);
        if(cc > 0)
        {
            auto sz = cc * sizeof(es::RightsId);
            es::RightsId *ids = (es::RightsId*)malloc(sz);
            memset(ids, 0, sz);
            es::ListCommonTicket(&wrt, ids, sz);
            for(u32 i = 0; i < cc; i++) tickets.push_back({ ids[i], hos::TicketType::Common });
            free(ids);
        }

        u32 pc = 0;
        es::CountPersonalizedTicket(&pc);
        if(cc > 0)
        {
            auto sz = pc * sizeof(es::RightsId);
            es::RightsId *ids = (es::RightsId*)malloc(sz);
            memset(ids, 0, sz);
            es::ListPersonalizedTicket(&wrt, ids, sz);
            for(u32 i = 0; i < pc; i++) tickets.push_back({ ids[i], hos::TicketType::Personalized });
            free(ids);
        }

        return tickets;
    }

    std::string GetExportedIconPath(u64 ApplicationId)
    {
        return "sdmc:/" + GoldleafDir + "/title/" + FormatApplicationId(ApplicationId) + ".jpg";
    }

    std::string GetExportedNACPPath(u64 ApplicationId)
    {
        return "sdmc:/" + GoldleafDir + "/title/" + FormatApplicationId(ApplicationId) + ".nacp";
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
        auto fexp = fs::GetExplorerForMountName(fs::GetPathRoot(Path));
        TicketData tik;
        u64 off = 0;
        u32 tiksig = 0;
        fexp->ReadFileBlock(Path, off, sizeof(u32), (u8*)&tiksig);
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
        off = tikdata + 0x40;
        u8 tkey[0x10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
        fexp->ReadFileBlock(Path, off, 0x10, tkey);
        std::stringstream strm;
        strm << std::uppercase << std::setfill('0') << std::hex;
        for(u32 i = 0; i < 0x10; i++) strm << (u32)tkey[i];
        tik.TitleKey = strm.str();
        off = tikdata + 0x160 + 0xf;
        u8 kgen = 0;
        fexp->ReadFileBlock(Path, off, 1, &kgen);
        tik.KeyGeneration = kgen;
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