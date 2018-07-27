#include "install/simple_filesystem.hpp"

#include <memory>
#include "nx/fs.hpp"
#include "error.hpp"

namespace tin::install::nsp
{
    SimpleFileSystem::SimpleFileSystem(nx::fs::IFileSystem& fileSystem, std::string rootPath) :
        m_fileSystem(&fileSystem) , m_rootPath(rootPath)
    {}

    SimpleFileSystem::~SimpleFileSystem() {}
    
    Result SimpleFileSystem::ReadFile(std::string path, u8* buff, size_t size, size_t offset)
    {
        nx::fs::IFile file;
        PROPAGATE_RESULT(m_fileSystem->OpenFile(m_rootPath + path, file), "Failed to open file");

        size_t actualReadSize = 0;

        PROPAGATE_RESULT(file.Read(offset, buff, size, &actualReadSize), "Failed to read file");

        if (actualReadSize != size)
        {
            printf("readExtractedInstallFile: Size read 0x%lx doesn't match expected size 0x%lx", actualReadSize, size);
            return -1;
        }

        return 0;
    }

    Result SimpleFileSystem::GetFileSize(std::string path, size_t* sizeOut)
    {
        nx::fs::IFile file;
        PROPAGATE_RESULT(m_fileSystem->OpenFile(m_rootPath + path, file), "Failed to open file");
        PROPAGATE_RESULT(file.GetSize(sizeOut), "Failed to get file size");
        return 0;
    }

    bool SimpleFileSystem::HasFile(std::string path)
    {
        nx::fs::IFile file;
        if (R_FAILED(m_fileSystem->OpenFile(m_rootPath + path, file)))
            return false;
        return true;
    }

    std::string SimpleFileSystem::FindFilePath(std::string path, std::function<bool (FsDirectoryEntry&)>& comparator)
    {
        Result rc = 0;
        nx::fs::IDirectory dir;

        if (R_FAILED(rc = m_fileSystem->OpenDirectory(m_rootPath + path, FS_DIROPEN_FILE | FS_DIROPEN_DIRECTORY, dir)))
        {
            fprintf(nxlinkout, "%s:%u: %s.  Error code: 0x%08x\n", __func__, __LINE__, "Failed to open dir", rc);
            return "";
        }

        size_t numEntriesRead;
        FsDirectoryEntry dirEntry;

        while (R_SUCCEEDED(rc = dir.Read(0, &dirEntry, 1, &numEntriesRead)) && numEntriesRead == 1)
        {
            // NOTE: Directories are unsupported, however there shouldn't be any in NSPs/Extracted NSPS anyway
            if (dirEntry.type == ENTRYTYPE_FILE)
            {
                if (comparator(dirEntry))
                    return dirEntry.name;
            }
        }

        fprintf(nxlinkout, "%s:%u: %s.  Error code: 0x%08x\n", __func__, __LINE__, "Failed to find file", rc);
        return "";
    }
}