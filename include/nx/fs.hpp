#pragma once

#include <string>
#include <switch.h>

namespace nx::fs
{
    class IFile
    {
        private:
            FsFileSystem *m_fileSystem;
            FsFile m_file;

        public:
            IFile();
            ~IFile();

            Result OpenFile(FsFileSystem& fileSystem, std::string path);
            Result Read(u64 offset, void* buf, size_t size, size_t* sizeReadOut);
            Result GetSize(u64* sizeOut);
    };
}