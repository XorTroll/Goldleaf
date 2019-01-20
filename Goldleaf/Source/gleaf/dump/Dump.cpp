#include <gleaf/dump/Dump.hpp>
#include <gleaf/dump/DebugTypes.hpp>
#include <gleaf/Application.hpp>
#include <gleaf/hactool.hpp>
#include <gleaf/horizon.hpp>
#include <fatfs/fatfs.hpp>
#include <gleaf/es.hpp>
#include <sstream>
#include <iomanip>

FsStorage fatfs_bin;

namespace gleaf::dump
{
    void DecryptCopyNAX0ToNCA(NcmContentStorage *ncst, NcmNcaId NCAId, std::string Path, std::function<void(u8 Percentage)> Callback)
    {
        u64 ncasize = 0;
        ncmContentStorageGetSize(ncst, &NCAId, &ncasize);
        u64 szrem = ncasize;
        FILE *f = fopen(Path.c_str(), "wb");
        u64 off = 0;
        while(szrem)
        {
            u64 rsize = std::min((u64)1048576, szrem);
            u8 *data = (u8*)malloc(sizeof(u8) * rsize);
            if(ncmContentStorageReadContentIdFile(ncst, &NCAId, off, data, rsize) != 0) break;
            fwrite(data, 1, rsize, f);
            szrem -= rsize;
            off += rsize;
            u8 perc = ((double)((double)off / (double)ncasize) * 100.0);
            Callback(perc);
            free(data);
        }
        fclose(f);
    }

    bool GetMetaRecord(NcmContentMetaDatabase *metadb, u64 ApplicationId, NcmMetaRecord *out)
    {
        size_t size = sizeof(NcmApplicationContentMetaKey) * 128;
        NcmApplicationContentMetaKey *metas = (NcmApplicationContentMetaKey*)malloc(size);
        u32 total = 0;
        u32 written = 0;
        bool got = false;
        Result rc = ncmContentMetaDatabaseListApplication(metadb, 0x80, metas, size, &written, &total);
        if(rc == 0) if(total > 0) for(u32 i = 0; i < total; i++) if(metas[i].metaRecord.titleId == ApplicationId)
        {
            *out = metas[i].metaRecord;
            got = true;
            break;
        }
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

    u32 TerminateES()
    {
        es::Finalize();
        pmdmntInitialize();
        pmshellInitialize();
        u64 espid = 0;
        pmdmntGetTitlePid(&espid, 0x0100000000000033);
        Handle eshandle;
        svcDebugActiveProcess(&eshandle, espid);
        DebugEventInfo dev;
        svcGetDebugEvent((u8*)&dev, eshandle);
        svcCloseHandle(eshandle);
        u32 flags = dev.info.attach_process.flags & 0x1dd;
        pmshellTerminateProcessByTitleId(0x0100000000000033);
        return flags;
    }

    void RelaunchES(u32 ESFlags)
    {
        u64 espid = 0;
        pmshellLaunchProcess(ESFlags, 0x0100000000000033, FsStorageId_NandSystem, &espid);
        pmdmntExit();
        pmshellExit();
        es::Initialize();
    }

    std::string GetTitleKeyData(u64 ApplicationId, bool ExportData)
    {
        fsOpenBisStorage(&fatfs_bin, 31);
        fatfs::FATFS fs;
        fatfs::FIL save;
        fatfs::f_mount(&fs, "", 1);
        fatfs::f_chdir("/save");
        fatfs::f_open(&save, "80000000000000e1", (FA_READ | FA_OPEN_EXISTING));
        std::string tkey = "";
        std::string orid = "";
        std::string fappid = horizon::FormatApplicationId(ApplicationId);
        std::string outdir = "sdmc:/goldleaf/dump/" + fappid;
        u32 tmpsz = 0;
        while(true)
        {
            if(tkey != "") break;
            u8 *tkdata = (u8*)malloc(0x40000);
            fatfs::FRESULT fr = f_read(&save, tkdata, 0x40000, &tmpsz);
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
            free(tkdata);
        }
        if(ExportData && (tkey != ""))
        {
            FILE *ceout = fopen((outdir + "/" + orid + ".cert").c_str(), "wb");
            fwrite(es::CertData, 1792, 1, ceout);
            fclose(ceout);
        }
        return tkey;
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

    bool HasTitleKeyCrypto(std::string NCAPath)
    {
        int outfd = dup(STDOUT_FILENO);
        int errfd = dup(STDERR_FILENO);
        freopen("sdmc:/goldleaf/dump/temp/hactool_stdout.log", "w", stdout);
        freopen("sdmc:/goldleaf/dump/temp/hactool_stderr.log", "w", stderr);
        hactool::ProcessResult pr = hactool::Process(NCAPath, hactool::Extraction::MakeExeFs("sdmc:/goldleaf/dump/temp/tfs"), hactool::ExtractionFormat::NCA, GetKeyFilePath());
        fclose(stdout);
        fclose(stderr);
        dup2(outfd, STDOUT_FILENO);
        dup2(errfd, STDERR_FILENO);
        stdout = fdopen(STDOUT_FILENO, "w");
        stderr = fdopen(STDERR_FILENO, "w");
        close(outfd);
        close(errfd);
        if(!pr.Ok) return true;
        bool ex = fs::IsDirectory("sdmc:/goldleaf/dump/temp/tfs");
        if(ex) fs::DeleteDirectory("sdmc:/goldleaf/dump/temp/tfs");
        return !ex;
    }
}