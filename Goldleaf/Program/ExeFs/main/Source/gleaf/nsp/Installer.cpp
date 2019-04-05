#include <gleaf/nsp/Installer.hpp>
#include <gleaf/err.hpp>
#include <gleaf/fs.hpp>
#include <sys/stat.h>
#include <fstream>
#include <dirent.h>

extern gleaf::set::Settings gsets;

namespace gleaf::nsp
{
    Installer::Installer(Storage Location, std::string Input, bool IgnoreVersion)
    {
        this->stid = static_cast<FsStorageId>(Location);
        this->icnmt = false;
        this->tik = false;
        this->nacps = NULL;
        this->rc = 0;
        this->basetid = 0;
        Input.reserve(FS_MAX_PATH);
        this->rc = fsOpenFileSystemWithId(&this->idfs, 0, FsFileSystemType_ApplicationPackage, Input.c_str());
        if(this->rc != 0)
        {
            this->Finalize();
            return;
        }
        fsdevMountDevice("ginst-nsp", this->idfs);
        std::string tikname = fs::SearchForFileInPath("ginst-nsp:/", "tik");
        if(tikname.empty()) this->tik = false;
        else
        {
            this->tik = true;
            this->stik = "ginst-nsp:/" + tikname;
            this->tikdata = horizon::ReadTicket(this->stik);
        }
        ByteBuffer cnmt;
        std::string cnmtname = fs::SearchForFileInPath("ginst-nsp:/", "cnmt.nca");
        if(cnmtname.empty())
        {
            this->rc = err::Make(err::ErrorDescription::MetaNotFound);
            this->Finalize();
            return;
        }
        std::string ocnmtn = "/" + cnmtname;
        ocnmtn.reserve(FS_MAX_PATH);
        FsFile cnmtfile;
        this->rc = fsFsOpenFile(&this->idfs, ocnmtn.c_str(), FS_OPEN_READ, &cnmtfile);
        if(this->rc != 0)
        {
            this->Finalize();
            return;
        }
        u64 cnmtsize = 0;
        this->rc = fsFileGetSize(&cnmtfile, &cnmtsize);
        fsFileClose(&cnmtfile);
        if(this->rc != 0)
        {
            this->Finalize();
            return;
        }
        fs::Explorer *nsys = fs::GetNANDSystemExplorer();
        fs::CopyFile("ginst-nsp:/" + cnmtname, nsys->FullPathFor("contents/temp/" + cnmtname));
        std::string cnmtabs = "@SystemContent://temp/" + cnmtname;
        ncm::ContentRecord record = { 0 };
        record.NCAId = horizon::GetNCAIdFromString(cnmtname);
        *(u64*)record.Size = (cnmtsize & 0xffffffffffff);
        record.Type = ncm::ContentType::Meta;
        cnmtabs.reserve(FS_MAX_PATH);
        this->rc = fsOpenFileSystemWithId(&this->cnmtfs, 0, FsFileSystemType_ContentMeta, cnmtabs.c_str());
        if(this->rc != 0)
        {
            this->Finalize();
            return;
        }
        fsdevMountDevice("ginst-cnmt", this->cnmtfs);
        std::string fcnmtname = fs::SearchForFileInPath("ginst-cnmt:/", "cnmt");
        if(fcnmtname.empty())
        {
            this->rc = err::Make(err::ErrorDescription::CNMTNotFound);
            return;
        }
        std::string fcname = "ginst-cnmt:/" + fcnmtname;
        u64 fcnmtsize = fs::GetFileSize(fcname);
        ByteBuffer fcnmt;
        fcnmt.Resize(fcnmtsize);
        FILE *ffcnmt = fopen(fcname.c_str(), "rb");
        if(!ffcnmt)
        {
            this->rc = err::Make(err::ErrorDescription::CNMTNotFound);
            return;
        }
        fread(fcnmt.GetData(), 1, fcnmt.GetSize(), ffcnmt);
        fclose(ffcnmt);
        fsdevUnmountDevice("ginst-cnmt");
        this->cmeta = ncm::ContentMeta(fcnmt.GetData(), fcnmt.GetSize());
        NcmContentStorage cst;
        this->rc = ncmOpenContentStorage(stid, &cst);
        if(this->rc != 0)
        {
            this->Finalize();
            return;
        }
        bool hascnmt = false;
        ncmContentStorageHas(&cst, &record.NCAId, &hascnmt);
        serviceClose(&cst.s);
        if(!hascnmt) this->ncas.push_back(record);
        else this->icnmt = true;
        this->cmeta.GetInstallContentMeta(cnmtbuf, record, IgnoreVersion);
        NcmMetaRecord metakey = this->cmeta.GetContentMetaKey();
        this->basetid = horizon::GetBaseApplicationId(metakey.titleId, static_cast<ncm::ContentMetaType>(metakey.type));
        for(auto &record : this->cmeta.GetContentRecords())
        {
            this->ncas.push_back(record);
            if(record.Type == ncm::ContentType::Control)
            {
                std::string cnca = horizon::GetStringFromNCAId(record.NCAId) + ".nca";
                fs::CopyFile("ginst-nsp:/" + cnca, nsys->FullPathFor("contents/temp/" + cnca));
                std::string ncaname = "@SystemContent://temp/" + cnca;
                ncaname.reserve(FS_MAX_PATH);
                FsFileSystem ginstctrl;
                Result crc = fsOpenFileSystemWithId(&ginstctrl, this->basetid, FsFileSystemType_ContentControl, ncaname.c_str());
                if(crc != 0) continue;
                fsdevMountDevice("ginst-ctrl", ginstctrl);
                DIR *d = opendir("ginst-ctrl:/");
                if(!d) continue;
                dirent *dent;
                std::string icon;
                while(true)
                {
                    dent = readdir(d);
                    if(dent == NULL) break;
                    std::string fle = std::string(dent->d_name);
                    if(fle.substr(fle.length() - 3) == "dat")
                    {
                        icon = "ginst-ctrl:/" + fle;
                        break;
                    }
                }
                closedir(d);
                this->icn = "sdmc:/goldleaf/meta/" + horizon::GetStringFromNCAId(record.NCAId) + ".jpg";
                fs::CopyFile(icon, this->icn);
                FILE *f = fopen("ginst-ctrl:/control.nacp", "rb");
                if(f)
                {
                    this->nacps = (NacpStruct*)malloc(sizeof(NacpStruct));
                    fread(this->nacps, sizeof(NacpStruct), 1, f);
                }
                fclose(f);
                fsdevUnmountDevice("ginst-ctrl");
                fs::DeleteFile(nsys->FullPathFor("contents/temp/" + cnca));
            }
        }
        fsdevUnmountDevice("ginst-ctrl");
    }

    Installer::~Installer()
    {
        this->Finalize();
    }

    Result Installer::ProcessRecords()
    {
        NcmContentMetaDatabase metadb;
        NcmMetaRecord metakey = this->cmeta.GetContentMetaKey();
        if(horizon::ExistsTitle(ncm::ContentMetaType::Any, Storage::SdCard, metakey.titleId))
        {
            this->rc = err::Make(err::ErrorDescription::TitleAlreadyInstalled);
            this->Finalize();
            return this->rc;
        }
        if(horizon::ExistsTitle(ncm::ContentMetaType::Any, Storage::NANDUser, metakey.titleId))
        {
            this->rc = err::Make(err::ErrorDescription::TitleAlreadyInstalled);
            this->Finalize();
            return this->rc;
        }
        this->rc = ncmOpenContentMetaDatabase(stid, &metadb);
        if(this->rc != 0)
        {
            serviceClose(&metadb.s);
            this->Finalize();
            return this->rc;
        }
        this->rc = ncmContentMetaDatabaseSet(&metadb, &metakey, cnmtbuf.GetSize(), (NcmContentMetaRecordsHeader*)cnmtbuf.GetData());
        if(this->rc != 0)
        {
            serviceClose(&metadb.s);
            this->Finalize();
            return this->rc;
        }
        this->rc = ncmContentMetaDatabaseCommit(&metadb);
        if(this->rc != 0)
        {
            serviceClose(&metadb.s);
            this->Finalize();
            return this->rc;
        }
        serviceClose(&metadb.s);
        std::vector<ns::ContentStorageRecord> records;
        std::tuple<Result, u32> nst = ns::CountApplicationContentMeta(this->basetid);
        this->rc = std::get<0>(nst);
        if(this->rc == 0x410) this->rc = 0;
        if(this->rc != 0)
        {
            serviceClose(&metadb.s);
            this->Finalize();
            return this->rc;
        }
        u32 cmetacount = std::get<1>(nst);
        /*
        if(cmetacount > 0)
        {
            records.resize(cmetacount);
            size_t csbufs = (cmetacount * sizeof(ns::ContentStorageRecord));
            ns::ContentStorageRecord *csbuf = (ns::ContentStorageRecord*)malloc(csbufs);
            std::tuple<Result, u32, void*> listt = ns::ListApplicationRecordContentMeta(0, this->basetid, csbufs);
            this->rc = std::get<0>(listt);
            if(this->rc != 0)
            {
                this->Finalize();
                return this->rc;
            }
            csbuf = (ns::ContentStorageRecord*)std::get<2>(listt);
            memcpy(records.data(), csbuf, csbufs);
        }
        */
        ns::ContentStorageRecord csrecord;
        csrecord.Record = metakey;
        csrecord.StorageId = stid;
        records.push_back(csrecord);
        ns::DeleteApplicationRecord(this->basetid);
        this->rc = ns::PushApplicationRecord(this->basetid, 3, records.data(), records.size() * sizeof(ns::ContentStorageRecord));
        if(this->rc != 0)
        {
            this->Finalize();
            return this->rc;
        }
        if(this->tik)
        {
            std::ifstream ifs(this->stik, std::ios::binary);
            ifs.seekg(0, ifs.end);
            int sztik = ifs.tellg();
            ifs.seekg(0, ifs.beg);
            auto btik = std::make_unique<u8[]>(sztik);
            ifs.read((char*)btik.get(), sztik);
            ifs.close();
            es::ImportTicket(btik.get(), sztik, es::CertData, 1792);
        }
        return this->rc;
    }

    Result Installer::WriteContents(std::function<void(ncm::ContentRecord Record, u32 Content, u32 ContentCount, int Percentage)> Callback)
    {
        if(!this->ncas.empty()) for(u32 i = 0; i < this->ncas.size(); i++)
        {
            NcmNcaId curid = this->ncas[i].NCAId;
            std::string ncaname = horizon::GetStringFromNCAId(curid);
            FsFile ncafile;
            std::string fncaname = "/" + ncaname + ".nca";
            fncaname.reserve(FS_MAX_PATH);
            this->rc = fsFsOpenFile(&this->idfs, fncaname.c_str(), FS_OPEN_READ, &ncafile);
            if(this->rc == 0) ncaname += ".nca";
            else
            {
                fncaname = "/" + ncaname + ".cnmt.nca";
                fncaname.reserve(FS_MAX_PATH);
                this->rc = fsFsOpenFile(&this->idfs, fncaname.c_str(), FS_OPEN_READ, &ncafile);
                if(this->rc == 0) ncaname += ".cnmt.nca";
            }
            if(this->rc != 0)
            {
                fsFileClose(&ncafile);
                this->Finalize();
                return this->rc;
            }
            ncm::ContentStorage cstorage(stid);
            cstorage.DeletePlaceHolder(curid);
            u64 ncasize = 0;
            this->rc = fsFileGetSize(&ncafile, &ncasize);
            if(this->rc != 0)
            {
                fsFileClose(&ncafile);
                this->Finalize();
                return this->rc;
            }
            u64 noff = 0;
            size_t reads = 0x1000000;
            auto readbuf = std::make_unique<u8[]>(reads);
            cstorage.CreatePlaceHolder(curid, curid, ncasize);
            float progress = 0.0f;
            while(noff < ncasize)
            {
                progress = (float)noff / (float)ncasize;
                Callback(this->ncas[i], i, this->ncas.size(), (int)(progress * 100.0));
                if((noff + reads) >= ncasize) reads = (ncasize - noff);
                size_t rout = 0;
                this->rc = fsFileRead(&ncafile, noff, readbuf.get(), reads, &rout);
                if(this->rc != 0)
                {
                    fsFileClose(&ncafile);
                    cstorage.DeletePlaceHolder(curid);
                    this->Finalize();
                    return this->rc;
                }
                cstorage.WritePlaceHolder(curid, noff, readbuf.get(), rout);
                noff += rout;
            }
            fsFileClose(&ncafile);
            cstorage.Register(curid, curid);
            cstorage.DeletePlaceHolder(curid);
        }
        return this->rc;
    }

    NacpStruct *Installer::GetNACP()
    {
        return this->nacps;
    }

    u64 Installer::GetApplicationId()
    {
        return this->cmeta.GetContentMetaKey().titleId;
    }

    ncm::ContentMetaType Installer::GetContentType()
    {
        return static_cast<ncm::ContentMetaType>(this->cmeta.GetContentMetaKey().type);
    }

    std::vector<ncm::ContentRecord> Installer::GetRecords()
    {
        return this->ncas;
    }

    std::string Installer::GetExportedIconPath()
    {
        return this->icn;
    }

    bool Installer::HasContent(ncm::ContentType Type)
    {
        bool has = false;
        for(u32 i = 0; i < this->ncas.size(); i++) if(this->ncas[i].Type == Type)
        {
            has = true;
            break;
        }
        return has;
    }

    bool Installer::HasTicket()
    {
        return this->tik;
    }

    horizon::TicketData Installer::GetTicketData()
    {
        return this->tikdata;
    }

    bool Installer::IsCNMTAlreadyInstalled()
    {
        return this->icnmt;
    }

    Result Installer::GetLatestResult()
    {
        return this->rc;
    }

    void Installer::Finalize()
    {
        this->ncas.clear();
        fsdevUnmountDevice("ginst-cnmt");
        fsdevUnmountDevice("ginst-ctrl");
        fsdevUnmountDevice("ginst-nsp");
        EnsureDirectories();
        if(this->nacps != NULL)
        {
            free(this->nacps);
            this->nacps = NULL;
        }
    }

    Result Install(std::string Path, fs::Explorer *Exp, Storage Location, std::function<bool(ncm::ContentMetaType Type, u64 ApplicationId, std::string IconPath, NacpStruct *NACP, horizon::TicketData *Tik, std::vector<ncm::ContentRecord> NCAs)> OnInitialProcess, std::function<void()> OnRecordProcess, std::function<void(ncm::ContentRecord Record, u32 Content, u32 ContentCount, u8 Percentage)> OnContentWrite)
    {
        Result rc = 0xf601;
        FsStorageId storage = static_cast<FsStorageId>(Location);
        PFS0 nsp(Exp, Path);
        if(nsp.IsOk())
        {
            rc = 0;
            std::string cnmtnca;
            u32 idxcnmtnca = 0;
            u64 scnmtnca = 0;
            std::string tik;
            u32 idxtik = 0;
            u64 stik = 0;
            auto files = nsp.GetFiles();
            for(u32 i = 0; i < files.size(); i++)
            {
                std::string file = files[i];
                if(fs::GetExtension(file) == "tik")
                {
                    tik = file;
                    idxtik = i;
                    stik = nsp.GetFileSize(i);
                }
                else if(file.substr(file.length() - 8) == "cnmt.nca")
                {
                    cnmtnca = file;
                    idxcnmtnca = i;
                    scnmtnca = nsp.GetFileSize(i);
                }
            }
            std::string icnmtnca = fs::GetFileName(cnmtnca);
            fs::Explorer *nsys = fs::GetNANDSystemExplorer();
            nsys->CreateDirectory("Contents/temp");
            std::string ncnmtnca = nsys->FullPathFor("Contents/temp/" + icnmtnca);
            nsys->DeleteFile(ncnmtnca);
            nsp.SaveFile(idxcnmtnca, nsys, ncnmtnca);
            std::string acnmtnca = "@SystemContent://temp/" + icnmtnca;
            acnmtnca.reserve(FS_MAX_PATH);
            ByteBuffer bcnmt;
            FsFileSystem cnmtncafs;
            std::vector<std::string> cnts;
            rc = fsOpenFileSystem(&cnmtncafs, FsFileSystemType_ContentMeta, acnmtnca.c_str());
            {
                fs::FileSystemExplorer cnmtfs("gnspcnmtnca", "NSP-ContentMeta", &cnmtncafs, true);
                cnts = cnmtfs.GetContents();
                std::string fcnmt;
                for(u32 i = 0; i < cnts.size(); i++)
                {
                    if(fs::GetExtension(cnts[i]) == "cnmt")
                    {
                        fcnmt = cnts[i];
                        break;
                    }
                }
                u64 fcnmtsz = cnmtfs.GetFileSize(fcnmt);
                bcnmt.Resize(fcnmtsz);
                cnmtfs.ReadFileBlock(fcnmt, 0, fcnmtsz, bcnmt.GetData());
            }
            ncm::ContentRecord record = { 0 };
            record.NCAId = horizon::GetNCAIdFromString(icnmtnca);
            *(u64*)record.Size = (scnmtnca & 0xffffffffffff);
            record.Type = ncm::ContentType::Meta;
            ncm::ContentMeta cnmt(bcnmt.GetData(), bcnmt.GetSize());
            NcmMetaRecord mrec = cnmt.GetContentMetaKey();
            if(horizon::ExistsTitle(ncm::ContentMetaType::Any, Storage::SdCard, mrec.titleId))
            {
                rc = err::Make(err::ErrorDescription::TitleAlreadyInstalled);
                return rc;
            }
            if(horizon::ExistsTitle(ncm::ContentMetaType::Any, Storage::NANDUser, mrec.titleId))
            {
                rc = err::Make(err::ErrorDescription::TitleAlreadyInstalled);
                return rc;
            }
            NcmContentStorage cst;
            rc = ncmOpenContentStorage(storage, &cst);
            bool hascnmt = false;
            ncmContentStorageHas(&cst, &record.NCAId, &hascnmt);
            serviceClose(&cst.s);
            std::vector<ncm::ContentRecord> ncas;
            if(!hascnmt) ncas.push_back(record);
            u64 baseappid = horizon::GetBaseApplicationId(mrec.titleId, static_cast<ncm::ContentMetaType>(mrec.type));
            auto recs = cnmt.GetContentRecords();
            NacpStruct nacp;
            memset(&nacp, 0, sizeof(nacp));
            horizon::TicketData tikd;
            if(stik > 0) tikd = horizon::ReadTicket(tik);
            std::string icon;
            for(u32 i = 0; i < recs.size(); i++)
            {
                ncas.push_back(recs[i]);
                if(recs[i].Type == ncm::ContentType::Control)
                {
                    std::string controlnca = horizon::GetStringFromNCAId(recs[i].NCAId) + ".nca";
                    u32 idxcontrolnca = nsp.GetFileIndexByName(controlnca);
                    std::string ncontrolnca = nsys->FullPathFor("Contents/temp/" + controlnca);
                    nsp.SaveFile(idxcontrolnca, nsys, ncontrolnca);
                    std::string acontrolnca = "@SystemContent://temp/" + controlnca;
                    acontrolnca.reserve(FS_MAX_PATH);
                    FsFileSystem controlncafs;
                    rc = fsOpenFileSystemWithId(&controlncafs, baseappid, FsFileSystemType_ContentControl, acontrolnca.c_str());
                    if(rc == 0)
                    {
                        fs::FileSystemExplorer controlfs("gnspcontrolnca", "NSP-Control", &controlncafs, true);
                        cnts = controlfs.GetContents();
                        for(u32 i = 0; i < cnts.size(); i++)
                        {
                            if(fs::GetExtension(cnts[i]) == "dat")
                            {
                                std::string cicon = cnts[i];
                                icon = "sdmc:/goldleaf/meta/" + horizon::GetStringFromNCAId(recs[i].NCAId) + ".jpg";
                                controlfs.CopyFile(cicon, icon);
                                break;
                            }
                        }
                        controlfs.ReadFileBlock("control.nacp", 0, sizeof(NacpStruct), (u8*)&nacp);
                    }
                    rc = 0;
                }
            }
            if(!OnInitialProcess(static_cast<ncm::ContentMetaType>(mrec.type), mrec.titleId, icon, (icon.empty() ? NULL : &nacp), ((stik > 0) ? &tikd : NULL), ncas)) return rc;
            NcmContentMetaDatabase mdb;
            rc = ncmOpenContentMetaDatabase(storage, &mdb);
            if(rc != 0) return rc;
            ByteBuffer ccnmt;
            cnmt.GetInstallContentMeta(ccnmt, record, gsets.IgnoreRequiredFirmwareVersion);
            rc = ncmContentMetaDatabaseSet(&mdb, &mrec, ccnmt.GetSize(), (NcmContentMetaRecordsHeader*)ccnmt.GetData());
            if(rc != 0)
            {
                serviceClose(&mdb.s);
                return rc;
            }
            rc = ncmContentMetaDatabaseCommit(&mdb);
            serviceClose(&mdb.s);
            if(rc != 0) return rc;
            auto res1 = ns::CountApplicationContentMeta(baseappid);
            rc = std::get<0>(res1);
            if(rc == 0x410) rc = 0;
            if(rc != 0) return rc;
            u32 cmetacount = std::get<1>(res1);
            std::vector<ns::ContentStorageRecord> srecs;
            if(cmetacount > 0)
            {
                size_t csbufs = (cmetacount * sizeof(ns::ContentStorageRecord));
                ns::ContentStorageRecord *csbuf = (ns::ContentStorageRecord*)malloc(csbufs);
                auto res2 = ns::ListApplicationRecordContentMeta(0, baseappid, csbuf, csbufs);
                rc = std::get<0>(res2);
                if(rc != 0)
                {
                    free(csbuf);
                    return rc;
                }
                for(u32 i = 0; i < cmetacount; i++) srecs.push_back(csbuf[i]);
                free(csbuf);
            }
            ns::ContentStorageRecord csrecord;
            csrecord.Record = mrec;
            csrecord.StorageId = storage;
            srecs.push_back(csrecord);
            ns::DeleteApplicationRecord(baseappid);
            rc = ns::PushApplicationRecord(baseappid, 3, srecs.data(), srecs.size() * sizeof(ns::ContentStorageRecord));
            if(rc != 0) return rc;
            if(stik > 0)
            {
                ByteBuffer btik;
                btik.Resize(stik);
                nsp.ReadFromFile(idxtik, 0, btik.GetSize(), btik.GetData());
                es::ImportTicket(btik.GetData(), btik.GetSize(), es::CertData, 1792);
            }
            OnRecordProcess();
            for(u32 i = 0; i < ncas.size(); i++)
            {
                ncm::ContentRecord rnca = ncas[i];
                NcmNcaId curid = rnca.NCAId;
                std::string ncaname = horizon::GetStringFromNCAId(curid);
                if(rnca.Type == ncm::ContentType::Meta) ncaname += ".cnmt";
                ncaname += ".nca";
                u32 idxncaname = nsp.GetFileIndexByName(ncaname);
                u64 ncasize = nsp.GetFileSize(idxncaname);
                NcmContentStorage cst;
                ncmOpenContentStorage(storage, &cst);
                ncm::DeletePlaceHolder(&cst, &curid);
                ncm::CreatePlaceHolder(&cst, &curid, &curid, ncasize);
                u64 noff = 0;
                u64 szrem = ncasize;
                u64 reads = 0x400000;
                u8 *rdata = (u8*)malloc(reads);
                float progress = 0.0f;
                while(szrem)
                {
                    u64 tread = std::min(szrem, reads);
                    u64 rbytes = nsp.ReadFromFile(idxncaname, noff, tread, rdata);
                    ncm::WritePlaceHolder(&cst, &curid, noff, rdata, rbytes);
                    noff += rbytes;
                    szrem -= rbytes;
                    progress = (float)noff / (float)ncasize;
                    OnContentWrite(rnca, i, ncas.size(), (u8)(progress * 100.0));
                }
                free(rdata);
                ncmContentStorageRegister(&cst, &curid, &curid);
                ncm::DeletePlaceHolder(&cst, &curid);
                serviceClose(&cst.s);
            }
            nsys->DeleteDirectory("Contents/temp");
        }
        return rc;
    }
}