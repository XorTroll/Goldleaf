#pragma once

#include "install/game_container.hpp"

namespace tin::install::nsp
{
    class ExtractedDirSession
    {
        public:
            FsDir m_dir;

            ExtractedDirSession();
            ~ExtractedDirSession();

            Result OpenDir(FsFileSystem& fileSystem, std::string path);
    };

    class ExtractedNSPContainer final : public IGameContainer
    {
        private:
            FsFileSystem m_fileSystem;
            std::string m_baseDirPath;

        public:
            ExtractedNSPContainer();
            ~ExtractedNSPContainer();

            Result OpenContainer(std::string path) override;
            void CloseContainer() override;

            Result ReadFile(std::string name, u8* buff, size_t size, size_t offset) override;
            Result GetFileSize(std::string name, size_t* sizeOut) override;

            bool HasFile(std::string name) override;
            std::string FindFile(std::function<bool (std::string)>& comparator) override;
    };
}