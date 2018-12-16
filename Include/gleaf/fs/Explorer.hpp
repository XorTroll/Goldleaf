/*

    Goldleaf - Nintendo Switch title manager homebrew (open-source)

    Copyright 2018 - Goldleaf project, developed and managed by XorTroll
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
            Explorer(FsFileSystem IFS);
            ~Explorer();
            bool NavigateBack();
            bool NavigateForward(std::string Path);
            std::vector<std::string> GetDirectories();
            std::vector<std::string> GetFiles();
            std::vector<std::string> GetContents();
            std::string GetCwd();
            Partition GetPartition();
            void MovePartition(Partition NewBase);
            std::string FullPathFor(std::string Path);
            u64 GetTotalSpaceForPath(std::string Path);
            u64 GetFreeSpaceForPath(std::string Path);
            u64 GetTotalSpace();
            u64 GetFreeSpace();
            void Close();
        private:
            bool customifs;
            Partition part;
            FsFileSystem ifs;
            std::string ecwd;
    };
}