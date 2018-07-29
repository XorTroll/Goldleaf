#include "nx/fs.hpp"

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
    IDirectory::IDirectory() {}
    IDirectory::~IDirectory()
    {
        fsDirClose(&m_dir);
    }

    Result IDirectory::Read(u64 inval, FsDirectoryEntry* buf, size_t maxEntries, size_t* entriesRead)
    {
        ASSERT_OK(fsDirRead(&m_dir, inval, entriesRead, maxEntries, buf), "Failed to read directory");
        return 0;
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

    Result IFileSystem::OpenDirectory(std::string path, int flags, IDirectory& dir)
    {
        ASSERT_OK(fsFsOpenDirectory(&m_fileSystem, path.c_str(), flags, &dir.m_dir), ("Failed to open directory " + path).c_str());
        return 0;
    }
}