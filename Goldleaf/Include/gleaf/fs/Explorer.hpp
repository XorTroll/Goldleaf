
/*

    Goldleaf - Nintendo Switch homebrew multitool, for several purposes and with several features

    Copyright 2018 - 2019 Goldleaf project, developed by XorTroll
    This project is under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

*/

#include <vector>
#include <gleaf/fs/FS.hpp>

namespace gleaf::fs
{
    class Explorer
    {
        public:
            Explorer(Partition Base);
            Explorer(FsFileSystem IFS, std::string DisplayName);
            ~Explorer();
            bool NavigateBack();
            bool NavigateForward(std::string Path);
            std::vector<std::string> GetDirectories();
            std::vector<std::string> GetFiles();
            std::vector<std::string> GetContents();
            std::string GetCwd();
            std::string GetPresentableCwd();
            bool HasPartition();
            bool HasCustomFS();
            Partition GetPartition();
            void MovePartition(Partition NewBase);
            std::string FullPathFor(std::string Path);
            std::string FullPresentablePathFor(std::string Path);
            u64 GetTotalSpaceForPath(std::string Path);
            u64 GetFreeSpaceForPath(std::string Path);
            u64 GetTotalSpace();
            u64 GetFreeSpace();
            static std::string GenerateMountName();
            static void DeleteMountName(std::string GMount);
            void Close();
        private:
            bool customifs;
            Partition part;
            FsFileSystem ifs;
            std::string dspname;
            std::string mntname;
            std::string ecwd;
    };
}