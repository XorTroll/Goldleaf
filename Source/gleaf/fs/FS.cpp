#include <gleaf/fs/FS.hpp>

namespace gleaf::fs
{
    bool Exists(std::string Path)
    {
        std::ifstream ifs(Path);
        bool ex = ifs.good();
        ifs.close();
        return ex;
    }

    void CreateFile(std::string Path)
    {
        std::ofstream ofs(Path);
        ofs.close();
    }

    void CreateDirectory(std::string Path)
    {
        ::mkdir(Path.c_str(), 777);
    }

    std::string SearchForFile(FsFileSystem FS, std::string Path, std::string Extension, std::string Root)
    {
        FsDir sddir;
        fsFsOpenDirectory(&FS, (Root + "/").c_str(), (FS_DIROPEN_FILE | FS_DIROPEN_DIRECTORY), &sddir);
        u64 count = 0;
        fsDirGetEntryCount(&sddir, &count);
        auto entries = std::make_unique<FsDirectoryEntry[]>(count);
        size_t eread;
        fsDirRead(&sddir, 0, &eread, count, entries.get());
        if(count > 0) for(u32 i = 0; i < count; i++)
        {
            FsDirectoryEntry ent = entries[i];
            std::string entname = ent.name;
            if(ent.type == ENTRYTYPE_DIR)
            {
                auto subdir = Path + entname + "/";
                auto dfound = SearchForFile(FS, subdir, Extension);
                if(dfound != "") return dfound;
                continue;
            }
            else if(ent.type == ENTRYTYPE_FILE)
            {
                auto ffound = entname.substr(entname.find(".") + 1);
                if(ffound == Extension) return entname;
            }
        }
        return "";
    }
}