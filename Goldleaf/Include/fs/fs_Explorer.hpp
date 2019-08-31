
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch

    Copyright 2018 - 2019 Goldleaf project, developed by XorTroll, emerged from Adubbz's work with Tinfoil

    This project is licensed under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

*/

#pragma once
#include <vector>
#include <fs/fs_Common.hpp>

namespace fs
{
    class Explorer
    {
        public:
            virtual bool ShouldWarnOnWriteAccess();
            void SetNames(pu::String MountName, pu::String DisplayName);
            bool NavigateBack();
            bool NavigateForward(pu::String Path);
            std::vector<pu::String> GetContents();
            pu::String GetMountName();
            pu::String GetCwd();
            pu::String GetPresentableCwd();
            pu::String FullPathFor(pu::String Path);
            pu::String FullPresentablePathFor(pu::String Path);
            pu::String MakeFull(pu::String Path);
            bool IsFullPath(pu::String Path);
            void CopyFile(pu::String Path, pu::String NewPath);
            void CopyFileProgress(pu::String Path, pu::String NewPath, std::function<void(u8 Percentage)> Callback);
            void CopyDirectory(pu::String Dir, pu::String NewDir);
            void CopyDirectoryProgress(pu::String Dir, pu::String NewDir, std::function<void(u8 Percentage)> Callback);
            bool IsFileBinary(pu::String Path);
            std::vector<u8> ReadFile(pu::String Path);
            std::vector<pu::String> ReadFileLines(pu::String Path, u32 LineOffset, u32 LineCount);
            std::vector<pu::String> ReadFileFormatHex(pu::String Path, u32 LineOffset, u32 LineCount);
            u64 GetDirectorySize(pu::String Path);
            void DeleteDirectory(pu::String Path);

            virtual std::vector<pu::String> GetDirectories(pu::String Path) = 0;
            virtual std::vector<pu::String> GetFiles(pu::String Path) = 0;
            virtual bool Exists(pu::String Path) = 0;
            virtual bool IsFile(pu::String Path) = 0;
            virtual bool IsDirectory(pu::String Path) = 0;
            virtual void CreateFile(pu::String Path) = 0;
            virtual void CreateDirectory(pu::String Path) = 0;
            virtual void RenameFile(pu::String Path, pu::String NewName) = 0;
            virtual void RenameDirectory(pu::String Path, pu::String NewName) = 0;
            virtual void DeleteFile(pu::String Path) = 0;
            virtual void DeleteDirectorySingle(pu::String Path) = 0;
            virtual u64 ReadFileBlock(pu::String Path, u64 Offset, u64 Size, u8 *Out) = 0;
            virtual u64 WriteFileBlock(pu::String Path, u8 *Data, u64 Size) = 0;
            virtual u64 GetFileSize(pu::String Path) = 0;
            virtual u64 GetTotalSpace() = 0;
            virtual u64 GetFreeSpace() = 0;
            virtual void SetArchiveBit(pu::String Path) = 0;
        protected:
            pu::String dspname;
            pu::String mntname;
            pu::String ecwd;
    };

    class StdExplorer : public Explorer
    {
        public:
            virtual std::vector<pu::String> GetDirectories(pu::String Path) override;
            virtual std::vector<pu::String> GetFiles(pu::String Path) override;
            virtual bool Exists(pu::String Path) override;
            virtual bool IsFile(pu::String Path) override;
            virtual bool IsDirectory(pu::String Path) override;
            virtual void CreateFile(pu::String Path) override;
            virtual void CreateDirectory(pu::String Path) override;
            virtual void RenameFile(pu::String Path, pu::String NewName) override;
            virtual void RenameDirectory(pu::String Path, pu::String NewName) override;
            virtual void DeleteFile(pu::String Path) override;
            virtual void DeleteDirectorySingle(pu::String Path) override;
            virtual u64 ReadFileBlock(pu::String Path, u64 Offset, u64 Size, u8 *Out) override;
            virtual u64 WriteFileBlock(pu::String Path, u8 *Data, u64 Size) override;
            virtual u64 GetFileSize(pu::String Path) override;
            virtual u64 GetTotalSpace() override;
            virtual u64 GetFreeSpace() override;
            virtual void SetArchiveBit(pu::String Path) override;
    };

    class SdCardExplorer final : public StdExplorer
    {
        public:
            SdCardExplorer();
            virtual u64 GetTotalSpace() override;
            virtual u64 GetFreeSpace() override;
    };

    class NANDExplorer final : public StdExplorer
    {
        public:
            NANDExplorer(Partition Part);
            ~NANDExplorer();
            Partition GetPartition();
            virtual bool ShouldWarnOnWriteAccess() override;
            virtual u64 GetTotalSpace() override;
            virtual u64 GetFreeSpace() override;
        private:
            Partition part;
            FsFileSystem fs;
    };

    class USBPCDriveExplorer final : public Explorer
    {
        public:
            USBPCDriveExplorer(pu::String MountName);
            virtual std::vector<pu::String> GetDirectories(pu::String Path) override;
            virtual std::vector<pu::String> GetFiles(pu::String Path) override;
            virtual bool Exists(pu::String Path) override;
            virtual bool IsFile(pu::String Path) override;
            virtual bool IsDirectory(pu::String Path) override;
            virtual void CreateFile(pu::String Path) override;
            virtual void CreateDirectory(pu::String Path) override;
            virtual void RenameFile(pu::String Path, pu::String NewName) override;
            virtual void RenameDirectory(pu::String Path, pu::String NewName) override;
            virtual void DeleteFile(pu::String Path) override;
            virtual void DeleteDirectorySingle(pu::String Path) override;
            virtual u64 ReadFileBlock(pu::String Path, u64 Offset, u64 Size, u8 *Out) override;
            virtual u64 WriteFileBlock(pu::String Path, u8 *Data, u64 Size) override;
            virtual u64 GetFileSize(pu::String Path) override;
            virtual u64 GetTotalSpace() override;
            virtual u64 GetFreeSpace() override;
            virtual void SetArchiveBit(pu::String Path) override;
    };

    class FileSystemExplorer final : public StdExplorer
    {
        public:
            FileSystemExplorer(pu::String MountName, pu::String DisplayName, FsFileSystem *FileSystem);
            ~FileSystemExplorer();
            FsFileSystem *GetFileSystem();
            virtual u64 GetTotalSpace() override;
            virtual u64 GetFreeSpace() override;
        private:
            FsFileSystem *fs;
    };

    SdCardExplorer *GetSdCardExplorer();
    NANDExplorer *GetPRODINFOFExplorer();
    NANDExplorer *GetNANDSafeExplorer();
    NANDExplorer *GetNANDUserExplorer();
    NANDExplorer *GetNANDSystemExplorer();
    USBPCDriveExplorer *GetUSBPCDriveExplorer(pu::String MountName);
    Explorer *GetExplorerForMountName(pu::String MountName);
    Explorer *GetExplorerForPath(pu::String Path);
}