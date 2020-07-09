
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2020  XorTroll

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

#include <fs/fs_FileSystem.hpp>
#include <sstream>

namespace fs
{
    static u8 *work_buf = nullptr;

    void CreateConcatenationFile(String Path)
    {
        fsdevCreateFile(Path.AsUTF8().c_str(), 0, FsCreateOption_BigFile);
    }

    void CopyFileProgress(String Path, String NewPath, std::function<void(double Done, double Total)> Callback)
    {
        auto gexp = GetExplorerForPath(Path);
        auto ogexp = GetExplorerForPath(NewPath);
        auto fsize = gexp->GetFileSize(Path);
        if((fsize >= Size4GB) && (ogexp == GetSdCardExplorer())) CreateConcatenationFile(NewPath);
        gexp->CopyFileProgress(Path, NewPath, Callback);
    }

    void CopyDirectoryProgress(String Dir, String NewDir, std::function<void(double Done, double Total)> Callback)
    {
        auto gexp = GetExplorerForPath(Dir);
        gexp->CopyDirectoryProgress(Dir, NewDir, Callback);
    }

    u64 GetTotalSpaceForPartition(Partition Partition)
    {
        u64 space = 0;
        fs::Explorer *fexp = nullptr;
        switch(Partition)
        {
            case Partition::PRODINFOF:
                fexp = fs::GetPRODINFOFExplorer();
                break;
            case Partition::NANDSafe:
                fexp = fs::GetNANDSafeExplorer();
                break;
            case Partition::NANDSystem:
                fexp = fs::GetNANDSystemExplorer();
                break;
            case Partition::NANDUser:
                fexp = fs::GetNANDUserExplorer();
                break;
            case Partition::SdCard:
                fexp = fs::GetSdCardExplorer();
                break;
        }
        if(fexp != nullptr) space = fexp->GetTotalSpace();
        return space;
    }

    u64 GetFreeSpaceForPartition(Partition Partition)
    {
        u64 space = 0;
        fs::Explorer *fexp = nullptr;
        switch(Partition)
        {
            case Partition::PRODINFOF:
                fexp = fs::GetPRODINFOFExplorer();
                break;
            case Partition::NANDSafe:
                fexp = fs::GetNANDSafeExplorer();
                break;
            case Partition::NANDSystem:
                fexp = fs::GetNANDSystemExplorer();
                break;
            case Partition::NANDUser:
                fexp = fs::GetNANDUserExplorer();
                break;
            case Partition::SdCard:
                fexp = fs::GetSdCardExplorer();
                break;
        }
        if(fexp != nullptr) space = fexp->GetFreeSpace();
        return space;
    }

    static const char *SizeSuffixes[] = { " bytes", " KB", " MB", " GB", " TB", " PB", " EB" };

    String FormatSize(u64 Bytes)
    {
        if(Bytes == 0) return String("0") + SizeSuffixes[0];
        u32 plc = floor((log(Bytes) / log(1024)));
        double btnum = (double)(Bytes / pow(1024, plc));
        double rbt = ((int)(btnum * 100.0) / 100.0);
        std::stringstream strm;
        strm << rbt;
        return (strm.str() + SizeSuffixes[plc]);
    }

    u8 *GetWorkBuffer()
    {
        if(work_buf == nullptr) work_buf = new (std::align_val_t(0x1000)) u8[WorkBufferSize]();
        memset(work_buf, 0, WorkBufferSize);
        return work_buf;
    }
}