
/*

    Goldleaf - Nintendo Switch homebrew multitool, for several purposes and with several features

    Copyright 2018 - 2019 Goldleaf project, developed by XorTroll
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
            bool itik;
            std::unique_ptr<u8[]> btik;
            size_t stik;
    };
}