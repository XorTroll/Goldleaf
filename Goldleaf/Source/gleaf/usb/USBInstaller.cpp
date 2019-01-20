#include <gleaf/usb/USBInstaller.hpp>
#include <gleaf/nsp/Installer.hpp>
#include <gleaf/usb/Communications.hpp>
#include <malloc.h>
#include <gleaf/fs.hpp>
#include <threads.h>

namespace gleaf::usb
{
    Installer::Installer(Destination Location, bool IgnoreVersion)
    {
        Command req = ReadCommand();
        if(req.MagicOk() && req.IsCommandId(CommandId::NSPData))
        {
            u32 filecount = Read32();
            for(u32 i = 0; i < filecount; i++)
            {
                u32 namelen = Read32();
                std::string name = ReadString(namelen);
                u64 offset = Read64();
                u64 size = Read64();
                this->cnts.push_back({ i, name, offset, size });
            }
        }
        switch(Location)
        {
            case Destination::NAND:
                this->stid = FsStorageId_NandUser;
                break;
            case Destination::SdCard:
                this->stid = FsStorageId_SdCard;
                break;
        }
        this->irc = { 0, InstallerError::Success };
        this->iver = IgnoreVersion;
        this->gtik = false;
        this->itik = false;
        this->stik = 0;
    }

    InstallerResult Installer::ProcessRecords()
    {
        NSPContentData cnmtdata;
        for(u32 i = 0; i < this->cnts.size(); i++)
        {
            NSPContentData cnt = this->cnts[i];
            if(cnt.Name.substr(cnt.Name.length() - 8) == "cnmt.nca")
            { 
                cnmtdata = cnt;
                this->ProcessContent(i, [&](std::string Name, u32 Index, u32 Count, int Percentage, double Speed){});
                break;
            }
        }
        NcmNcaId cnmtid = horizon::GetNCAIdFromString(cnmtdata.Name);
        ncm::ContentStorage cst(this->stid);
        std::string cnmtabs = cst.GetPath(cnmtid);
        FsFileSystem cnmtfs;
        cnmtabs.reserve(FS_MAX_PATH);
        Result rc = fsOpenFileSystemWithId(&cnmtfs, 0, FsFileSystemType_ContentMeta, cnmtabs.c_str());
        if(rc != 0)
        {
            this->irc.Error = rc;
            this->irc.Type = InstallerError::CNMTMCAOpen;
            return this->irc;
        }
        std::string fcnmtname = fs::SearchForFile(cnmtfs, "", "cnmt");
        if(fcnmtname == "")
        {
            this->irc.Error = rc;
            this->irc.Type = InstallerError::BadCNMT;
            return this->irc;
        }
        FsFile fcnmtfile;
        fcnmtname.reserve(FS_MAX_PATH);
        rc = fsFsOpenFile(&cnmtfs, ("/" + fcnmtname).c_str(), FS_OPEN_READ, &fcnmtfile);
        if(rc != 0)
        {
            this->irc.Error = rc;
            this->irc.Type = InstallerError::CNMTOpen;
            return this->irc;
        }
        u64 fcnmtsize = 0;
        fsFileGetSize(&fcnmtfile, &fcnmtsize);
        ByteBuffer fcnmt;
        fcnmt.Resize(fcnmtsize);
        size_t rout = 0;
        fsFileRead(&fcnmtfile, 0, fcnmt.GetData(), fcnmt.GetSize(), &rout);
        ncm::ContentMeta cmeta(fcnmt.GetData(), fcnmt.GetSize());
        ncm::ContentRecord cnmtr;
        cnmtr.NCAId = cnmtid;
        *(u64*)&cnmtr.Size = cnmtdata.Size & 0xffffffffffff;
        cnmtr.Type = ncm::ContentType::Meta;
        cmeta.GetInstallContentMeta(cnmtbuf, cnmtr, iver);
        NcmContentMetaDatabase metadb;
        NcmMetaRecord metakey = cmeta.GetContentMetaKey();
        rc = ncmOpenContentMetaDatabase(stid, &metadb);
        if(rc != 0)
        {
            this->irc.Error = rc;
            this->irc.Type = InstallerError::MetaDatabaseOpen;
            serviceClose(&metadb.s);
            return this->irc;
        }
        rc = ncmContentMetaDatabaseSet(&metadb, &metakey, cnmtbuf.GetSize(), (NcmContentMetaRecordsHeader*)cnmtbuf.GetData());
        if(rc != 0)
        {
            this->irc.Error = rc;
            this->irc.Type = InstallerError::MetaDatabaseSet;
            serviceClose(&metadb.s);
            return this->irc;
        }
        rc = ncmContentMetaDatabaseCommit(&metadb);
        if(rc != 0)
        {
            this->irc.Error = rc;
            this->irc.Type = InstallerError::MetaDatabaseCommit;
            serviceClose(&metadb.s);
            return this->irc;
        }
        serviceClose(&metadb.s);
        std::vector<ns::ContentStorageRecord> records;
        u64 basetid = horizon::GetBaseApplicationId(metakey.titleId, static_cast<ncm::ContentMetaType>(metakey.type));
        std::tuple<Result, u32> nst = ns::CountApplicationContentMeta(basetid);
        rc = std::get<0>(nst);
        u32 cmetacount = std::get<1>(nst);
        if((rc != 0) && (rc != 0x410))
        {
            this->irc.Error = rc;
            this->irc.Type = InstallerError::ContentMetaCount;
            return this->irc;
        }
        if(cmetacount > 0)
        {
            records.resize(cmetacount);
            size_t csbufs = (cmetacount * sizeof(ns::ContentStorageRecord));
            ns::ContentStorageRecord *csbuf = (ns::ContentStorageRecord*)malloc(csbufs);
            std::tuple<Result, u32, void*> listt = ns::ListApplicationRecordContentMeta(0, basetid, csbufs);
            rc = std::get<0>(listt);
            csbuf = (ns::ContentStorageRecord*)std::get<2>(listt);
            if(rc != 0)
            {
                this->irc.Error = rc;
                this->irc.Type = InstallerError::ContentMetaList;
                return this->irc;
            }
            memcpy(records.data(), csbuf, csbufs);
        }
        ns::ContentStorageRecord csrecord;
        csrecord.Record = metakey;
        csrecord.StorageId = stid;
        records.push_back(csrecord);
        rc = ns::DeleteApplicationRecord(basetid);
        rc = ns::PushApplicationRecord(basetid, 0x3, records.data(), records.size() * sizeof(ns::ContentStorageRecord));
        if(rc != 0)
        {
            this->irc.Error = rc;
            this->irc.Type = InstallerError::RecordPush;
        }
        return this->irc;
    }

    InstallerResult Installer::ProcessContent(u32 Index, std::function<void(std::string Name, u32 Index, u32 Count, int Percentage, double Speed)> Callback)
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
                BufferedPlaceHolderWriter bphw(&cst, ncaid, cnt.Size);
                ContentThreadArguments targs;
                targs.Index = cnt.Index;
                targs.Size = cnt.Size;
                targs.WriterRef = &bphw;
                thrd_t tcntread;
                thrd_t tcntappend;
                thrd_create(&tcntread, OnContentRead, &targs);
                thrd_create(&tcntappend, OnContentAppend, &targs);
                u64 freq = armGetSystemTickFreq();
                u64 tstart = armGetSystemTick();
                size_t bssize = 0;
                double speed = 0.0;
                while(!bphw.IsBufferDataComplete())
                {
                    u64 tnew = armGetSystemTick();
                    if((tnew - tstart) >= freq)
                    {
                        size_t bnsize = bphw.GetSizeBuffered();
                        double mbbuf = ((bnsize / 1000000.0) - (bssize / 1000000.0));
                        double dtime = ((double)(tnew - tstart) / (double)freq);
                        speed = (mbbuf / dtime);
                        tstart = tnew;
                        bssize = bnsize;
                    }
                    u64 mbtotal = (bphw.GetTotalDataSize() / 1000000);
                    u64 mbdlsz = (bphw.GetSizeBuffered() / 1000000);
                    int perc = (int)(((double)bphw.GetSizeBuffered() / (double)bphw.GetTotalDataSize()) * 100.0);
                    Callback(name, Index, cnts.size(), perc, speed);
                }
                u64 mbtotal = (bphw.GetTotalDataSize() / 1000000);
                while(!bphw.IsPlaceHolderComplete())
                {
                    u64 mbinsz = (bphw.GetSizeWrittenToPlaceHolder() / 1000000);
                    int perc = (int)(((double)bphw.GetSizeWrittenToPlaceHolder() / (double)bphw.GetTotalDataSize()) * 100.0);
                    // Callback(name, Index, cnts.size(), perc, speed);
                }
                thrd_join(tcntread, NULL);
                thrd_join(tcntappend, NULL);
                cst.Register(ncaid, ncaid);
                cst.DeletePlaceHolder(ncaid);
            }
        }
        else if(ext == "tik")
        {
            Command tikcmd = MakeCommand(CommandId::NSPTicket);
            WriteCommand(tikcmd);
            this->btik = std::make_unique<u8[]>(cnt.Size);
            Read((void*)this->btik.get(), cnt.Size);
            this->stik = cnt.Size;
            this->gtik = true;
        }
        if(this->gtik && !this->itik)
        {
            this->itik = true;
            es::ImportTicket(this->btik.get(), this->stik, es::CertData, 1792);
        }
        return this->irc;
    }

    InstallerResult Installer::ProcessContents(std::function<void(std::string Name, u32 Index, u32 Count, int Percentage, double Speed)> Callback)
    {
        for(u32 i = 0; i < this->cnts.size(); i++)
        {
            NSPContentData cnt = this->cnts[i];
            if(cnt.Name.substr(cnt.Name.length() - 8) == "cnmt.nca") continue;
            else this->ProcessContent(i, Callback);
        }
        return this->irc;
    }

    InstallerResult Installer::GetLatestResult()
    {
        return this->irc;
    }

    void Installer::Finish()
    {
        Command fcmd = MakeCommand(CommandId::Finish);
        WriteCommand(fcmd);
    }
}