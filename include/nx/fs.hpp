#pragma once

#include <string>

extern "C"
{
#include <switch/types.h>
#include <switch/services/fs.h>
}

#include "nx/ipc/tin_ipc.h"

namespace nx::fs
{
    class IFileSystem;

    class IFile
    {
        friend IFileSystem;

        private:
            FsFile m_file;

            IFile(FsFile& file);

        public:
            // Don't allow copying, or garbage may be closed by the destructor
            IFile& operator=(const IFile&) = delete;
            IFile(const IFile&) = delete;   

            ~IFile();

            void Read(u64 offset, void* buf, size_t size);
            u64 GetSize();
    };

    class IDirectory
    {
        friend IFileSystem;

        private:
            FsDir m_dir;

            IDirectory(FsDir& dir);

        public:
            // Don't allow copying, or garbage may be closed by the destructor
            IDirectory& operator=(const IDirectory&) = delete;
            IDirectory(const IDirectory&) = delete;   

            ~IDirectory();

            void Read(u64 inval, FsDirectoryEntry* buf, size_t numEntries);
            u64 GetEntryCount();
    };

    class IFileSystem
    {
        private:
            FsFileSystem m_fileSystem;

        public:
            // Don't allow copying, or garbage may be closed by the destructor
            IFileSystem& operator=(const IFileSystem&) = delete;
            IFileSystem(const IFileSystem&) = delete;   

            IFileSystem();
            ~IFileSystem();

            Result OpenSdFileSystem();
            void OpenFileSystemWithId(std::string path, FsFileSystemType fileSystemType, u64 titleId);
            void CloseFileSystem();
             
            IFile OpenFile(std::string path);
            IDirectory OpenDirectory(std::string path, int flags);
    };
}