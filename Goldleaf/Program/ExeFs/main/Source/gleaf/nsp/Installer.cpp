#include <gleaf/nsp/Installer.hpp>
#include <sys/stat.h>
#include <fstream>
#include <dirent.h>

namespace gleaf::nsp
{
    Installer::Installer(Storage Location, std::string Input, bool IgnoreVersion)
    {
        this->stid = static_cast<FsStorageId>(Location);
        this->icnmt = false;
        this->tik = false;
        this->irc = { 0, InstallerError::Success };
        this->basetid = 0;
        Input.reserve(FS_MAX_PATH);
        Result rc = fsOpenFileSystemWithId(&this->idfs, 0, FsFileSystemType_ApplicationPackage, Input.c_str());
        if(rc == 0x236e02)
        {
            this->irc.Error = rc;
            this->irc.Type = InstallerError::BadNSP;
            this->Finalize();
            return;
        }
        else if(rc != 0)
        {
            this->irc.Error = rc;
            this->irc.Type = InstallerError::NSPOpen;
            this->Finalize();
            return;
        }
        fsdevMountDevice("gnspi", this->idfs);
        std::string tikname = fs::SearchForFile(this->idfs, "", "tik");
        if(tikname == "") this->tik = false;
        else
        {
            this->tik = true;
            this->stik = "gnspi:/" + tikname;
            this->tikdata = horizon::ReadTicket(this->stik);
        }
        ByteBuffer cnmt;
        std::string cnmtname = fs::SearchForFile(this->idfs, "", "cnmt.nca");
        if(cnmtname == "")
        {
            this->irc.Error = rc;
            this->irc.Type = InstallerError::BadCNMTNCA;
            this->Finalize();
            return;
        }
        FsFile cnmtfile;
        cnmtname.reserve(FS_MAX_PATH);
        fsFsOpenFile(&this->idfs, cnmtname.c_str(), FS_OPEN_READ, &cnmtfile);
        u64 cnmtsize = 0;
        fsFileGetSize(&cnmtfile, &cnmtsize);
        std::string cnmtabs = Input + "/" + cnmtname;
        ncm::ContentRecord record;
        fsFileClose(&cnmtfile);
        record.NCAId = horizon::GetNCAIdFromString(cnmtname);
        *(u64*)record.Size = (cnmtsize & 0xffffffffffff);
        record.Type = ncm::ContentType::Meta;
        FsFileSystem cnmtfs;
        cnmtabs.reserve(FS_MAX_PATH);
        rc = fsOpenFileSystemWithId(&cnmtfs, 0, FsFileSystemType_ContentMeta, cnmtabs.c_str());
        if(rc != 0)
        {
            this->irc.Error = rc;
            this->irc.Type = InstallerError::CNMTMCAOpen;
            this->Finalize();
            return;
        }
        std::string fcnmtname = fs::SearchForFile(cnmtfs, "", "cnmt");
        if(fcnmtname == "")
        {
            this->irc.Error = rc;
            this->irc.Type = InstallerError::BadCNMT;
            this->Finalize();
            return;
        }
        FsFile fcnmtfile;
        fcnmtname.reserve(FS_MAX_PATH);
        rc = fsFsOpenFile(&cnmtfs, ("/" + fcnmtname).c_str(), FS_OPEN_READ, &fcnmtfile);
        if(rc != 0)
        {
            this->irc.Error = rc;
            this->irc.Type = InstallerError::CNMTOpen;
            this->Finalize();
        }
        u64 fcnmtsize = 0;
        fsFileGetSize(&fcnmtfile, &fcnmtsize);
        ByteBuffer fcnmt;
        fcnmt.Resize(fcnmtsize);
        size_t rout = 0;
        fsFileRead(&fcnmtfile, 0, fcnmt.GetData(), fcnmt.GetSize(), &rout);
        this->cmeta = ncm::ContentMeta(fcnmt.GetData(), fcnmt.GetSize());
        ncm::ContentStorage cnmts(stid);
        if(!cnmts.Has(record.NCAId)) this->ncas.push_back(record);
        else this->icnmt = true;
        this->cmeta.GetInstallContentMeta(cnmtbuf, record, IgnoreVersion);
        for(auto &record : this->cmeta.GetContentRecords())
        {
            this->ncas.push_back(record);
            if(record.Type == ncm::ContentType::Control)
            {
                std::string ncaname = Input + "/" + horizon::GetStringFromNCAId(record.NCAId) + ".nca";
                ncaname.reserve(FS_MAX_PATH);
                FsFileSystem gnspcnca;
                rc = fsOpenFileSystemWithId(&gnspcnca, this->GetApplicationId(), FsFileSystemType_ContentControl, ncaname.c_str());
                if(rc != 0) continue;
                fsdevMountDevice("gnspcnca", gnspcnca);
                DIR *d = opendir("gnspcnca:/");
                dirent *dent;
                std::string icon;
                while(true)
                {
                    dent = readdir(d);
                    if(dent == NULL) break;
                    std::string fle = std::string(dent->d_name);
                    if(fle.substr(fle.length() - 3) == "dat")
                    {
                        icon = "gnspcnca:/" + fle;
                        break;
                    }
                }
                closedir(d);
                std::ifstream dat(icon);
                this->icn = ("sdmc:/goldleaf/meta/" + horizon::GetStringFromNCAId(record.NCAId) + ".jpg");
                remove(this->icn.c_str());
                std::ofstream out(this->icn);
                out << dat.rdbuf();
                out.close();
                dat.close();
                FILE *f = fopen("gnspcnca:/control.nacp", "rb");
                this->nacps = (NacpStruct*)malloc(sizeof(NacpStruct));
                fread(this->nacps, sizeof(NacpStruct), 1, f);
                fclose(f);
                fsdevUnmountDevice("gnspcnca");
            }
        }
        NcmMetaRecord metakey = this->cmeta.GetContentMetaKey();
        this->basetid = horizon::GetBaseApplicationId(metakey.titleId, static_cast<ncm::ContentMetaType>(metakey.type));
        fsFileClose(&fcnmtfile);
        fsFsClose(&cnmtfs);
    }

    Installer::~Installer()
    {
        this->Finalize();
    }

    InstallerResult Installer::ProcessRecords()
    {
        Result rc = 0;
        NcmContentMetaDatabase metadb;
        NcmMetaRecord metakey = this->cmeta.GetContentMetaKey();
        if(horizon::ExistsTitle(ncm::ContentMetaType::Any, Storage::SdCard, metakey.titleId))
        {
            this->irc.Type = InstallerError::TitleFound;
            this->Finalize();
            return this->irc;
        }
        if(horizon::ExistsTitle(ncm::ContentMetaType::Any, Storage::NANDUser, metakey.titleId))
        {
            this->irc.Type = InstallerError::TitleFound;
            this->Finalize();
            return this->irc;
        }
        rc = ncmOpenContentMetaDatabase(stid, &metadb);
        if(rc != 0)
        {
            this->irc.Error = rc;
            this->irc.Type = InstallerError::MetaDatabaseOpen;
            serviceClose(&metadb.s);
            this->Finalize();
            return this->irc;
        }
        rc = ncmContentMetaDatabaseSet(&metadb, &metakey, cnmtbuf.GetSize(), (NcmContentMetaRecordsHeader*)cnmtbuf.GetData());
        if(rc != 0)
        {
            this->irc.Error = rc;
            this->irc.Type = InstallerError::MetaDatabaseSet;
            serviceClose(&metadb.s);
            this->Finalize();
            return this->irc;
        }
        rc = ncmContentMetaDatabaseCommit(&metadb);
        if(rc != 0)
        {
            this->irc.Error = rc;
            this->irc.Type = InstallerError::MetaDatabaseCommit;
            serviceClose(&metadb.s);
            this->Finalize();
            return this->irc;
        }
        serviceClose(&metadb.s);
        std::vector<ns::ContentStorageRecord> records;
        std::tuple<Result, u32> nst = ns::CountApplicationContentMeta(this->basetid);
        rc = std::get<0>(nst);
        u32 cmetacount = std::get<1>(nst);
        if((rc != 0) && (rc != 0x410))
        {
            this->irc.Error = rc;
            this->irc.Type = InstallerError::ContentMetaCount;
            this->Finalize();
            return this->irc;
        }
        if(cmetacount > 0)
        {
            records.resize(cmetacount);
            size_t csbufs = (cmetacount * sizeof(ns::ContentStorageRecord));
            ns::ContentStorageRecord *csbuf = (ns::ContentStorageRecord*)malloc(csbufs);
            std::tuple<Result, u32, void*> listt = ns::ListApplicationRecordContentMeta(0, this->basetid, csbufs);
            rc = std::get<0>(listt);
            csbuf = (ns::ContentStorageRecord*)std::get<2>(listt);
            if(rc != 0)
            {
                this->irc.Error = rc;
                this->irc.Type = InstallerError::ContentMetaList;
                this->Finalize();
                return this->irc;
            }
            memcpy(records.data(), csbuf, csbufs);
        }
        ns::ContentStorageRecord csrecord;
        csrecord.Record = metakey;
        csrecord.StorageId = stid;
        records.push_back(csrecord);
        rc = ns::DeleteApplicationRecord(this->basetid);
        rc = ns::PushApplicationRecord(this->basetid, 0x3, records.data(), records.size() * sizeof(ns::ContentStorageRecord));
        if(rc != 0)
        {
            this->irc.Error = rc;
            this->irc.Type = InstallerError::RecordPush;
            this->Finalize();
            return this->irc;
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
        return this->irc;
    }

    InstallerResult Installer::WriteContents(std::function<void(ncm::ContentRecord Record, u32 Content, u32 ContentCount, int Percentage)> Callback)
    {
        if(!this->ncas.empty()) for(u32 i = 0; i < this->ncas.size(); i++)
        {
            NcmNcaId curid = this->ncas[i].NCAId;
            std::string ncaname = horizon::GetStringFromNCAId(curid);
            bool nca = false;
            bool cnmt = false;
            FsFile ncafile;
            std::string fncaname = "/" + ncaname + ".nca";
            fncaname.reserve(FS_MAX_PATH);
            Result rc = fsFsOpenFile(&this->idfs, fncaname.c_str(), FS_OPEN_READ, &ncafile);
            if(rc == 0)
            {
                nca = true;
                ncaname += ".nca";
            }
            else
            {
                fncaname = "/" + ncaname + ".cnmt.nca";
                fncaname.reserve(FS_MAX_PATH);
                rc = fsFsOpenFile(&this->idfs, fncaname.c_str(), FS_OPEN_READ, &ncafile);
                if(rc == 0)
                {
                    ncaname += ".cnmt.nca";
                    cnmt = true;
                }
            }
            if(!nca) if(!cnmt)
            {
                this->irc.Error = rc;
                this->irc.Type = InstallerError::InstallBadNCA;
                continue;
            }
            ncm::ContentStorage cstorage(stid);
            cstorage.DeletePlaceHolder(curid);
            u64 ncasize = 0;
            rc = fsFileGetSize(&ncafile, &ncasize);
            if(rc != 0)
            {
                this->irc.Error = rc;
                this->irc.Type = InstallerError::InstallBadNCA;
                fsFileClose(&ncafile);
                continue;
            }
            u64 noff = 0;
            size_t reads = 0x100000;
            auto readbuf = std::make_unique<u8[]>(reads);
            cstorage.CreatePlaceHolder(curid, curid, ncasize);
            float progress = 0.0f;
            while(noff < ncasize)
            {
                progress = (float)noff / (float)ncasize;
                Callback(this->ncas[i], i, this->ncas.size(), (int)(progress * 100.0));
                if((noff + reads) >= ncasize) reads = (ncasize - noff);
                size_t rout = 0;
                fsFileRead(&ncafile, noff, readbuf.get(), reads, &rout);
                cstorage.WritePlaceHolder(curid, noff, readbuf.get(), reads);
                noff += reads;
            }
            cstorage.Register(curid, curid);
            cstorage.DeletePlaceHolder(curid);
            fsFileClose(&ncafile);
        }
        return this->irc;
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
        if(!this->ncas.empty()) for(u32 i = 0; i < this->ncas.size(); i++) if(this->ncas[i].Type == Type)
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

    InstallerResult Installer::GetLatestResult()
    {
        return this->irc;
    }

    void Installer::Finalize()
    {
        fsdevUnmountDevice("gnspi");
        if(this->nacps != NULL)
        {
            free(this->nacps);
            this->nacps = NULL;
        }
    }
}