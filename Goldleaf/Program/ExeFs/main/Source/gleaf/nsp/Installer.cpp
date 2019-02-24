#include <gleaf/nsp/Installer.hpp>
#include <gleaf/err.hpp>
#include <gleaf/fs.hpp>
#include <gleaf/ui.hpp>
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
        fsdevUnmountDevice("gnspi");
        fsdevMountDevice("gnspi", this->idfs);
        std::string tikname = fs::SearchForFile(this->idfs, "tik");
        if(tikname == "") this->tik = false;
        else
        {
            this->tik = true;
            this->stik = "gnspi:/" + tikname;
            this->tikdata = horizon::ReadTicket(this->stik);
        }
        ByteBuffer cnmt;
        std::string cnmtname = fs::SearchForFile(this->idfs, "cnmt.nca");
        if(cnmtname == "")
        {
            this->rc = err::Make(err::ErrorDescription::MetaNotFound);
            this->Finalize();
            return;
        }
        FsFile cnmtfile;
        std::string ocnmtn = "/" + cnmtname;
        ocnmtn.reserve(FS_MAX_PATH);
        this->rc = fsFsOpenFile(&this->idfs, ocnmtn.c_str(), FS_OPEN_READ, &cnmtfile);
        if(this->rc != 0)
        {
            this->Finalize();
            return;
        }
        u64 cnmtsize = 0;
        this->rc = fsFileGetSize(&cnmtfile, &cnmtsize);
        if(this->rc != 0)
        {
            fsFileClose(&cnmtfile);
            this->Finalize();
            return;
        }
        fsFileClose(&cnmtfile);
        fs::Explorer *nsys = fs::GetNANDSystemExplorer();
        fs::CopyFile("gnspi:/" + cnmtname, nsys->FullPathFor("contents/temp/" + cnmtname));
        std::string cnmtabs = "@SystemContent://temp/" + cnmtname;
        ncm::ContentRecord record = { 0 };
        record.NCAId = horizon::GetNCAIdFromString(cnmtname);
        *(u64*)record.Size = (cnmtsize & 0xffffffffffff);
        record.Type = ncm::ContentType::Meta;
        FsFileSystem cnmtfs;
        cnmtabs.reserve(FS_MAX_PATH);
        this->rc = fsOpenFileSystemWithId(&cnmtfs, 0, FsFileSystemType_ContentMeta, cnmtabs.c_str());
        if(this->rc != 0)
        {
            fsFsClose(&cnmtfs);
            this->Finalize();
            return;
        }
        std::string fcnmtname = fs::SearchForFile(cnmtfs, "cnmt");
        if(fcnmtname == "")
        {
            fsFsClose(&cnmtfs);
            this->rc = err::Make(err::ErrorDescription::CNMTNotFound);
            this->Finalize();
            return;
        }
        FsFile fcnmtfile;
        fcnmtname.reserve(FS_MAX_PATH);
        std::string fcname = "/" + fcnmtname;
        fcname.reserve(FS_MAX_PATH);
        this->rc = fsFsOpenFile(&cnmtfs, fcname.c_str(), FS_OPEN_READ, &fcnmtfile);
        if(this->rc != 0)
        {
            fsFsClose(&cnmtfs);
            this->Finalize();
            return;
        }
        u64 fcnmtsize = 0;
        this->rc = fsFileGetSize(&fcnmtfile, &fcnmtsize);
        if(this->rc != 0)
        {
            fsFileClose(&fcnmtfile);
            fsFsClose(&cnmtfs);
            this->Finalize();
            return;
        }
        ByteBuffer fcnmt;
        fcnmt.Resize(fcnmtsize);
        size_t rout = 0;
        this->rc = fsFileRead(&fcnmtfile, 0, fcnmt.GetData(), fcnmt.GetSize(), &rout);
        if(this->rc != 0)
        {
            fsFileClose(&fcnmtfile);
            fsFsClose(&cnmtfs);
            this->Finalize();
            return;
        }
        this->cmeta = ncm::ContentMeta(fcnmt.GetData(), fcnmt.GetSize());
        ncm::ContentStorage cnmts(stid);
        if(!cnmts.Has(record.NCAId)) this->ncas.push_back(record);
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
                fs::CopyFile("gnspi:/" + cnca, nsys->FullPathFor("contents/temp/" + cnca));
                std::string ncaname = "@SystemContent://temp/" + cnca;
                ncaname.reserve(FS_MAX_PATH);
                FsFileSystem gnspcnca;
                Result crc = fsOpenFileSystemWithId(&gnspcnca, this->basetid, FsFileSystemType_ContentControl, ncaname.c_str());
                if(crc != 0) continue;
                fsdevMountDevice("gnspcnca", gnspcnca);
                DIR *d = opendir("gnspcnca:/");
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
                        icon = "gnspcnca:/" + fle;
                        break;
                    }
                }
                closedir(d);
                std::ifstream dat(icon);
                this->icn = "sdmc:/goldleaf/meta/" + horizon::GetStringFromNCAId(record.NCAId) + ".jpg";
                fs::DeleteFile(this->icn);
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
        fsFileClose(&fcnmtfile);
        fsFsClose(&cnmtfs);
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
        ns::ContentStorageRecord csrecord;
        csrecord.Record = metakey;
        csrecord.StorageId = stid;
        records.push_back(csrecord);
        ns::DeleteApplicationRecord(this->basetid);
        this->rc = ns::PushApplicationRecord(this->basetid, 0x3, records.data(), records.size() * sizeof(ns::ContentStorageRecord));
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

    Result Installer::GetLatestResult()
    {
        return this->rc;
    }

    void Installer::Finalize()
    {
        fsdevUnmountDevice("gnspcnca");
        fsdevUnmountDevice("gnspi");
        fs::Explorer *nsys = fs::GetNANDSystemExplorer();
        fs::DeleteDirectory(nsys->FullPathFor("contents/temp"));
        fs::CreateDirectory(nsys->FullPathFor("contents/temp"));
        if(this->nacps != NULL)
        {
            free(this->nacps);
            this->nacps = NULL;
        }
    }
}