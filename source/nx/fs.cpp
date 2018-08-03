#include "nx/fs.hpp"

#include <cstring>
#include "nx/ipc/tin_ipc.h"
#include "error.hpp"

namespace nx::fs
{
    // IFile

    IFile::IFile(FsFile& file)
    {
        m_file = file;
    }

    IFile::~IFile()
    {
        fsFileClose(&m_file);
    }

    void IFile::Read(u64 offset, void* buf, size_t size)
    {
        u64 sizeRead;
        ASSERT_OK(fsFileRead(&m_file, offset, buf, size, &sizeRead), "Failed to read file");
        
        if (sizeRead != size)
        {
            std::string msg = "Size read " + std::string("" + sizeRead) + " doesn't match expected size " + std::string("" + size);
            throw std::runtime_error(msg.c_str());
        }
    }

    u64 IFile::GetSize()
    {
        u64 sizeOut;
        ASSERT_OK(fsFileGetSize(&m_file, &sizeOut), "Failed to get file size");
        return sizeOut;
    }

    // End IFile

    // IDirectory
    IDirectory::IDirectory(FsDir& dir) 
    {
        m_dir = dir;
    }

    IDirectory::~IDirectory()
    {
        fsDirClose(&m_dir);
    }

    void IDirectory::Read(u64 inval, FsDirectoryEntry* buf, size_t numEntries)
    {
        size_t entriesRead;
        ASSERT_OK(fsDirRead(&m_dir, inval, &entriesRead, numEntries, buf), "Failed to read directory");

        /*if (entriesRead != numEntries)
        {
            std::string msg = "Entries read " + std::string("" + entriesRead) + " doesn't match expected number " + std::string("" + numEntries);
            throw std::runtime_error(msg);
        }*/
    }

    u64 IDirectory::GetEntryCount()
    {
        u64 entryCount = 0;
        ASSERT_OK(fsDirGetEntryCount(&m_dir, &entryCount), "Failed to get entry count");
        return entryCount;
    }

    // End IDirectory

    IFileSystem::IFileSystem() {}

    IFileSystem::~IFileSystem()
    {
        this->CloseFileSystem();
    }

    Result IFileSystem::OpenSdFileSystem()
    {
        ASSERT_OK(fsMountSdcard(&m_fileSystem), "Failed to mount sd card");
        return 0;
    }

    Result IFileSystem::OpenFileSystemWithId(std::string path, FsFileSystemType fileSystemType, u64 titleId)
    {
        ASSERT_OK(fsOpenFileSystemWithId(&m_fileSystem, titleId, fileSystemType, path.c_str()), "Failed to open file system with id");
        return 0;  
    }

    void IFileSystem::CloseFileSystem()
    {
        fsFsClose(&m_fileSystem);
    }

    IFile IFileSystem::OpenFile(std::string path)
    {
        FsFile file;
        ASSERT_OK(fsFsOpenFile(&m_fileSystem, path.c_str(), FS_OPEN_READ, &file), ("Failed to open file " + path).c_str());
        return IFile(file);
    }

    IDirectory IFileSystem::OpenDirectory(std::string path, int flags)
    {
        FsDir dir;
        memset(&dir, 0, sizeof(FsDir));
        LOG_DEBUG(("Attempting to open directory " + path + "\n").c_str());
        ASSERT_OK(fsFsOpenDirectory(&m_fileSystem, path.c_str(), flags, &dir), ("Failed to open directory " + path).c_str());
        LOG_DEBUG(("Opened directory " + path + "\n").c_str());
        return IDirectory(dir);
    }
}