#include <gleaf/usb/USBInstaller.hpp>
#include <gleaf/usb/Communications.hpp>
#include <malloc.h>
#include <gleaf/fs.hpp>

namespace gleaf::usb
{
    Installer::Installer(UsbReader reader, std::string file, Destination location, bool ignoreVersion)
    {
        this->contentData = reader.ReadData(file);
        switch(location)
        {
            case Destination::NAND:
                this->storageId = FsStorageId_NandUser;
                break;
            case Destination::SdCard:
                this->storageId = FsStorageId_SdCard;
                break;
        }

        this->usbReader = reader;
        this->filename = file;
        this->installerResult = { 0, InstallerError::Success };

        //TODO
        this->iver = ignoreVersion;
        this->gtik = false;
        this->itik = false;
        this->stik = 0;
    }

    InstallerResult Installer::ProcessRecords()
    {
        NSPContentData cnmtData = this->ProcessCnmt();
        NcmNcaId cnmtid = horizon::GetNCAIdFromString(cnmtData.Name);

        ByteBuffer fcnmt;
        this->ReadFcmnt(cnmtid, &fcnmt);
        if (this->installerResult.Error != 0) {
            return this->installerResult;
        }

        NcmMetaRecord metakey;
        this->WriteMetaDatabase(cnmtid, cnmtData, fcnmt, &metakey);
        if (this->installerResult.Error != 0) {
            return this->installerResult;
        }

        u64 basetid = horizon::GetBaseApplicationId(metakey.titleId, static_cast<ncm::ContentMetaType>(metakey.type));

        std::vector<ns::ContentStorageRecord> records;
        this->RegisterContentMeta(basetid, &records);
        if (this->installerResult.Error != 0) {
            return this->installerResult;
        }

        return this->PushRecords(metakey, basetid, records);
    }

    InstallerResult Installer::ProcessContent(u32 Index, std::function<void(std::string Name, u32 Index, u32 Count, int Percentage, double Speed)> Callback)
    {
        NSPContentData cnt = this->contentData[Index];
        std::string name = cnt.Name;
        std::string ext = fs::GetExtension(name);

        if (ext == "nca")
        {
            this->ProcessNca(cnt, name, ext);
        }
        else if (ext == "tik")
        {
            this->btik = this->usbReader.ReadTicket(this->filename, &this->stik);
            this->gtik = true;
        }

        if(this->gtik && !this->itik)
        {
            this->itik = true;
            es::ImportTicket(this->btik.get(), this->stik, es::CertData, 0x700);
        }

        return this->installerResult;
    }

    InstallerResult Installer::ProcessContents(std::function<void(std::string Name, u32 Index, u32 Count, int Percentage, double Speed)> Callback)
    {
        for(u32 i = 0; i < this->contentData.size(); i++)
        {
            NSPContentData cnt = this->contentData[i];
            if(cnt.Name.substr(cnt.Name.length() - 8) == "cnmt.nca")
                continue;
            else this->ProcessContent(i, Callback);
        }
        return this->installerResult;
    }

    InstallerResult Installer::GetLatestResult()
    {
        return this->installerResult;
    }

    void Installer::Finish()
    {
        
    }

    
    NSPContentData Installer::ProcessCnmt()
    {
        NSPContentData cnmtdata;
        for(u32 i = 0; i < this->contentData.size(); i++)
        {
            NSPContentData cnt = this->contentData[i];
            if(cnt.Name.substr(cnt.Name.length() - 8) == "cnmt.nca")
            { 
                cnmtdata = cnt;
                this->ProcessContent(i, [&](std::string Name, u32 Index, u32 Count, int Percentage, double Speed){});
                break;
            }
        }
        return cnmtdata;
    }
    
    void Installer::ProcessNca(NSPContentData content, std::string name, std::string ext)
    {
        NcmNcaId ncaid = horizon::GetNCAIdFromString(name);
        ncm::ContentStorage contentStorage(this->storageId);
        if(!contentStorage.Has(ncaid))
        {
            contentStorage.DeletePlaceHolder(ncaid);
            contentStorage.CreatePlaceHolder(ncaid, ncaid, content.Size);
            BufferedPlaceHolderWriter bufferPHWriter(&contentStorage, ncaid, content.Size);

            size_t length;
            std::unique_ptr<u8[]> data = this->usbReader.ReadContent(this->filename, content.Index, &length);
            bufferPHWriter.AppendData(static_cast<void*>(data.get()), length);
            bufferPHWriter.WriteSegmentToPlaceHolder();

            contentStorage.Register(ncaid, ncaid);
            contentStorage.DeletePlaceHolder(ncaid);
        }
    }
    
    InstallerResult Installer::ReadFcmnt(NcmNcaId cnmtid, ByteBuffer* output)
    {
        ncm::ContentStorage contentStorage(this->storageId);
        std::string cnmtPath = contentStorage.GetPath(cnmtid);
        FsFileSystem cnmtfs;
        cnmtPath.reserve(FS_MAX_PATH);
        Result rc = fsOpenFileSystemWithId(&cnmtfs, 0, FsFileSystemType_ContentMeta, cnmtPath.c_str());
        if(rc != 0)
        {
            this->installerResult.Error = rc;
            this->installerResult.Type = InstallerError::CNMTMCAOpen;
            return this->installerResult;
        }

        std::string fcnmtName = fs::SearchForFile(cnmtfs, "", "cnmt");
        if(fcnmtName == "")
        {
            this->installerResult.Error = rc;
            this->installerResult.Type = InstallerError::BadCNMT;
            return this->installerResult;
        }

        FsFile fcnmtFile;
        fcnmtName.reserve(FS_MAX_PATH);
        rc = fsFsOpenFile(&cnmtfs, ("/" + fcnmtName).c_str(), FS_OPEN_READ, &fcnmtFile);
        if(rc != 0)
        {
            this->installerResult.Error = rc;
            this->installerResult.Type = InstallerError::CNMTOpen;
            return this->installerResult;
        }

        u64 fcnmtSize = 0;
        fsFileGetSize(&fcnmtFile, &fcnmtSize);

        ByteBuffer fcnmt;
        fcnmt.Resize(fcnmtSize);

        size_t rout = 0;
        fsFileRead(&fcnmtFile, 0, fcnmt.GetData(), fcnmt.GetSize(), &rout);

        *output = fcnmt;
        return this->installerResult;
    }

    InstallerResult Installer::WriteMetaDatabase(NcmNcaId cnmtid, NSPContentData cnmtData, ByteBuffer fcnmt, NcmMetaRecord* result)
    {
        ncm::ContentMeta cmeta(fcnmt.GetData(), fcnmt.GetSize());
        ncm::ContentRecord cnmtr;
        cnmtr.NCAId = cnmtid;
        *(u64*)&cnmtr.Size = cnmtData.Size & 0xffffffffffff;
        cnmtr.Type = ncm::ContentType::Meta;
        cmeta.GetInstallContentMeta(cnmtbuf, cnmtr, this->iver);
        NcmContentMetaDatabase metadb;
        NcmMetaRecord metakey = cmeta.GetContentMetaKey();
        Result rc = ncmOpenContentMetaDatabase(this->storageId, &metadb);
        if(rc != 0)
        {
            this->installerResult.Error = rc;
            this->installerResult.Type = InstallerError::MetaDatabaseOpen;
            serviceClose(&metadb.s);
            return this->installerResult;
        }
        rc = ncmContentMetaDatabaseSet(&metadb, &metakey, cnmtbuf.GetSize(), (NcmContentMetaRecordsHeader*)cnmtbuf.GetData());
        if(rc != 0)
        {
            this->installerResult.Error = rc;
            this->installerResult.Type = InstallerError::MetaDatabaseSet;
            serviceClose(&metadb.s);
            return this->installerResult;
        }
        rc = ncmContentMetaDatabaseCommit(&metadb);
        if(rc != 0)
        {
            this->installerResult.Error = rc;
            this->installerResult.Type = InstallerError::MetaDatabaseCommit;
            serviceClose(&metadb.s);
            return this->installerResult;
        }
        serviceClose(&metadb.s);

        *result = metakey;
        return this->installerResult;
    }

    InstallerResult Installer::RegisterContentMeta(u64 basetid, std::vector<ns::ContentStorageRecord>* result)
    {
        std::vector<ns::ContentStorageRecord> records;
        std::tuple<Result, u32> nst = ns::CountApplicationContentMeta(basetid);
        Result rc = std::get<0>(nst);
        u32 cmetacount = std::get<1>(nst);
        if((rc != 0) && (rc != 0x410))
        {
            this->installerResult.Error = rc;
            this->installerResult.Type = InstallerError::ContentMetaCount;
            return this->installerResult;
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
                this->installerResult.Error = rc;
                this->installerResult.Type = InstallerError::ContentMetaList;
                return this->installerResult;
            }
            memcpy(records.data(), csbuf, csbufs);
            //free(csbuf);
        }

        *result = records;
        return this->installerResult;
    }

    InstallerResult Installer::PushRecords(NcmMetaRecord metakey, u64 basetid, std::vector<ns::ContentStorageRecord> records)
    {
        ns::ContentStorageRecord csrecord;
        csrecord.Record = metakey;
        csrecord.StorageId = this->storageId;
        records.push_back(csrecord);
        Result rc = ns::DeleteApplicationRecord(basetid);
        rc = ns::PushApplicationRecord(basetid, 0x3, records.data(), records.size() * sizeof(ns::ContentStorageRecord));
        if(rc != 0)
        {
            this->installerResult.Error = rc;
            this->installerResult.Type = InstallerError::RecordPush;
        }
        return this->installerResult;
    }
}