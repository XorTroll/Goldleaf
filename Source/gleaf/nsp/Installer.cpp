#include <gleaf/nsp/Installer.hpp>

namespace gleaf::nsp
{
    Installer::Installer(Destination Location, std::string Input, bool IgnoreVersion)
    {
        stid = FsStorageId_SdCard;
        input = Input;
        Input.reserve(FS_MAX_PATH);
        Result rc = fsOpenFileSystemWithId(&idfs, 0, FsFileSystemType_ApplicationPackage, Input.c_str());
        if(rc == 0x236e02) ThrowError("Failed to read / open the NSP file.");
        else if(rc != 0) ThrowError("Failed to open file system via NSP file: " + Input);
        ByteBuffer cnmt;
        std::string cnmtname = fs::SearchForFile(idfs, "", "cnmt.nca");
        if(cnmtname == "") ThrowError("Error trying to find CNMT NCA.");
        FsFile cnmtfile;
        cnmtname.reserve(FS_MAX_PATH);
        fsFsOpenFile(&idfs, cnmtname.c_str(), FS_OPEN_READ, &cnmtfile);
        u64 cnmtsize = 0;
        fsFileGetSize(&cnmtfile, &cnmtsize);
        std::string cnmtabs = Input + "/" + cnmtname;
        ncm::ContentRecord record;
        fsFileClose(&cnmtfile);
        record.NCAId = horizon::GetNCAIdFromString(cnmtname);
        *(u64*)record.Size = (cnmtsize & 0xFFFFFFFFFFFF);
        record.Type = ncm::ContentType::Meta;
        FsFileSystem cnmtfs;
        cnmtabs.reserve(FS_MAX_PATH);
        rc = fsOpenFileSystemWithId(&cnmtfs, 0, FsFileSystemType_ContentMeta, cnmtabs.c_str());
        if(rc != 0) ThrowError("Error opening CNMT NCA file system.");
        std::string fcnmtname = fs::SearchForFile(cnmtfs, "", "cnmt");
        if(fcnmtname == "") ThrowError("CNMT file not found.");
        FsFile fcnmtfile;
        fcnmtname.reserve(FS_MAX_PATH);
        rc = fsFsOpenFile(&cnmtfs, ("/" + fcnmtname).c_str(), FS_OPEN_READ, &fcnmtfile);
        if(rc != 0) ThrowError("Error opening CNMT file.");
        u64 fcnmtsize = 0;
        rc = fsFileGetSize(&fcnmtfile, &fcnmtsize);
        if(rc != 0) ThrowError("Error getting size of CNMT file.");
        ByteBuffer fcnmt;
        fcnmt.Resize(fcnmtsize);
        size_t rout = 0;
        fsFileRead(&fcnmtfile, 0, fcnmt.GetData(), fcnmt.GetSize(), &rout);
        cmeta = ncm::ContentMeta(fcnmt.GetData(), fcnmt.GetSize());
        ncm::ContentStorage cnmts(stid);
        if(!cnmts.Has(record.NCAId)) ncas.push_back(record);
        else { /* CNMT already installed */ }
        cmeta.GetInstallContentMeta(cnmtbuf, record, IgnoreVersion);
        for(auto &record : cmeta.GetContentRecords()) ncas.push_back(record);
        fsFileClose(&fcnmtfile);
        fsFsClose(&cnmtfs);
    }

    Installer::~Installer()
    {
        fsFsClose(&idfs);
    }

    void Installer::InitializeRecords()
    {
        Result rc = 0;
        NcmContentMetaDatabase metadb;
        NcmMetaRecord metakey = cmeta.GetContentMetaKey();
        rc = ncmOpenContentMetaDatabase(stid, &metadb);
        if(rc != 0)
        {
            ThrowError("Error opening meta database.");
            serviceClose(&metadb.s);
        }
        rc = ncmContentMetaDatabaseSet(&metadb, &metakey, cnmtbuf.GetSize(), (NcmContentMetaRecordsHeader*)cnmtbuf.GetData());
        if(rc != 0)
        {
            ThrowError("Error setting content records");
            serviceClose(&metadb.s);
        }
        rc = ncmContentMetaDatabaseCommit(&metadb);
        if(rc != 0)
        {
            ThrowError("Error committing content records.");
            serviceClose(&metadb.s);
        }
        std::vector<ns::ContentStorageRecord> records;
        basetid = horizon::GetBaseApplicationId(metakey.titleId, static_cast<ncm::ContentMetaType>(metakey.type));
        std::tuple<Result, u32> nst = ns::CountApplicationContentMeta(basetid);
        rc = std::get<0>(nst);
        u32 cmetacount = std::get<1>(nst);
        if((rc != 0) && (rc != 0x410)) ThrowError("Error counting application content meta.");
        if(cmetacount > 0)
        {
            records.resize(cmetacount);
            size_t csbufs = (cmetacount * sizeof(ns::ContentStorageRecord));
            ns::ContentStorageRecord *csbuf = (ns::ContentStorageRecord*)malloc(csbufs);
            std::tuple<Result, u32, void*> listt = ns::ListApplicationRecordContentMeta(0, basetid, csbufs);
            rc = std::get<0>(listt);
            u32 entread = std::get<1>(listt);
            csbuf = (ns::ContentStorageRecord*)std::get<2>(listt);
            if(rc != 0) ThrowError("Failed to list application record content meta.");
            if(entread != cmetacount) ThrowError("Mismatch between application record content meta counts.");
            memcpy(records.data(), csbuf, csbufs);
        }
        ns::ContentStorageRecord csrecord;
        csrecord.Record = metakey;
        csrecord.StorageId = stid;
        records.push_back(csrecord);
        rc = ns::DeleteApplicationRecord(basetid);
        rc = ns::PushApplicationRecord(basetid, 0x3, records.data(), records.size() * sizeof(ns::ContentStorageRecord));
        if(rc != 0) ThrowError("Error pushing application records.");
        std::string tikname = fs::SearchForFile(idfs, "", "tik");
        FsFile tikfile;
        tikname.reserve(FS_MAX_PATH);
        rc = fsFsOpenFile(&idfs, tikname.c_str(), FS_OPEN_READ, &tikfile);
        if(rc == 0)
        {
            u64 tiksize = 0;
            rc = fsFileGetSize(&tikfile, &tiksize);
            if(rc != 0)
            {
                fsFileClose(&tikfile);
                return;
            }
            auto tik = std::make_unique<u8[]>(tiksize);
            size_t otiksize = 0;
            rc = fsFileRead(&tikfile, 0, tik.get(), tiksize, &otiksize);
            if(rc != 0)
            {
                fsFileClose(&tikfile);
                return;
            }
            std::string certname = fs::SearchForFile(idfs, "", "cert");
            FsFile certfile;
            certname.reserve(FS_MAX_PATH);
            rc = fsFsOpenFile(&idfs, certname.c_str(), FS_OPEN_READ, &certfile);
            if(rc == 0)
            {
                u64 certsize = 0;
                rc = fsFileGetSize(&certfile, &certsize);
                if(rc != 0)
                {
                    fsFileClose(&certfile);
                    return;
                }
                auto cert = std::make_unique<u8[]>(certsize);
                size_t ocertsize = 0;
                rc = fsFileRead(&certfile, 0, cert.get(), certsize, &ocertsize);
                if(rc != 0)
                {
                    fsFileClose(&certfile);
                    return;
                }
                rc = es::ImportTicket(tik.get(), tiksize, cert.get(), certsize);
            }
        }
    }

    void Installer::InstallNCAs(std::function<void(ncm::ContentRecord, u32, u32, int)> OnChunk)
    {
        if(!ncas.empty()) for(u32 i = 0; i < ncas.size(); i++)
        {
            NcmNcaId curid = ncas[i].NCAId;
            std::string ncaname = horizon::GetStringFromNCAId(curid);
            bool nca = false;
            bool cnmt = false;
            FsFile ncafile;
            std::string fncaname = "/" + ncaname + ".nca";
            fncaname.reserve(FS_MAX_PATH);
            Result rc = fsFsOpenFile(&idfs, fncaname.c_str(), FS_OPEN_READ, &ncafile);
            if(rc == 0)
            {
                nca = true;
                ncaname += ".nca";
            }
            else
            {
                fncaname = "/" + ncaname + ".cnmt.nca";
                fncaname.reserve(FS_MAX_PATH);
                rc = fsFsOpenFile(&idfs, fncaname.c_str(), FS_OPEN_READ, &ncafile);
                if(rc == 0)
                {
                    ncaname += ".cnmt.nca";
                    cnmt = true;
                }
            }
            if(!nca) if(!cnmt) ThrowError("NCA file not found, or error opening it. NCA Id: " + ncaname);
            ncm::ContentStorage cstorage(stid);
            cstorage.DeletePlaceHolder(curid);
            u64 ncasize = 0;
            rc = fsFileGetSize(&ncafile, &ncasize);
            if(rc != 0) ThrowError("Error getting NCA file size.");
            u64 noff = 0;
            size_t reads = 0x100000;
            auto readbuf = std::make_unique<u8[]>(reads);
            if(readbuf == NULL) ThrowError("Error allocating memory.");
            cstorage.CreatePlaceHolder(curid, curid, ncasize);
            float progress = 0.0f;
            while(noff < ncasize)
            {
                progress = (float)noff / (float)ncasize;
                OnChunk(ncas[i], i, ncas.size(), (int)(progress * 100.0));
                if((noff + reads) >= ncasize) reads = (ncasize - noff);
                size_t rout = 0;
                fsFileRead(&ncafile, noff, readbuf.get(), reads, &rout);
                cstorage.WritePlaceHolder(curid, noff, readbuf.get(), reads);
                noff += reads;
            }
            cstorage.Register(curid, curid);
            cstorage.DeletePlaceHolder(curid);
            fsFileClose(&ncafile);
        }
    }

    void Installer::Finish()
    {
        fsFsClose(&idfs);
    }

    u64 Installer::GetApplicationId()
    {
        return basetid;
    }

    ncm::ContentMetaType Installer::GetContentType()
    {
        return static_cast<ncm::ContentMetaType>(cmeta.GetContentMetaKey().type);
    }

    std::vector<ncm::ContentRecord> Installer::GetNCAList()
    {
        return ncas;
    }
}