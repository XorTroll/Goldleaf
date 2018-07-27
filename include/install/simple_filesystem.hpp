#pragma once

#include <functional>
#include "nx/fs.hpp"

namespace tin::install::nsp
{
    class SimpleFileSystem final
    {
        private:
            nx::fs::IFileSystem* m_fileSystem;

        public:
            const std::string m_rootPath;

            SimpleFileSystem(nx::fs::IFileSystem& fileSystem, std::string rootPath);
            ~SimpleFileSystem();

            Result ReadFile(std::string path, u8* buff, size_t size, size_t offset);
            Result GetFileSize(std::string path, size_t* sizeOut);

            bool HasFile(std::string path);
            std::string FindFilePath(std::string path, std::function<bool (FsDirectoryEntry&)>& comparator);
    };
}