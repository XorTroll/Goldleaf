#include "nx/fs.hpp"

#include "nx/ipc/tin_ipc.h"
#include "error.hpp"

namespace nx::fs
{
    // IFile

    IFile::IFile() {}

    IFile::~IFile()
    {
        fsFileClose(&m_file);
    }

    Result IFile::Read(u64 offset, void* buf, size_t size, size_t* sizeReadOut)
    {
        PROPAGATE_RESULT(fsFileRead(&m_file, offset, buf, size, sizeReadOut), "Failed to read file");
        return 0;
    }

    Result IFile::GetSize(u64* sizeOut)
    {
        PROPAGATE_RESULT(fsFileGetSize(&m_file, sizeOut), "Failed to get file size");
        return 0;
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
        PROPAGATE_RESULT(fsDirRead(&m_dir, inval, entriesRead, maxEntries, buf), "Failed to read directory");
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
        PROPAGATE_RESULT(fsMountSdcard(&m_fileSystem), "Failed to mount sd card");
        return 0;
    }

    Result IFileSystem::OpenFileSystemWithId(std::string path, FsFileSystemType fileSystemType, u64 titleId)
    {
        PROPAGATE_RESULT(fsOpenFileSystemWithId(path.c_str(), fileSystemType, titleId, &m_fileSystem), "Failed to open file system with id");
        return 0;  
    }

    void IFileSystem::CloseFileSystem()
    {
        fsFsClose(&m_fileSystem);
    }

    Result IFileSystem::OpenFile(std::string path, IFile& file)
    {
        PROPAGATE_RESULT(fsFsOpenFile(&m_fileSystem, path.c_str(), FS_OPEN_READ, &file.m_file), ("Failed to open file " + path).c_str());
        return 0;
    }

    Result IFileSystem::OpenDirectory(std::string path, int flags, IDirectory& dir)
    {
        PROPAGATE_RESULT(fsFsOpenDirectory(&m_fileSystem, path.c_str(), flags, &dir.m_dir), ("Failed to open directory " + path).c_str());
        return 0;
    }
}