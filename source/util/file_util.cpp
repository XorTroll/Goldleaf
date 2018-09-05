#include "util/file_util.hpp"

#include <memory>
#include "nx/fs.hpp"

namespace tin::util
{
    std::vector<std::string> GetNSPList()
    {
        std::vector<std::string> nspList;
        nx::fs::IFileSystem fileSystem;
        fileSystem.OpenSdFileSystem();
        nx::fs::IDirectory dir = fileSystem.OpenDirectory("/tinfoil/nsp/", FS_DIROPEN_FILE);

        u64 entryCount = dir.GetEntryCount();

        auto dirEntries = std::make_unique<FsDirectoryEntry[]>(entryCount);

        dir.Read(0, dirEntries.get(), entryCount);

        for (unsigned int i = 0; i < entryCount; i++)
        {
            FsDirectoryEntry dirEntry = dirEntries[i];
            std::string dirEntryName(dirEntry.name);
            std::string ext = ".nsp";

            if (dirEntry.type != ENTRYTYPE_FILE || dirEntryName.compare(dirEntryName.size() - ext.size(), ext.size(), ext) != 0)
                continue;

            nspList.push_back(dirEntry.name);
        }

        return nspList;
    }
}