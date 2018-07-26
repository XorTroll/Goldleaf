#pragma once

#include <vector>
#include <switch.h>
#include "install/game_container.hpp"

namespace tin::install::nsp
{
    class NSPEnclosedFile final
    {
        public:
            const std::string m_name;
            const size_t m_absoluteNSPFileOffset;
            const size_t m_fileSize;

            NSPEnclosedFile(std::string name, size_t absoluteNSPFileOffset, size_t fileSize);
    };

    class NSPContainer final : public IGameContainer
    {
        private:
            FsFileSystem m_fileSystem;
            FsFile m_fsFile;

            std::vector<NSPEnclosedFile> m_enclosedFiles;

        public:
            NSPContainer();
            ~NSPContainer();

            Result OpenContainer(std::string path) override;
            void CloseContainer() override;

            Result ReadFile(std::string name, u8* buff, size_t size, size_t offset) override;
            Result GetFileSize(std::string name, size_t* sizeOut) override;

            bool HasFile(std::string name) override;
            std::string GetFileNameFromExtension(std::string extension) override;

        private:
            Result ReadNSPHeader();
            Result GetEnclosedFile(NSPEnclosedFile **out, std::string name);
    };
}