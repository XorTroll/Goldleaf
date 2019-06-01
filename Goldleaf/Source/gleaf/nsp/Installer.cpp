#include <gleaf/nsp/Installer.hpp>
#include <gleaf/err.hpp>
#include <gleaf/fs.hpp>
#include <sys/stat.h>
#include <fstream>
#include <malloc.h>
#include <dirent.h>
#include <chrono>

extern gleaf::set::Settings gsets;

namespace gleaf::nsp
{
    Installer::Installer(std::string Path, fs::Explorer *Exp, Storage Location) : nspentry(Exp, Path), storage(static_cast<FsStorageId>(Location))
    {
    }

    Installer::~Installer()
    {
        FinalizeInstallation();
    }

    Result Installer::PrepareInstallation()
    {
        Result rc = 0xdead;
        if(nspentry.IsOk())
        {
            rc = 0;
            std::string cnmtnca;
            u32 idxcnmtnca = 0;
            u64 scnmtnca = 0;
            std::string tik;
            u32 idxtik = 0;
            stik = 0;
            auto files = nspentry.GetFiles();
            for(u32 i = 0; i < files.size(); i++)
            {
                std::string file = files[i];
                if(fs::GetExtension(file) == "tik")
                {
                    tik = file;
                    idxtik = i;
                    stik = nspentry.GetFileSize(i);
                }
                else if(file.substr(file.length() - 8) == "cnmt.nca")
                {
                    cnmtnca = file;
                    idxcnmtnca = i;
                    scnmtnca = nspentry.GetFileSize(i);
                }
            }
            std::string icnmtnca = fs::GetFileName(cnmtnca);
            fs::Explorer *nsys = fs::GetNANDSystemExplorer();
            nsys->CreateDirectory("Contents/temp");
            std::string ncnmtnca = nsys->FullPathFor("Contents/temp/" + cnmtnca);
            nsys->DeleteFile(ncnmtnca);
            nspentry.SaveFile(idxcnmtnca, nsys, ncnmtnca);
            std::string acnmtnca = "@SystemContent://temp/" + cnmtnca;
            acnmtnca.reserve(FS_MAX_PATH);
            ByteBuffer bcnmt;
            FsFileSystem cnmtncafs;
            rc = fsOpenFileSystem(&cnmtncafs, FsFileSystemType_ContentMeta, acnmtnca.c_str());
            if(rc != 0) return rc;
            {
                fs::FileSystemExplorer cnmtfs("gnspcnmtnca", "NSP-ContentMeta", &cnmtncafs, true);
                auto cnts = cnmtfs.GetContents();
                std::string fcnmt;
                for(u32 i = 0; i < cnts.size(); i++)
                {
                    std::string cnt = cnts[i];
                    if(fs::GetExtension(cnt) == "cnmt")
                    {
                        fcnmt = cnt;
                        break;
                    }
                }
                u64 fcnmtsz = cnmtfs.GetFileSize(fcnmt);
                bcnmt.Resize(fcnmtsz);
                cnmtfs.ReadFileBlock(fcnmt, 0, fcnmtsz, bcnmt.GetData());
            }
            record = { 0 };
            record.NCAId = horizon::GetNCAIdFromString(icnmtnca);
            *(u64*)record.Size = (scnmtnca & 0xffffffffffff);
            record.Type = ncm::ContentType::Meta;
            u64 baseappid;

            memset(&mrec, 0, sizeof(NcmMetaRecord));
            cnmt = ncm::ContentMeta(bcnmt.GetData(), bcnmt.GetSize());
            mrec = cnmt.GetContentMetaKey();
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
            if(!hascnmt) ncas.push_back(record);
            baseappid = horizon::GetBaseApplicationId(mrec.titleId, static_cast<ncm::ContentMetaType>(mrec.type));
            auto recs = cnmt.GetContentRecords();
            entrynacp = (NacpStruct*)malloc(sizeof(NacpStruct));
            std::string nstik = "Contents/temp/" + tik;
            std::string ptik = nsys->FullPathFor(nstik);
            if(stik > 0)
            {
                nspentry.SaveFile(idxtik, nsys, ptik);
                entrytik = horizon::ReadTicket(ptik);
            }
            std::string ncontrolnca;
            for(u32 i = 0; i < recs.size(); i++)
            {
                ncas.push_back(recs[i]);
                if(recs[i].Type == ncm::ContentType::Control)
                {
                    std::string controlncaid = horizon::GetStringFromNCAId(recs[i].NCAId);
                    std::string controlnca = controlncaid + ".nca";
                    u32 idxcontrolnca = nspentry.GetFileIndexByName(controlnca);
                    ncontrolnca = nsys->FullPathFor("Contents/temp/" + controlnca);
                    nspentry.SaveFile(idxcontrolnca, nsys, ncontrolnca);
                    std::string acontrolnca = "@SystemContent://temp/" + controlnca;
                    acontrolnca.reserve(FS_MAX_PATH);
                    FsFileSystem controlncafs;
                    rc = fsOpenFileSystemWithId(&controlncafs, baseappid, FsFileSystemType_ContentControl, acontrolnca.c_str());
                    if(rc == 0)
                    {
                        fs::FileSystemExplorer controlfs("gnspcontrolnca", "NSP-Control", &controlncafs, true);
                        auto cnts = controlfs.GetContents();
                        for(u32 i = 0; i < cnts.size(); i++)
                        {
                            std::string cnt = cnts[i];
                            if(fs::GetExtension(cnt) == "dat")
                            {
                                icon = "sdmc:/goldleaf/meta/" + controlncaid + ".jpg";
                                controlfs.CopyFile(cnt, icon);
                                break;
                            }
                        }
                        controlfs.ReadFileBlock("control.nacp", 0, sizeof(NacpStruct), (u8*)entrynacp);
                    }
                    rc = 0;
                }
            }
        }
        return rc;
    }

    Result Installer::PreProcessContents()
    {
        fs::Explorer *nsys = fs::GetNANDSystemExplorer();
        NcmContentMetaDatabase mdb;
        Result rc = ncmOpenContentMetaDatabase(storage, &mdb);
        if(rc != 0) return rc;
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
            auto tdata = nsys->ReadFile("Contents/temp/" + tik);
            es::ImportTicket(tdata.data(), tdata.size(), es::CertData, 1792);
        }
        return rc;
    }

    ncm::ContentMetaType Installer::GetContentMetaType()
    {
        return static_cast<ncm::ContentMetaType>(mrec.type);
    }

    u64 Installer::GetApplicationId()
    {
        return mrec.titleId;
    }

    std::string Installer::GetExportedIconPath()
    {
        return icon;
    }

    NacpStruct *Installer::GetNACP()
    {
        return entrynacp;
    }

    bool Installer::HasTicket()
    {
        return (stik > 0);
    }

    horizon::TicketData Installer::GetTicketData()
    {
        return entrytik;
    }

    std::vector<ncm::ContentRecord> Installer::GetNCAs()
    {
        return ncas;
    }

    Result Installer::WriteContents(std::function<void(ncm::ContentRecord Record, u32 Content, u32 ContentCount, double Done, double Total, u64 BytesSec)> OnContentWrite)
    {
        fs::Explorer *nsys = fs::GetNANDSystemExplorer();
        Result rc = 0;
        u64 reads = fs::GetFileSystemOperationsBufferSize();
        u8 *rdata = fs::GetFileSystemOperationsBuffer();
        for(u32 i = 0; i < ncas.size(); i++)
        {
            ncm::ContentRecord rnca = ncas[i];
            NcmNcaId curid = rnca.NCAId;
            std::string ncaname = horizon::GetStringFromNCAId(curid);
            if(rnca.Type == ncm::ContentType::Meta) ncaname += ".cnmt";
            ncaname += ".nca";
            u32 idxncaname = nspentry.GetFileIndexByName(ncaname);
            u64 ncasize = nspentry.GetFileSize(idxncaname);
            NcmContentStorage cst;
            ncmOpenContentStorage(storage, &cst);
            ncm::DeletePlaceHolder(&cst, &curid);
            ncm::CreatePlaceHolder(&cst, &curid, &curid, ncasize);
            u64 noff = 0;
            u64 szrem = ncasize;
            u64 tmpwritten = 0;
            u64 bsec = 0;
            auto t1 = std::chrono::steady_clock::now();
            while(szrem)
            {
                auto t2 = std::chrono::steady_clock::now();
                u64 diff = std::chrono::duration_cast<std::chrono::seconds>(t2 - t1).count();
                if(diff >= 1)
                {
                    t1 = t2;
                    bsec = tmpwritten;
                    tmpwritten = 0;
                }
                u64 rbytes = 0;
                u64 rsize = std::min(szrem, reads);
                switch(rnca.Type)
                {
                    case ncm::ContentType::Meta:
                    case ncm::ContentType::Control:
                        rbytes = nsys->ReadFileBlock("Contents/temp/" + ncaname, noff, rsize, rdata);
                        break;
                    default:
                        rbytes = nspentry.ReadFromFile(idxncaname, noff, rsize, rdata);
                        break;   
                }
                ncm::WritePlaceHolder(&cst, &curid, noff, rdata, rbytes);
                noff += rbytes;
                tmpwritten += (double)rbytes;
                szrem -= rbytes;
                OnContentWrite(rnca, i, ncas.size(), (double)noff, (double)ncasize, bsec);
            }
            ncmContentStorageRegister(&cst, &curid, &curid);
            ncm::DeletePlaceHolder(&cst, &curid);
            serviceClose(&cst.s);
        }
        
        return rc;
    }

    void Installer::FinalizeInstallation()
    {
        free(entrynacp);
        entrynacp = NULL;
        fs::Explorer *nsys = fs::GetNANDSystemExplorer();
        nsys->DeleteDirectory("Contents/temp");
    }
}