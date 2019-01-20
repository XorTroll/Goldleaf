
/*

    Goldleaf - Nintendo Switch homebrew multitool, for several purposes and with several features

    Copyright 2018 - 2019 Goldleaf project, developed by XorTroll
    This project is under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

*/

#pragma once
#include <gleaf/nsp/Installer.hpp>
#include <Commands.hpp>
#include <UsbReader.hpp>

namespace gleaf::usb
{
    class Installer
    {
        public:
            typedef std::function<void(std::string name, u32 index, u32 count, int percentage, double speed)> ProgressReport;

            Installer(UsbReader reader, std::string file, Destination location, bool ignoreVersion);
            InstallerResult ProcessRecords();
            InstallerResult ProcessContent(u32 Index, std::function<void(std::string Name, u32 Index, u32 Count, int Percentage, double Speed)> Callback);
            InstallerResult ProcessContents(std::function<void(std::string Name, u32 Index, u32 Count, int Percentage, double Speed)> Callback);
            InstallerResult GetLatestResult();
            void Finish();
        private:
            NSPContentData ProcessCnmt();
            void ProcessNca(NSPContentData content, std::string name, std::string ext);
            InstallerResult ReadFcmnt(NcmNcaId cnmtid, ByteBuffer* output);
            InstallerResult WriteMetaDatabase(NcmNcaId cnmtid, NSPContentData cnmtData, ByteBuffer fcnmt, NcmMetaRecord* result);
            InstallerResult RegisterContentMeta(u64 basetid, std::vector<ns::ContentStorageRecord>* result);
            InstallerResult PushRecords(NcmMetaRecord metakey, u64 basetid, std::vector<ns::ContentStorageRecord> records);

            UsbReader usbReader;
            std::string filename;
            std::vector<NSPContentData> contentData;
            FsStorageId storageId;
            InstallerResult installerResult;
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