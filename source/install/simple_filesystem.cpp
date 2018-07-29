#include "install/simple_filesystem.hpp"

#include <exception>
#include <memory>
#include "nx/fs.hpp"
#include "error.hpp"

namespace tin::install::nsp
{
    SimpleFileSystem::SimpleFileSystem(nx::fs::IFileSystem& fileSystem, std::string rootPath, std::string absoluteRootPath) :
        m_fileSystem(&fileSystem) , m_rootPath(rootPath), m_absoluteRootPath(absoluteRootPath)
    {}

    SimpleFileSystem::~SimpleFileSystem() {}

    nx::fs::IFile SimpleFileSystem::OpenFile(std::string path)
    {
        return m_fileSystem->OpenFile(m_rootPath + path);
    }

    bool SimpleFileSystem::HasFile(std::string path)
    {
        try
        {
            fprintf(nxlinkout, ("Attempting to find file at " + m_rootPath + path + "\n").c_str());
            m_fileSystem->OpenFile(m_rootPath + path);
            return true;
        }
        catch (std::exception& e) {}
        return false;
    }

    std::string SimpleFileSystem::GetFileNameFromExtension(std::string path, std::string extension)
    {
        Result rc = 0;
        nx::fs::IDirectory dir;

        if (R_FAILED(rc = m_fileSystem->OpenDirectory(m_rootPath + path, FS_DIROPEN_FILE | FS_DIROPEN_DIRECTORY, dir)))
        {
            fprintf(nxlinkout, "%s:%u: %s.  Error code: 0x%08x\n", __func__, __LINE__, "Failed to open dir", rc);
            return "";
        }

        size_t numEntriesRead;
        auto dirEntries = std::make_unique<FsDirectoryEntry[]>(256);

        if (R_FAILED(rc = dir.Read(0, dirEntries.get(), 256, &numEntriesRead)))
        {
            fprintf(nxlinkout, "%s:%u: %s.  Error code: 0x%08x\n", __func__, __LINE__, "Failed to read dir", rc);
            return "";
        }

        for (unsigned int i = 0; i < numEntriesRead; i++)
        {
            FsDirectoryEntry dirEntry = dirEntries[i];
            std::string dirEntryName = dirEntry.name;

            if (dirEntry.type == ENTRYTYPE_DIR)
            {
                auto subdirPath = path + dirEntryName + "/";
                auto subdirFound = this->GetFileNameFromExtension(subdirPath, extension);

                if (subdirFound != "")
                    return subdirFound;
                continue;
            }
            else if (dirEntry.type == ENTRYTYPE_FILE)
            {
                auto foundExtension = dirEntryName.substr(dirEntryName.find(".") + 1); 

                if (foundExtension == extension)
                    return dirEntryName;
            }
        }

        return "";
    }
}