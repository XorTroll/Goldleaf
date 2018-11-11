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
            const std::string m_absoluteRootPath;

            SimpleFileSystem(nx::fs::IFileSystem& fileSystem, std::string rootPath, std::string absoluteRootPath);
            ~SimpleFileSystem();

            nx::fs::IFile OpenFile(std::string path);
            bool HasFile(std::string path);
            std::string GetFileNameFromExtension(std::string path, std::string extension);
    };
}