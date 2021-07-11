
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2021 XorTroll

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

namespace fs {

    namespace {

        const char *g_SizeSuffixes[] = { "bytes", "KB", "MB", "GB", "TB", "PB", "EB" };
        u8 *g_WorkBuffer = nullptr;

    }

    void CreateConcatenationFile(String path) {
        fsdevCreateFile(path.AsUTF8().c_str(), 0, FsCreateOption_BigFile);
    }

    void CopyFileProgress(String path, String new_path, std::function<void(double Done, double Total)> cb_fn) {
        auto exp = GetExplorerForPath(path);
        auto new_exp = GetExplorerForPath(new_path);
        const auto file_size = exp->GetFileSize(path);
        if((file_size >= Size4GB) && (new_exp == GetSdCardExplorer())) {
            CreateConcatenationFile(new_path);
        }
        exp->CopyFileProgress(path, new_path, cb_fn);
    }

    void CopyDirectoryProgress(String dir, String new_dir, std::function<void(double Done, double Total)> cb_fn) {
        auto exp = GetExplorerForPath(dir);
        exp->CopyDirectoryProgress(dir, new_dir, cb_fn);
    }

    u64 GetTotalSpaceForPartition(Partition partition) {
        fs::Explorer *exp = nullptr;
        switch(partition) {
            case Partition::PRODINFOF: {
                exp = fs::GetPRODINFOFExplorer();
                break;
            }
            case Partition::NANDSafe: {
                exp = fs::GetNANDSafeExplorer();
                break;
            }
            case Partition::NANDSystem: {
                exp = fs::GetNANDSystemExplorer();
                break;
            }
            case Partition::NANDUser: {
                exp = fs::GetNANDUserExplorer();
                break;
            }
            case Partition::SdCard: {
                exp = fs::GetSdCardExplorer();
                break;
            }
        }
        if(exp != nullptr) {
            return exp->GetTotalSpace();
        }
        return 0;
    }

    u64 GetFreeSpaceForPartition(Partition partition) {
        fs::Explorer *exp = nullptr;
        switch(partition) {
            case Partition::PRODINFOF: {
                exp = fs::GetPRODINFOFExplorer();
                break;
            }
            case Partition::NANDSafe: {
                exp = fs::GetNANDSafeExplorer();
                break;
            }
            case Partition::NANDSystem: {
                exp = fs::GetNANDSystemExplorer();
                break;
            }
            case Partition::NANDUser: {
                exp = fs::GetNANDUserExplorer();
                break;
            }
            case Partition::SdCard: {
                exp = fs::GetSdCardExplorer();
                break;
            }
        }
        if(exp != nullptr) {
            return exp->GetFreeSpace();
        }
        return 0;
    }

    String FormatSize(u64 bytes) {
        if(bytes == 0) {
            return String("0") + " " + g_SizeSuffixes[0];
        }
        const auto plc = static_cast<u32>(floor(log(bytes) / log(1024)));
        const auto byte_num = (double)(bytes / pow(1024, plc));
        const double rbt = ((int)(byte_num * 100.0) / 100.0);
        std::stringstream strm;
        strm << rbt;
        return strm.str() + " " + g_SizeSuffixes[plc];
    }

    u8 *GetWorkBuffer() {
        if(g_WorkBuffer == nullptr) {
            g_WorkBuffer = new (std::align_val_t(0x1000)) u8[WorkBufferSize]();
        }
        memset(g_WorkBuffer, 0, WorkBufferSize);
        return g_WorkBuffer;
    }

}