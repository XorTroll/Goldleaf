
/*

    Goldleaf - Nintendo Switch title manager homebrew

    Copyright 2018 - Goldleaf project, developed by XorTroll
    This project is under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

*/

#pragma once
#include <usb/Commands.hpp>

namespace gleaf::usb
{
    class Installer
    {
        public:
            Installer(Destination Location, bool IgnoreVersion);
            InstallerResult ProcessRecords();
            InstallerResult ProcessContent(u32 Index, std::function<void(std::string Name, u32 Index, u32 Count, int Percentage, double Speed)> Callback);
            InstallerResult ProcessContents(std::function<void(std::string Name, u32 Index, u32 Count, int Percentage, double Speed)> Callback);
            InstallerResult GetLatestResult();
            void Finish();
        private:
            std::vector<NSPContentData> cnts;
            FsStorageId stid;
            InstallerResult irc;
            ByteBuffer cnmtbuf;
            bool iver;
            bool gtik;
            bool gcert;
            bool itik;
            std::unique_ptr<u8[]> btik;
            std::unique_ptr<u8[]> bcert;
            size_t stik;
            size_t scert;
    };
}