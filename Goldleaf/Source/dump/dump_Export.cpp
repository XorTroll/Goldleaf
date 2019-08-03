#include <dump/dump_Export.hpp>
#include <hos/hos_Titles.hpp>
#include <fatfs/fatfs.hpp>
#include <es/es_Service.hpp>
#include <sstream>
#include <iomanip>

FsStorage fatfs_bin;

namespace dump
{
    void DecryptCopyNAX0ToNCA(NcmContentStorage *ncst, NcmNcaId NCAId, std::string Path, std::function<void(double Done, double Total)> Callback)
    {
        u64 ncasize = 0;
        ncmContentStorageGetSize(ncst, &NCAId, &ncasize);
        u64 szrem = ncasize;
        FILE *f = fopen(Path.c_str(), "wb");
        u64 off = 0;
        u64 rmax = fs::GetFileSystemOperationsBufferSize();
        u8 *data = fs::GetFileSystemOperationsBuffer();
        while(szrem)
        {
            u64 rsize = std::min(rmax, szrem);
            if(ncmContentStorageReadContentIdFile(ncst, &NCAId, off, data, rsize) != 0) break;
            fwrite(data, 1, rsize, f);
            szrem -= rsize;
            off += rsize;
            Callback((double)off, (double)ncasize);
        }
        fclose(f);
    }

    bool GetMetaRecord(NcmContentMetaDatabase *metadb, u64 ApplicationId, NcmMetaRecord *out)
    {
        size_t size = sizeof(NcmMetaRecord) * hos::MaxTitleCount;
        NcmMetaRecord *metas = new NcmMetaRecord[hos::MaxTitleCount];
        u32 total = 0;
        u32 written = 0;
        bool got = false;
        Result rc = ncmContentMetaDatabaseList(metadb, 0, ApplicationId, 0, U64_MAX, metas, size, &written, &total);
        if((rc == 0) && (written > 0)) 
        {
            for(u32 i = 0; i < written; i++)
            {
                if(metas[i].titleId == ApplicationId)
                {
                    memcpy(out, &metas[i], sizeof(NcmMetaRecord));
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
        ncmOpenContentMetaDatabase(FsStorageId_SdCard, &cmdb);
        NcmMetaRecord rec;
        bool ok = GetMetaRecord(&cmdb, ApplicationId, &rec);
        if(ok) stid = FsStorageId_SdCard;
        else
        {
            serviceClose(&cmdb.s);
            ncmOpenContentMetaDatabase(FsStorageId_NandUser, &cmdb);
            NcmMetaRecord rec;
            bool ok = GetMetaRecord(&cmdb, ApplicationId, &rec);
            if(ok) stid = FsStorageId_NandUser;
        }
        serviceClose(&cmdb.s);
        return stid;
    }

    std::string GetTitleKeyData(u64 ApplicationId, bool ExportData)
    {
        fsOpenBisStorage(&fatfs_bin, FsBisStorageId_System);
        FATFS fs;
        FIL save;
        f_mount(&fs, "0", 1);
        f_chdir("/save");
        f_open(&save, "80000000000000e1", (FA_READ | FA_OPEN_EXISTING));
        std::string tkey = "";
        std::string orid = "";
        std::string fappid = hos::FormatApplicationId(ApplicationId);
        std::string outdir = "sdmc:/" + GoldleafDir + "/dump/" + fappid;
        u32 tmpsz = 0;
        while(true)
        {
            if(tkey != "") break;
            u8 *tkdata = new u8[0x40000];
            FRESULT fr = f_read(&save, tkdata, 0x40000, &tmpsz);
            if(fr) break;
            if(tmpsz == 0) break;
            for(u32 i = 0; i < tmpsz; i += 0x4000)
            {
                if(tkey != "") break;
                for(u32 j = 0; j < (i + 0x4000); j += 0x400)
                {
                    if(tkey != "") break;
                    if(*reinterpret_cast<u32*>(&tkdata[j]) == 0x10004)
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
                        std::string tid = stid.str();
                        std::string rid = srid.str();
                        std::string etkey = stkey.str();
                        if(fappid == tid)
                        {
                            orid = rid;
                            if(ExportData)
                            {
                                FILE *tikf = fopen((outdir + "/" + rid + ".tik").c_str(), "wb");
                                fwrite(&tkdata[j], 1, 0x400, tikf);
                                fclose(tikf);
                            }
                            tkey = etkey;
                            break;
                        }
                    }
                }
            }
            delete[] tkdata;
        }
        f_close(&save);
        f_mount(NULL, "0", 1);
        fsStorageClose(&fatfs_bin);
        if(ExportData && (tkey != ""))
        {
            FILE *ceout = fopen((outdir + "/" + orid + ".cert").c_str(), "wb");
            fwrite(es::CertData, 1792, 1, ceout);
            fclose(ceout);
        }
        return "";
    }

    std::string GetNCAIdPath(NcmContentStorage *st, NcmNcaId *Id)
    {
        char out[FS_MAX_PATH] = { 0 };
        Result rc = ncmContentStorageGetPath(st, Id, out, FS_MAX_PATH);
        std::string sst = "";
        if(rc == 0) sst = std::string(out);
        return sst;
    }

    bool GetNCAId(NcmContentMetaDatabase *cmdb, NcmMetaRecord *rec, u64 ApplicationId, NCAType Type, NcmNcaId *out)
    {
        NcmContentType ctype = NcmContentType_Program;
        switch(Type)
        {
            case NCAType::Program:
                ctype = NcmContentType_Program;
                break;
            case NCAType::Control:
                ctype = NcmContentType_Icon;
                break;
            case NCAType::Meta:
                ctype = NcmContentType_CNMT;
                break;
            case NCAType::LegalInfo:
                ctype = NcmContentType_Info;
                break;
            case NCAType::OfflineHtml:
                ctype = NcmContentType_Doc;
                break;
            case NCAType::Data:
                ctype = NcmContentType_Data;
                break;
        }
        Result rc = ncmContentMetaDatabaseGetContentIdByType(cmdb, ctype, rec, out);
        return (rc == 0);
    }
}