#pragma once

#include <string>
#include <switch.h>
#include "nx/ipc/tin_ipc.h"

namespace nx::fs
{
    class IFileSystem;

    class IFile
    {
        friend IFileSystem;

        private:
            FsFile m_file;

        public:
            IFile();
            ~IFile();

            Result Read(u64 offset, void* buf, size_t size, size_t* sizeReadOut);
            Result GetSize(u64* sizeOut);
    };

    class IDirectory
    {
        friend IFileSystem;

        private:
            FsDir m_dir;

        public:
            IDirectory();
            ~IDirectory();

            Result Read(u64 inval, FsDirectoryEntry* buf, size_t maxEntries, size_t* entriesRead);
    };

    class IFileSystem
    {
        private:
            FsFileSystem m_fileSystem;

        public:
            IFileSystem();
            ~IFileSystem();

            Result OpenSdFileSystem();
            Result OpenFileSystemWithId(std::string path, FsFileSystemType fileSystemType, u64 titleId);
            void CloseFileSystem();
             
            Result OpenFile(std::string path, IFile& file);
            Result OpenDirectory(std::string path, int flags, IDirectory& dir);
    };
}