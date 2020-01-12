
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2019  XorTroll

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

#include <nsp/nsp_Installer.hpp>
#include <err/err_Result.hpp>
#include <fs/fs_FileSystem.hpp>
#include <sys/stat.h>
#include <hos/hos_Common.hpp>
#include <fstream>
#include <malloc.h>
#include <dirent.h>
#include <chrono>

extern cfg::Settings global_settings;

namespace nsp
{
    Installer::Installer(String Path, fs::Explorer *Exp, Storage Location) : nspentry(Exp, Path), storage(static_cast<NcmStorageId>(Location))
    {
    }

    Installer::~Installer()
    {
        FinalizeInstallation();
    }

    Result Installer::PrepareInstallation()
    {
        auto rc = err::result::ResultInvalidNSP;
        if(nspentry.IsOk())
        {
            rc = 0;
            String cnmtnca;
            u32 idxcnmtnca = 0;
            u64 scnmtnca = 0;
            u32 idxtik = 0;
            stik = 0;
            auto files = nspentry.GetFiles();
            for(u32 i = 0; i < files.size(); i++)
            {
                String file = files[i];
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
                rc = err::result::ResultMetaNotFound;
                return rc;
            }
            String icnmtnca = fs::GetFileName(cnmtnca);
            fs::Explorer *nsys = fs::GetNANDSystemExplorer();
            nsys->CreateDirectory("Contents/temp");
            String ncnmtnca = nsys->FullPathFor("Contents/temp/" + cnmtnca);
            nsys->DeleteFile(ncnmtnca);
            nspentry.SaveFile(idxcnmtnca, nsys, ncnmtnca);
            String acnmtnca = "@SystemContent://temp/" + cnmtnca;
            acnmtnca.reserve(FS_MAX_PATH);
            FsRightsId rid = {};
            rc = fsGetRightsIdAndKeyGenerationByPath(acnmtnca.AsUTF8().c_str(), &keygen, &rid);
            if(R_FAILED(rc)) return rc;
            u8 systemkgen = hos::ComputeSystemKeyGeneration();
            if(systemkgen < keygen)
            {
                rc = err::result::ResultKeyGenMismatch;
                return rc;
            }
            FsFileSystem cnmtncafs;
            rc = fsOpenFileSystemWithId(&cnmtncafs, 0, FsFileSystemType_ContentMeta, acnmtnca.AsUTF8().c_str());
            if(R_FAILED(rc)) return rc;
            {
                fs::FspExplorer cnmtfs("NSP-ContentMeta", cnmtncafs);
                auto cnts = cnmtfs.GetContents();
                String fcnmt;
                for(u32 i = 0; i < cnts.size(); i++)
                {
                    String cnt = cnts[i];
                    if(fs::GetExtension(cnt) == "cnmt")
                    {
                        fcnmt = cnt;
                        break;
                    }
                }
                if(fcnmt.empty())
                {
                    rc = err::result::ResultMetaNotFound;
                    return rc;
                }
                u64 fcnmtsz = cnmtfs.GetFileSize(fcnmt);
                u8 *cnmtbuf = new u8[fcnmtsz]();
                cnmtfs.StartFile(fcnmt, fs::FileMode::Read);
                cnmtfs.ReadFileBlock(fcnmt, 0, fcnmtsz, cnmtbuf);
                cnmtfs.EndFile(fs::FileMode::Read);
                cnmt = ncm::ContentMeta(cnmtbuf, fcnmtsz);
                delete[] cnmtbuf;
            }
            ncm::ContentRecord record = {};
            record.ContentId = hos::StringAsContentId(icnmtnca);
            *(u64*)record.Size = (scnmtnca & 0xffffffffffff);
            record.Type = ncm::ContentType::Meta;
            auto tmprec = cnmt.GetContentMetaKey();
            memcpy(&mrec, &tmprec, sizeof(NcmContentMetaKey));
            if(hos::ExistsTitle(ncm::ContentMetaType::Any, Storage::SdCard, mrec.id))
            {
                rc = err::result::ResultTitleAlreadyInstalled;
                return rc;
            }
            if(hos::ExistsTitle(ncm::ContentMetaType::Any, Storage::NANDUser, mrec.id))
            {
                rc = err::result::ResultTitleAlreadyInstalled;
                return rc;
            }
            NcmContentStorage cst;
            rc = ncmOpenContentStorage(&cst, storage);
            bool hascnmt = false;
            ncmContentStorageHas(&cst, &hascnmt, &record.ContentId);
            serviceClose(&cst.s);
            if(!hascnmt) ncas.push_back(record);
            cnmt.GetInstallContentMeta(ccnmt, record, global_settings.ignore_required_fw_ver);
            baseappid = hos::GetBaseApplicationId(mrec.id, static_cast<ncm::ContentMetaType>(mrec.type));
            auto recs = cnmt.GetContentRecords();
            memset(&entrynacp, 0, sizeof(entrynacp));
            String ptik = nsys->FullPathFor("Contents/temp/" + tik);
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
                    String controlncaid = hos::ContentIdAsString(recs[i].ContentId);
                    String controlnca = controlncaid + ".nca";
                    u32 idxcontrolnca = nspentry.GetFileIndexByName(controlnca);
                    auto ncontrolnca = nsys->FullPathFor("Contents/temp/" + controlnca);
                    nspentry.SaveFile(idxcontrolnca, nsys, ncontrolnca);
                    String acontrolnca = "@SystemContent://temp/" + controlnca;
                    acontrolnca.reserve(FS_MAX_PATH);
                    FsFileSystem controlncafs;
                    auto rc2 = fsOpenFileSystemWithId(&controlncafs, mrec.id, FsFileSystemType_ContentControl, acontrolnca.AsUTF8().c_str());
                    if(rc2 == 0)
                    {
                        fs::FspExplorer controlfs("NSP-Control", controlncafs);
                        auto cnts = controlfs.GetContents();
                        for(u32 i = 0; i < cnts.size(); i++)
                        {
                            String cnt = cnts[i];
                            if(fs::GetExtension(cnt) == "dat")
                            {
                                icon = "sdmc:/" + consts::Root + "/meta/" + controlncaid + ".jpg";
                                controlfs.CopyFile(cnt, icon);
                                break;
                            }
                        }
                        auto fcontrol = "control.nacp";
                        controlfs.StartFile(fcontrol, fs::FileMode::Read);
                        controlfs.ReadFileBlock(fcontrol, 0, sizeof(NacpStruct), (u8*)&entrynacp);
                        controlfs.EndFile(fs::FileMode::Read);
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
        Result rc = ncmOpenContentMetaDatabase(&mdb, storage);
        if(R_FAILED(rc)) return rc;
        rc = ncmContentMetaDatabaseSet(&mdb, &mrec, ccnmt.GetData(), ccnmt.GetSize());
        if(R_FAILED(rc))
        {
            serviceClose(&mdb.s);
            return rc;
        }
        rc = ncmContentMetaDatabaseCommit(&mdb);
        serviceClose(&mdb.s);
        if(R_FAILED(rc)) return rc;
        u32 cmetacount = 0;
        rc = ns::CountApplicationContentMeta(baseappid, &cmetacount);
        if(rc == 0x410) rc = 0;
        if(R_FAILED(rc)) return rc;
        std::vector<ns::ContentStorageRecord> srecs;
        if(cmetacount > 0)
        {
            srecs.resize(cmetacount);
            size_t csbufs = (cmetacount * sizeof(ns::ContentStorageRecord));
            ns::ContentStorageRecord *csbuf = new ns::ContentStorageRecord[cmetacount]();
            u32 cmcount = 0;
            rc = ns::ListApplicationRecordContentMeta(0, baseappid, csbuf, csbufs, &cmcount);
            if(R_FAILED(rc))
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
        if(stik > 0)
        {
            u8 *tikbuf = fs::GetFileSystemOperationsBuffer();
            auto nmtik = "Contents/temp/" + tik;
            nsys->StartFile(nmtik, fs::FileMode::Read);
            nsys->ReadFileBlock(nmtik, 0, stik, tikbuf);
            nsys->EndFile(fs::FileMode::Read);
            rc = es::ImportTicket(tikbuf, stik, es::CertData, es::CertSize);
        }
        return rc;
    }

    ncm::ContentMetaType Installer::GetContentMetaType()
    {
        return static_cast<ncm::ContentMetaType>(mrec.type);
    }

    u64 Installer::GetApplicationId()
    {
        return mrec.id;
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
        std::vector<String> ncanames;
        std::vector<u64> ncasizes;
        std::vector<u32> ncaidxs;
        for(auto &rnca: ncas)
        {
            NcmContentId curid = rnca.ContentId;
            String ncaname = hos::ContentIdAsString(curid);
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
            NcmContentId curid = rnca.ContentId;
            String ncaname = ncanames[i];
            u64 ncasize = ncasizes[i];
            u32 idxncaname = ncaidxs[i];

            NcmContentStorage cst;
            ncmOpenContentStorage(&cst, storage);

            NcmPlaceHolderId plhdid = {};
            memcpy(plhdid.uuid.uuid, curid.c, sizeof(curid.c));
            
            ncmContentStorageDeletePlaceHolder(&cst, &plhdid);
            ncmContentStorageCreatePlaceHolder(&cst, &curid, &plhdid, ncasize);
            u64 noff = 0;
            u64 szrem = ncasize;
            auto nmnca = "Contents/temp/" + ncaname;
            switch(rnca.Type)
            {
                case ncm::ContentType::Meta:
                case ncm::ContentType::Control:
                    nsys->StartFile(nmnca, fs::FileMode::Read);
                    break;
                default:
                    nspentry.GetExplorer()->StartFile(nspentry.GetPath(), fs::FileMode::Read);
                    break;
            }
            while(szrem)
            {
                u64 rbytes = 0;
                u64 rsize = std::min(szrem, reads);
                auto t1 = std::chrono::steady_clock::now();
                switch(rnca.Type)
                {
                    case ncm::ContentType::Meta:
                    case ncm::ContentType::Control:
                        rbytes = nsys->ReadFileBlock(nmnca, noff, rsize, rdata);
                        break;
                    default:
                        rbytes = nspentry.ReadFromFile(idxncaname, noff, rsize, rdata);
                        break;
                }
                ncmContentStorageWritePlaceHolder(&cst, &plhdid, noff, rdata, rbytes);
                noff += rbytes;
                szrem -= rbytes;
                auto t2 = std::chrono::steady_clock::now();
                u64 diff = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
                double bsec = (1000.0f / (double)diff) * rbytes; // By elapsed time and written bytes, compute how much data has been written in 1sec.
                OnContentWrite(rnca, i, ncas.size(), (double)(noff + twrittensize), (double)totalsize, (u64)bsec);
            }
            switch(rnca.Type)
            {
                case ncm::ContentType::Meta:
                case ncm::ContentType::Control:
                    nsys->EndFile(fs::FileMode::Read);
                    break;
                default:
                    nspentry.GetExplorer()->EndFile(fs::FileMode::Read);
                    break;
            }
            twrittensize += noff;
            ncmContentStorageRegister(&cst, &curid, &plhdid);
            ncmContentStorageDeletePlaceHolder(&cst, &plhdid);
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