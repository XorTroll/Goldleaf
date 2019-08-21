#include <nsp/nsp_Installer.hpp>
#include <err/err_Errors.hpp>
#include <fs/fs_Explorer.hpp>
#include <sys/stat.h>
#include <hos/hos_Common.hpp>
#include <fstream>
#include <malloc.h>
#include <dirent.h>
#include <chrono>

extern set::Settings gsets;

namespace nsp
{
    Installer::Installer(pu::String Path, fs::Explorer *Exp, Storage Location) : nspentry(Exp, Path), storage(static_cast<FsStorageId>(Location))
    {
    }

    Installer::~Installer()
    {
        FinalizeInstallation();
    }

    Result Installer::PrepareInstallation()
    {
        Result rc = err::Make(err::ErrorDescription::InvalidNSP);
        if(nspentry.IsOk())
        {
            rc = 0;
            pu::String cnmtnca;
            u32 idxcnmtnca = 0;
            u64 scnmtnca = 0;
            u32 idxtik = 0;
            stik = 0;
            auto files = nspentry.GetFiles();
            for(u32 i = 0; i < files.size(); i++)
            {
                pu::String file = files[i];
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
            if(scnmtnca == 0)
            {
                rc = err::Make(err::ErrorDescription::MetaNotFound);
                return rc;
            }
            pu::String icnmtnca = fs::GetFileName(cnmtnca);
            fs::Explorer *nsys = fs::GetNANDSystemExplorer();
            nsys->CreateDirectory("Contents/temp");
            pu::String ncnmtnca = nsys->FullPathFor("Contents/temp/" + cnmtnca);
            nsys->DeleteFile(ncnmtnca);
            nspentry.SaveFile(idxcnmtnca, nsys, ncnmtnca);
            pu::String acnmtnca = "@SystemContent://temp/" + cnmtnca;
            acnmtnca.reserve(FS_MAX_PATH);
            FsRightsId rid = {};
            rc = fsGetRightsIdAndKeyGenerationByPath(acnmtnca.AsUTF8().c_str(), &keygen, &rid);
            if(rc != 0) return rc;
            u8 systemkgen = hos::ComputeSystemKeyGeneration();
            if(systemkgen < keygen)
            {
                rc = err::Make(err::ErrorDescription::KeyGenMismatch);
                return rc;
            }
            FsFileSystem cnmtncafs;
            rc = fsOpenFileSystemWithId(&cnmtncafs, 0, FsFileSystemType_ContentMeta, acnmtnca.AsUTF8().c_str());
            if(rc != 0) return rc;
            {
                fs::FileSystemExplorer cnmtfs("gnspcnmtnca", "NSP-ContentMeta", &cnmtncafs);
                auto cnts = cnmtfs.GetContents();
                pu::String fcnmt;
                for(u32 i = 0; i < cnts.size(); i++)
                {
                    pu::String cnt = cnts[i];
                    if(fs::GetExtension(cnt) == "cnmt")
                    {
                        fcnmt = cnt;
                        break;
                    }
                }
                if(fcnmt.empty())
                {
                    rc = err::Make(err::ErrorDescription::MetaNotFound);
                    return rc;
                }
                u64 fcnmtsz = cnmtfs.GetFileSize(fcnmt);
                u8 *cnmtbuf = new u8[fcnmtsz]();
                cnmtfs.ReadFileBlock(fcnmt, 0, fcnmtsz, cnmtbuf);
                cnmt = ncm::ContentMeta(cnmtbuf, fcnmtsz);
                delete[] cnmtbuf;
            }
            ncm::ContentRecord record = {};
            record.ContentId = hos::StringAsContentId(icnmtnca);
            *(u64*)record.Size = (scnmtnca & 0xffffffffffff);
            record.Type = ncm::ContentType::Meta;
            auto tmprec = cnmt.GetContentMetaKey();
            memcpy(&mrec, &tmprec, sizeof(NcmMetaRecord));
            if(hos::ExistsTitle(ncm::ContentMetaType::Any, Storage::SdCard, mrec.titleId))
            {
                rc = err::Make(err::ErrorDescription::TitleAlreadyInstalled);
                return rc;
            }
            if(hos::ExistsTitle(ncm::ContentMetaType::Any, Storage::NANDUser, mrec.titleId))
            {
                rc = err::Make(err::ErrorDescription::TitleAlreadyInstalled);
                return rc;
            }
            NcmContentStorage cst;
            rc = ncmOpenContentStorage(storage, &cst);
            bool hascnmt = false;
            ncmContentStorageHas(&cst, &record.ContentId, &hascnmt);
            serviceClose(&cst.s);
            if(!hascnmt) ncas.push_back(record);
            cnmt.GetInstallContentMeta(ccnmt, record, gsets.IgnoreRequiredFirmwareVersion);
            baseappid = hos::GetBaseApplicationId(mrec.titleId, static_cast<ncm::ContentMetaType>(mrec.type));
            auto recs = cnmt.GetContentRecords();
            memset(&entrynacp, 0, sizeof(entrynacp));
            pu::String ptik = nsys->FullPathFor("Contents/temp/" + tik);
            if(stik > 0)
            {
                nspentry.SaveFile(idxtik, nsys, ptik);
                entrytik = hos::ReadTicket(ptik);
            }
            for(u32 i = 0; i < recs.size(); i++)
            {
                ncas.push_back(recs[i]);
                if(recs[i].Type == ncm::ContentType::Control)
                {
                    pu::String controlncaid = hos::ContentIdAsString(recs[i].ContentId);
                    pu::String controlnca = controlncaid + ".nca";
                    u32 idxcontrolnca = nspentry.GetFileIndexByName(controlnca);
                    auto ncontrolnca = nsys->FullPathFor("Contents/temp/" + controlnca);
                    nspentry.SaveFile(idxcontrolnca, nsys, ncontrolnca);
                    pu::String acontrolnca = "@SystemContent://temp/" + controlnca;
                    acontrolnca.reserve(FS_MAX_PATH);
                    FsFileSystem controlncafs;
                    auto rc2 = fsOpenFileSystemWithId(&controlncafs, mrec.titleId, FsFileSystemType_ContentControl, acontrolnca.AsUTF8().c_str());
                    if(rc2 == 0)
                    {
                        fs::FileSystemExplorer controlfs("gnspcontrolnca", "NSP-Control", &controlncafs);
                        auto cnts = controlfs.GetContents();
                        for(u32 i = 0; i < cnts.size(); i++)
                        {
                            pu::String cnt = cnts[i];
                            if(fs::GetExtension(cnt) == "dat")
                            {
                                icon = "sdmc:/" + GoldleafDir + "/meta/" + controlncaid + ".jpg";
                                controlfs.CopyFile(cnt, icon);
                                break;
                            }
                        }
                        controlfs.ReadFileBlock("control.nacp", 0, sizeof(NacpStruct), (u8*)&entrynacp);
                    }
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
        rc = ncmContentMetaDatabaseSet(&mdb, &mrec, ccnmt.GetSize(), (NcmContentMetaRecordsHeader*)ccnmt.GetData());
        if(rc != 0)
        {
            serviceClose(&mdb.s);
            return rc;
        }
        rc = ncmContentMetaDatabaseCommit(&mdb);
        serviceClose(&mdb.s);
        if(rc != 0) return rc;
        u32 cmetacount = 0;
        rc = ns::CountApplicationContentMeta(baseappid, &cmetacount);
        if(rc == 0x410) rc = 0;
        if(rc != 0) return rc;
        std::vector<ns::ContentStorageRecord> srecs;
        if(cmetacount > 0)
        {
            srecs.resize(cmetacount);
            size_t csbufs = (cmetacount * sizeof(ns::ContentStorageRecord));
            ns::ContentStorageRecord *csbuf = new ns::ContentStorageRecord[cmetacount]();
            u32 cmcount = 0;
            rc = ns::ListApplicationRecordContentMeta(0, baseappid, csbuf, csbufs, &cmcount);
            if(rc != 0)
            {
                delete[] csbuf;
                return rc;
            }
            memcpy(srecs.data(), csbuf, csbufs);
            delete[] csbuf;
        }
        ns::ContentStorageRecord csrecord = {};
        csrecord.Record = mrec;
        csrecord.StorageId = storage;
        srecs.push_back(csrecord);
        ns::DeleteApplicationRecord(baseappid);
        rc = ns::PushApplicationRecord(baseappid, 3, srecs.data(), srecs.size() * sizeof(ns::ContentStorageRecord));
        if(rc != 0) return rc;
        if(stik > 0)
        {
            u8 *tikbuf = new u8[stik]();
            nsys->ReadFileBlock("Contents/temp/" + tik, 0, stik, tikbuf);
            es::ImportTicket(tikbuf, stik, es::CertData, 1792);
            delete[] tikbuf;
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
        return icon.AsUTF8();
    }

    NacpStruct *Installer::GetNACP()
    {
        return &entrynacp;
    }

    bool Installer::HasTicket()
    {
        return (stik > 0);
    }

    hos::TicketData Installer::GetTicketData()
    {
        return entrytik;
    }

    u8 Installer::GetKeyGeneration()
    {
        return keygen;
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
        u64 totalsize = 0;
        u64 twrittensize = 0;
        std::vector<pu::String> ncanames;
        std::vector<u64> ncasizes;
        std::vector<u32> ncaidxs;
        for(u32 i = 0; i < ncas.size(); i++)
        {
            ncm::ContentRecord rnca = ncas[i];
            NcmNcaId curid = rnca.ContentId;
            pu::String ncaname = hos::ContentIdAsString(curid);
            if(rnca.Type == ncm::ContentType::Meta) ncaname += ".cnmt";
            ncaname += ".nca";
            u32 idxncaname = nspentry.GetFileIndexByName(ncaname);
            auto cursize =  nspentry.GetFileSize(idxncaname);
            totalsize += cursize;
            ncaidxs.push_back(idxncaname);
            ncanames.push_back(ncaname);
            ncasizes.push_back(cursize);
        }
        for(u32 i = 0; i < ncas.size(); i++)
        {
            ncm::ContentRecord rnca = ncas[i];
            NcmNcaId curid = rnca.ContentId;
            pu::String ncaname = ncanames[i];
            u64 ncasize = ncasizes[i];
            u32 idxncaname = ncaidxs[i];

            NcmContentStorage cst;
            ncmOpenContentStorage(storage, &cst);
            ncm::DeletePlaceHolder(&cst, &curid);
            ncm::CreatePlaceHolder(&cst, &curid, &curid, ncasize);
            u64 noff = 0;
            u64 szrem = ncasize;
            while(szrem)
            {
                u64 rbytes = 0;
                u64 rsize = std::min(szrem, reads);
                auto t1 = std::chrono::steady_clock::now();
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
                szrem -= rbytes;
                auto t2 = std::chrono::steady_clock::now();
                u64 diff = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
                double bsec = (1000.0f / (double)diff) * rbytes; // By elapsed time and written bytes, compute how much data has been written in 1sec.
                OnContentWrite(rnca, i, ncas.size(), (double)(noff + twrittensize), (double)totalsize, (u64)bsec);
            }
            twrittensize += noff;
            ncmContentStorageRegister(&cst, &curid, &curid);
            ncm::DeletePlaceHolder(&cst, &curid);
            serviceClose(&cst.s);
        }
        return rc;
    }

    void Installer::FinalizeInstallation()
    {
        fs::Explorer *nsys = fs::GetNANDSystemExplorer();
        nsys->DeleteDirectory("Contents/temp");
    }
}