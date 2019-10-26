
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

#include <dump/dump_Export.hpp>
#include <hos/hos_Titles.hpp>
#include <fatfs/fatfs.hpp>
#include <es/es_Service.hpp>
#include <sstream>
#include <iomanip>

FsStorage fatfs_bin;

namespace dump
{
    void DecryptCopyNAX0ToNCA(NcmContentStorage *ncst, NcmContentId NCAId, pu::String Path, std::function<void(double Done, double Total)> Callback)
    {
        s64 ncasize = 0;
        ncmContentStorageGetSizeFromContentId(ncst, &ncasize, &NCAId);
        u64 szrem = ncasize;
        FILE *f = fopen(Path.AsUTF8().c_str(), "wb");
        u64 off = 0;
        u64 rmax = fs::GetFileSystemOperationsBufferSize();
        u8 *data = fs::GetFileSystemOperationsBuffer();
        while(szrem)
        {
            u64 rsize = std::min(rmax, szrem);
            if(ncmContentStorageReadContentIdFile(ncst, data, off, &NCAId, rsize) != 0) break;
            fwrite(data, 1, rsize, f);
            szrem -= rsize;
            off += rsize;
            Callback((double)off, (double)ncasize);
        }
        fclose(f);
    }

    bool GetMetaRecord(NcmContentMetaDatabase *metadb, u64 ApplicationId, NcmContentMetaKey *out)
    {
        size_t size = sizeof(NcmContentMetaKey) * hos::MaxTitleCount;
        NcmContentMetaKey *metas = new NcmContentMetaKey[hos::MaxTitleCount]();
        s32 total = 0;
        s32 written = 0;
        bool got = false;
        Result rc = ncmContentMetaDatabaseList(metadb, &total, &written, metas, size, NcmContentMetaType_Unknown, ApplicationId, 0, U64_MAX, NcmContentInstallType_Full);
        if((rc == 0) && (written > 0)) 
        {
            for(s32 i = 0; i < written; i++)
            {
                if(metas[i].title_id == ApplicationId)
                {
                    memcpy(out, &metas[i], sizeof(NcmContentMetaKey));
                    got = true;
                    break;
                }
            }
        }
        delete[] metas;
        return got;
    }

    FsStorageId GetApplicationLocation(u64 ApplicationId)
    {
        FsStorageId stid = FsStorageId_None;
        NcmContentMetaDatabase cmdb;
        ncmOpenContentMetaDatabase(&cmdb, FsStorageId_SdCard);
        NcmContentMetaKey rec;
        bool ok = GetMetaRecord(&cmdb, ApplicationId, &rec);
        if(ok) stid = FsStorageId_SdCard;
        else
        {
            serviceClose(&cmdb.s);
            ncmOpenContentMetaDatabase(&cmdb, FsStorageId_NandUser);
            NcmContentMetaKey rec;
            bool ok = GetMetaRecord(&cmdb, ApplicationId, &rec);
            if(ok) stid = FsStorageId_NandUser;
        }
        serviceClose(&cmdb.s);
        return stid;
    }

    void GenerateTicketCert(u64 ApplicationId)
    {
        auto rc = fsOpenBisStorage(&fatfs_bin, FsBisStorageId_System);
        if(R_SUCCEEDED(rc))
        {
            auto exp = fs::GetSdCardExplorer();
            FATFS fs;
            FIL save;
            f_mount(&fs, "0", 1);
            f_chdir("/save");
            f_open(&save, "80000000000000e1", (FA_READ | FA_OPEN_EXISTING));
            pu::String tkey;
            pu::String orid;
            pu::String fappid = hos::FormatApplicationId(ApplicationId);
            pu::String outdir = "sdmc:/" + GoldleafDir + "/dump/title/" + fappid;
            u32 tmpsz = 0;
            while(true)
            {
                if(!tkey.empty()) break;
                u8 *tkdata = fs::GetFileSystemOperationsBuffer();
                FRESULT fr = f_read(&save, tkdata, 0x40000, &tmpsz);
                if(fr) break;
                if(tmpsz == 0) break;
                for(u32 i = 0; i < tmpsz; i += 0x4000)
                {
                    if(!tkey.empty()) break;
                    for(u32 j = 0; j < (i + 0x4000); j += 0x400)
                    {
                        if(!tkey.empty()) break;
                        if(hos::IsValidTicketSignature(*reinterpret_cast<u32*>(&tkdata[j])))
                        {
                            std::stringstream stid;
                            std::stringstream srid;
                            for(u32 k = 0; k < 0x10; k++)
                            {
                                u32 off = j + 0x2a0 + k;
                                srid << std::setw(2) << std::setfill('0') << std::hex << (int)tkdata[off];
                            }
                            for(u32 k = 0; k < 0x8; k++)
                            {
                                u32 off = j + 0x2a0 + k;
                                stid << std::setw(2) << std::setfill('0') << std::uppercase << std::hex << (int)tkdata[off];
                            }
                            std::stringstream stkey;
                            for(u32 k = 0; k < 0x10; k++)
                            {
                                u32 off = j + 0x180 + k;
                                stkey << std::setw(2) << std::setfill('0') << std::uppercase << std::hex << (int)tkdata[off];
                            }
                            pu::String tid = stid.str();
                            pu::String rid = srid.str();
                            pu::String etkey = stkey.str();
                            if(fappid == tid)
                            {
                                orid = rid;
                                exp->WriteFileBlock(outdir + "/" + rid + ".tik", &tkdata[j], 0x400);
                                tkey = etkey;
                                break;
                            }
                        }
                    }
                }
            }
            f_close(&save);
            f_mount(NULL, "0", 1);
            fsStorageClose(&fatfs_bin);
            if(!tkey.empty()) exp->WriteFileBlock(outdir + "/" + orid + ".cert", const_cast<u8*>(es::CertData), es::CertSize);
        }
    }

    pu::String GetContentIdPath(NcmContentStorage *st, NcmContentId *Id)
    {
        char out[FS_MAX_PATH] = { 0 };
        Result rc = ncmContentStorageGetPath(st, out, FS_MAX_PATH, Id);
        pu::String sst = "";
        if(rc == 0) sst = pu::String(out);
        return sst;
    }

    bool GetContentId(NcmContentMetaDatabase *cmdb, NcmContentMetaKey *rec, u64 ApplicationId, NCAType Type, NcmContentId *out)
    {
        NcmContentType ctype = NcmContentType_Program;
        switch(Type)
        {
            case NCAType::Program:
                ctype = NcmContentType_Program;
                break;
            case NCAType::Control:
                ctype = NcmContentType_Control;
                break;
            case NCAType::Meta:
                ctype = NcmContentType_Meta;
                break;
            case NCAType::LegalInfo:
                ctype = NcmContentType_LegalInformation;
                break;
            case NCAType::OfflineHtml:
                ctype = NcmContentType_HtmlDocument;
                break;
            case NCAType::Data:
                ctype = NcmContentType_Data;
                break;
        }
        Result rc = ncmContentMetaDatabaseGetContentIdByType(cmdb, out, rec, ctype);
        return (rc == 0);
    }
}