#include <gleaf/dump/Dump.hpp>
#include <gleaf/dump/DebugTypes.hpp>
#include <gleaf/Application.hpp>
#include <gleaf/hactool.hpp>
#include <gleaf/horizon.hpp>
#include <gleaf/es.hpp>
#include <sstream>
#include <iomanip>

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
        std::string tkey = "";
        std::string orid = "";
        std::string fappid = horizon::FormatApplicationId(ApplicationId);
        std::string outdir = "sdmc:/goldleaf/dump/" + fappid;
        FsFileSystem ess;
        Result rc = 0;
        do
        {
            rc = fsMount_SystemSaveData(&ess, 0x80000000000000e1);
        } while(rc != 0);
        fsdevMountDevice("escommon", ess);
        FILE *f = fopen("escommon:/ticket.bin", "rb");
        fseek(f, 0, SEEK_END);
        u64 fsize = ftell(f);
        rewind(f);
        u64 szrem = fsize;
        while(szrem)
        {
            u8 *ticket = (u8*)malloc(0x400 * sizeof(u8));
            fread(ticket, 1, 0x400, f);
            szrem -= 0x400;
            std::stringstream stid;
            std::stringstream srid;
            for(u32 i = 0; i < 0x10; i++)
            {
                u32 off = 0x2a0 + i;
                srid << std::setw(2) << std::setfill('0') << std::hex << (int)ticket[off];
            }
            for(u32 i = 0; i < 0x8; i++)
            {
                u32 off = 0x2a0 + i;
                stid << std::setw(2) << std::setfill('0') << std::uppercase << std::hex << (int)ticket[off];
            }
            std::stringstream stkey;
            for(u32 i = 0; i < 0x10; i++)
            {
                u32 off = 0x180 + i;
                stkey << std::setw(2) << std::setfill('0') << std::uppercase << std::hex << (int)ticket[off];
            }
            std::string tid = stid.str();
            std::string rid = srid.str();
            std::string etkey = stkey.str();
            // We found the ticket, return the tkey and export the ticket
            if(fappid == tid)
            {
                orid = rid;
                if(ExportData)
                {
                    FILE *tikf = fopen((outdir + "/" + rid + ".tik").c_str(), "wb");
                    fwrite(ticket, 1, 0x400, tikf);
                    fclose(tikf);
                }
                tkey = etkey;
                free(ticket);
                break;
            }
            free(ticket);
        }
        fclose(f);
        fsdevUnmountDevice("escommon");
        // If we found a tkey export the cert too!
        if(ExportData && (tkey != ""))
        {
            FsFileSystem css;
            do
            {
                rc = fsMount_SystemSaveData(&css, 0x80000000000000e0);
            } while(rc != 0);
            fsdevMountDevice("escert", css);
            FILE *ceout = fopen((outdir + "/" + orid + ".cert").c_str(), "wb");
            FILE *c1 = fopen("escert:/certificate/CA00000003", "rb");
            if(c1)
            {
                fseek(c1, 0, SEEK_END);
                u64 c1size = ftell(c1);
                rewind(c1);
                u8 *bc1 = (u8*)malloc(c1size * sizeof(u8));
                fread(bc1, 1, c1size, c1);
                fwrite(bc1, 1, c1size, ceout);
                free(bc1);
            }
            fclose(c1);
            FILE *c2 = fopen("escert:/certificate/XS00000020", "rb");
            if(c2)
            {
                fseek(c2, 0, SEEK_END);
                u64 c2size = ftell(c2);
                rewind(c2);
                u8 *bc2 = (u8*)malloc(c2size * sizeof(u8));
                fread(bc2, 1, c2size, c2);
                fwrite(bc2, 1, c2size, ceout);
                free(bc2);
            }
            fclose(c2);
            fclose(ceout);
            fsdevUnmountDevice("escert");
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