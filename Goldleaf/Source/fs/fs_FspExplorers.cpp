
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2019  XorTroll

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

#include <fs/fs_FspExplorers.hpp>

namespace fs
{
    SdCardExplorer::SdCardExplorer() : StdExplorer()
    {
        this->SetNames("sdmc", "SdCard");
    }

    u64 SdCardExplorer::GetTotalSpace()
    {
        s64 sz = 0;
        fsFsGetTotalSpace(fsdevGetDeviceFileSystem("sdmc"), "/", &sz);
        return (u64)sz;
    }

    u64 SdCardExplorer::GetFreeSpace()
    {
        s64 sz = 0;
        fsFsGetFreeSpace(fsdevGetDeviceFileSystem("sdmc"), "/", &sz);
        return (u64)sz;
    }

    Service fspusb;

    NANDExplorer::NANDExplorer(Partition Part) : StdExplorer()
    {
        this->part = Part;
        switch(Part)
        {
            case Partition::PRODINFOF:
            {
                fsOpenBisFileSystem(&this->fs, FsBisPartitionId_CalibrationFile, "");
                fsdevMountDevice("gprodinfof", this->fs);
                this->SetNames("gprodinfof", "ProdInfoF");
                break;
            }
            case Partition::NANDSafe:
            {
                fsOpenBisFileSystem(&this->fs, FsBisPartitionId_SafeMode, "");
                fsdevMountDevice("gnsafe", this->fs);
                this->SetNames("gnsafe", "Safe");
                break;
            }
            case Partition::NANDUser:
            {
                fsOpenBisFileSystem(&this->fs, FsBisPartitionId_User, "");
                fsdevMountDevice("gnuser", this->fs);
                this->SetNames("gnuser", "User");
                break;
            }
            case Partition::NANDSystem:
            {
                fsOpenBisFileSystem(&this->fs, FsBisPartitionId_System, "");
                fsdevMountDevice("gnsystem", this->fs);
                this->SetNames("gnsystem", "System");
                break;
            }
            case Partition::SdCard:
            {
                smGetService(&fspusb, "fsp-usb");
                u32 in = 0;
                serviceDispatchIn(&fspusb, 3, in,
                    .out_num_objects = 1,
                    .out_objects = &this->fs.s,
                );
                fsdevMountDevice("gusbdrv", this->fs);
                this->SetNames("gusbdrv", "USB-0");
                break;
            }
            default:
                break;
        }
    }

    NANDExplorer::~NANDExplorer()
    {
        switch(this->part)
        {
            case Partition::PRODINFOF:
            {
                fsdevUnmountDevice("gprodinfof");
                break;
            }
            case Partition::NANDSafe:
            {
                fsdevUnmountDevice("gnsafe");
                break;
            }
            case Partition::NANDUser:
            {
                fsdevUnmountDevice("gnuser");
                break;
            }
            case Partition::NANDSystem:
            {
                fsdevUnmountDevice("gnsystem");
                break;
            }
            default:
                break;
        }
    }

    Partition NANDExplorer::GetPartition()
    {
        return this->part;
    }

    bool NANDExplorer::ShouldWarnOnWriteAccess()
    {
        return true;
    }

    u64 NANDExplorer::GetTotalSpace()
    {
        s64 sz = 0;
        fsFsGetTotalSpace(&this->fs, "/", &sz);
        return (u64)sz;
    }

    u64 NANDExplorer::GetFreeSpace()
    {
        s64 sz = 0;
        fsFsGetFreeSpace(&this->fs, "/", &sz);
        return (u64)sz;
    }

    FileSystemExplorer::FileSystemExplorer(String MountName, String DisplayName, FsFileSystem *FileSystem) : StdExplorer()
    {
        this->fs = FileSystem;
        this->SetNames(MountName, DisplayName);
        fsdevMountDevice(MountName.AsUTF8().c_str(), *this->fs);
    }

    FileSystemExplorer::~FileSystemExplorer()
    {
        fsdevUnmountDevice(this->mntname.AsUTF8().c_str());
    }

    FsFileSystem *FileSystemExplorer::GetFileSystem()
    {
        return this->fs;
    }

    u64 FileSystemExplorer::GetTotalSpace()
    {
        s64 sz = 0;
        fsFsGetTotalSpace(this->fs, "/", &sz);
        return (u64)sz;
    }

    u64 FileSystemExplorer::GetFreeSpace()
    {
        s64 sz = 0;
        fsFsGetFreeSpace(this->fs, "/", &sz);
        return (u64)sz;
    }
}