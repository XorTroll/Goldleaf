/*

    Goldleaf - Nintendo Switch title manager homebrew (open-source)

    Copyright 2018 - Goldleaf project, developed and managed by XorTroll
    This project is under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

*/

#include <switch.h>
#include <string>
#include <vector>

namespace gleaf::fs
{
    enum class Partition
    {
        NANDSafe,
        NANDSystem,
        NANDUser,
        SdCard,
    };

    class Explorer
    {
        public:
            Explorer(Partition Base);
            ~Explorer();
            bool NavigateBack();
            bool NavigateForward(std::string Path);
            std::vector<std::string> GetDirectories();
            std::vector<std::string> GetFiles();
            std::vector<std::string> GetContents();
            std::string GetCwd();
            void MovePartition(Partition NewBase);
            std::string FullPathFor(std::string Path);
            void Close();
        private:
            Partition part;
            FsFileSystem ifs;
            std::string ecwd;
    };
}