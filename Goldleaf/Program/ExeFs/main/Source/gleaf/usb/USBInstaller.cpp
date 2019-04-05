#include <gleaf/usb/USBInstaller.hpp>
#include <gleaf/nsp/Installer.hpp>
#include <gleaf/usb/Communications.hpp>
#include <gleaf/fs.hpp>
#include <gleaf/horizon.hpp>
#include <malloc.h>
#include <threads.h>

#include <gleaf/ui.hpp>

namespace gleaf::ui
{
    extern MainApplication *mainapp;
}

namespace gleaf::usb
{
    Installer::Installer(Storage Location, bool IgnoreVersion, UsbCallbackFn Callback)
    {
        this->stid = static_cast<FsStorageId>(Location);
        this->rc = 0;
        this->iver = IgnoreVersion;
        this->gtik = false;
        this->itik = false;
        this->stik = 0;
        this->appid = 0;
        Command req = ReadCommand(Callback);
        if(CommandMagicOk(req) && IsCommandId(req, CommandId::NSPData))
        {
            u32 filecount = Read32(Callback);
            for(u32 i = 0; i < filecount; i++)
            {
                std::string name = ReadString(Callback);
                u64 offset = Read64(Callback);
                u64 size = Read64(Callback);
                this->cnts.push_back({ i, name, offset, size });
            }
        }
        else this->rc = err::Make(err::ErrorDescription::BadGLUCCommand);
    }

    Result Installer::ProcessRecords(std::function<void(std::string Name, u32 Index, u32 Count, int Percentage, double Speed)> Callback)
    {
        NSPContentData cnmtdata;
        for(u32 i = 0; i < this->cnts.size(); i++)
        {
            NSPContentData cnt = this->cnts[i];
            if(cnt.Name.substr(cnt.Name.length() - 8) == "cnmt.nca")
            { 
                cnmtdata = cnt;
                this->ProcessContent(i, Callback);
                break;
            }
        }
        NcmNcaId cnmtid = horizon::GetNCAIdFromString(cnmtdata.Name);
        ncm::ContentStorage cst(this->stid);
        std::string cnmtabs = cst.GetPath(cnmtid);
        FsFileSystem cnmtfs;
        fsdevMountDevice("gusbcnmtfs", cnmtfs);
        cnmtabs.reserve(FS_MAX_PATH);
        this->rc = fsOpenFileSystemWithId(&cnmtfs, 0, FsFileSystemType_ContentMeta, cnmtabs.c_str());
        if(this->rc != 0)
        {
            this->Finalize();
            return this->rc;
        }
        std::string fcnmtname = fs::SearchForFileInPath("gusbcnmtfs:/", "cnmt");
        if(fcnmtname == "")
        {
            this->rc = err::Make(err::ErrorDescription::CNMTNotFound);
            this->Finalize();
            return this->rc;
        }
        FsFile fcnmtfile;
        fcnmtname.reserve(FS_MAX_PATH);
        do
        {
            this->rc = fsFsOpenFile(&cnmtfs, ("/" + fcnmtname).c_str(), FS_OPEN_READ, &fcnmtfile);
        }
        while(this->rc == 0xd401);
        if(this->rc != 0)
        {
            this->Finalize();
            return this->rc;
        }
        u64 fcnmtsize = 0;
        this->rc = fsFileGetSize(&fcnmtfile, &fcnmtsize);
        if(this->rc != 0)
        {
            this->Finalize();
            return this->rc;
        }
        ByteBuffer fcnmt;
        fcnmt.Resize(fcnmtsize);
        size_t rout = 0;
        this->rc = fsFileRead(&fcnmtfile, 0, fcnmt.GetData(), fcnmt.GetSize(), &rout);
        if(this->rc != 0)
        {
            this->Finalize();
            return this->rc;
        }
        ncm::ContentMeta cmeta(fcnmt.GetData(), fcnmt.GetSize());
        ncm::ContentRecord cnmtr;
        cnmtr.NCAId = cnmtid;
        *(u64*)&cnmtr.Size = cnmtdata.Size & 0xffffffffffff;
        cnmtr.Type = ncm::ContentType::Meta;
        cmeta.GetInstallContentMeta(cnmtbuf, cnmtr, this->iver);
        NcmContentMetaDatabase metadb;
        NcmMetaRecord metakey = cmeta.GetContentMetaKey();
        this->appid = metakey.titleId;
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
        u64 basetid = horizon::GetBaseApplicationId(metakey.titleId, static_cast<ncm::ContentMetaType>(metakey.type));
        std::tuple<Result, u32> nst = ns::CountApplicationContentMeta(basetid);
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
            std::tuple<Result, u32, void*> listt = ns::ListApplicationRecordContentMeta(0, basetid, csbufs);
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
        ns::DeleteApplicationRecord(basetid);
        this->rc = ns::PushApplicationRecord(basetid, 0x3, records.data(), records.size() * sizeof(ns::ContentStorageRecord));
        if(this->rc != 0)
        {
            this->Finalize();
            return this->rc;
        }
        return this->rc;
    }

    Result Installer::ProcessContent(u32 Index, std::function<void(std::string Name, u32 Index, u32 Count, int Percentage, double Speed)> Callback)
    {
        NSPContentData cnt = this->cnts[Index];
        std::string name = cnt.Name;
        std::string ext = fs::GetExtension(name);
        if(ext == "nca")
        {
            NcmNcaId ncaid = horizon::GetNCAIdFromString(name);
            ncm::ContentStorage cst(this->stid);
            if(!cst.Has(ncaid))
            {
                cst.DeletePlaceHolder(ncaid);
                cst.CreatePlaceHolder(ncaid, ncaid, cnt.Size);
                WriteCommand(CommandId::NSPContent);
                Write32(cnt.Index);
                size_t reads = 0x200000;
                u64 noff = 0;
                u8 *readbuf = (u8*)memalign(0x1000, reads);
                float progress = 0.0f;
                u64 freq = armGetSystemTickFreq();
                u64 tstart = armGetSystemTick();
                size_t bssize = 0;
                double speed = 0.0;
                while(noff < cnt.Size)
                {
                    u64 tnew = armGetSystemTick();
                    if((tnew - tstart) >= freq)
                    {
                        double mbbuf = ((noff / 1000000.0) - (bssize / 1000000.0));
                        double dtime = ((double)(tnew - tstart) / (double)freq);
                        speed = (mbbuf / dtime);
                        tstart = tnew;
                        bssize = noff;
                    }
                    progress = (float)noff / (float)cnt.Size;
                    if((noff + reads) >= cnt.Size) reads = (cnt.Size - noff);
                    size_t rout = usb::Read(readbuf, reads);
                    if(rout == 0) break;
                    Callback(name, Index, cnts.size(), (int)(progress * 100.0), speed);
                    cst.WritePlaceHolder(ncaid, noff, readbuf, rout);
                    noff += rout;
                }
                free(readbuf);
                cst.Register(ncaid, ncaid);
                cst.DeletePlaceHolder(ncaid);
            }
        }
        else if(ext == "tik")
        {
            WriteCommand(CommandId::NSPTicket);
            this->btik = std::make_unique<u8[]>(cnt.Size);
            if(Read((void*)this->btik.get(), cnt.Size) != 0)
            {
                this->stik = cnt.Size;
                this->gtik = true;
            }
        }
        if(this->gtik && !this->itik)
        {
            es::ImportTicket(this->btik.get(), this->stik, es::CertData, 1792);
            this->itik = true;
        }
        return this->rc;
    }

    Result Installer::ProcessContents(std::function<void(std::string Name, u32 Index, u32 Count, int Percentage, double Speed)> Callback)
    {
        for(u32 i = 0; i < this->cnts.size(); i++)
        {
            NSPContentData cnt = this->cnts[i];
            if(cnt.Name.substr(cnt.Name.length() - 8) == "cnmt.nca") continue;
            else this->ProcessContent(i, Callback);
        }
        return this->rc;
    }

    u64 Installer::GetApplicationId()
    {
        return this->appid;
    }

    Result Installer::GetLatestResult()
    {
        return this->rc;
    }

    void Installer::Finalize()
    {
        WriteCommand(CommandId::Finish);
    }
}