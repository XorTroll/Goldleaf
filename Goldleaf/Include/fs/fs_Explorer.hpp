
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
            void SetNames(std::string MountName, std::string DisplayName);
            bool NavigateBack();
            bool NavigateForward(std::string Path);
            std::vector<std::string> GetContents();
            std::string GetMountName();
            std::string GetCwd();
            std::string GetPresentableCwd();
            std::string FullPathFor(std::string Path);
            std::string FullPresentablePathFor(std::string Path);
            std::string MakeFull(std::string Path);
            bool IsFullPath(std::string Path);
            void CopyFile(std::string Path, std::string NewPath);
            void CopyFileProgress(std::string Path, std::string NewPath, std::function<void(u8 Percentage)> Callback);
            void CopyDirectory(std::string Dir, std::string NewDir);
            void CopyDirectoryProgress(std::string Dir, std::string NewDir, std::function<void(u8 Percentage)> Callback);
            bool IsFileBinary(std::string Path);
            std::vector<u8> ReadFile(std::string Path);
            std::vector<std::string> ReadFileLines(std::string Path, u32 LineOffset, u32 LineCount);
            std::vector<std::string> ReadFileFormatHex(std::string Path, u32 LineOffset, u32 LineCount);
            u64 GetDirectorySize(std::string Path);
            void DeleteDirectory(std::string Path);

            virtual std::vector<std::string> GetDirectories(std::string Path) = 0;
            virtual std::vector<std::string> GetFiles(std::string Path) = 0;
            virtual bool Exists(std::string Path) = 0;
            virtual bool IsFile(std::string Path) = 0;
            virtual bool IsDirectory(std::string Path) = 0;
            virtual void CreateFile(std::string Path) = 0;
            virtual void CreateDirectory(std::string Path) = 0;
            virtual void RenameFile(std::string Path, std::string NewName) = 0;
            virtual void RenameDirectory(std::string Path, std::string NewName) = 0;
            virtual void DeleteFile(std::string Path) = 0;
            virtual void DeleteDirectorySingle(std::string Path) = 0;
            virtual u64 ReadFileBlock(std::string Path, u64 Offset, u64 Size, u8 *Out) = 0;
            virtual u64 WriteFileBlock(std::string Path, u8 *Data, u64 Size) = 0;
            virtual u64 GetFileSize(std::string Path) = 0;
            virtual u64 GetTotalSpace() = 0;
            virtual u64 GetFreeSpace() = 0;
            virtual void SetArchiveBit(std::string Path) = 0;
        protected:
            std::string dspname;
            std::string mntname;
            std::string ecwd;
    };

    class StdExplorer : public Explorer
    {
        public:
            virtual std::vector<std::string> GetDirectories(std::string Path) override;
            virtual std::vector<std::string> GetFiles(std::string Path) override;
            virtual bool Exists(std::string Path) override;
            virtual bool IsFile(std::string Path) override;
            virtual bool IsDirectory(std::string Path) override;
            virtual void CreateFile(std::string Path) override;
            virtual void CreateDirectory(std::string Path) override;
            virtual void RenameFile(std::string Path, std::string NewName) override;
            virtual void RenameDirectory(std::string Path, std::string NewName) override;
            virtual void DeleteFile(std::string Path) override;
            virtual void DeleteDirectorySingle(std::string Path) override;
            virtual u64 ReadFileBlock(std::string Path, u64 Offset, u64 Size, u8 *Out) override;
            virtual u64 WriteFileBlock(std::string Path, u8 *Data, u64 Size) override;
            virtual u64 GetFileSize(std::string Path) override;
            virtual u64 GetTotalSpace() override;
            virtual u64 GetFreeSpace() override;
            virtual void SetArchiveBit(std::string Path) override;
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
            USBPCDriveExplorer(std::string MountName);
            virtual std::vector<std::string> GetDirectories(std::string Path) override;
            virtual std::vector<std::string> GetFiles(std::string Path) override;
            virtual bool Exists(std::string Path) override;
            virtual bool IsFile(std::string Path) override;
            virtual bool IsDirectory(std::string Path) override;
            virtual void CreateFile(std::string Path) override;
            virtual void CreateDirectory(std::string Path) override;
            virtual void RenameFile(std::string Path, std::string NewName) override;
            virtual void RenameDirectory(std::string Path, std::string NewName) override;
            virtual void DeleteFile(std::string Path) override;
            virtual void DeleteDirectorySingle(std::string Path) override;
            virtual u64 ReadFileBlock(std::string Path, u64 Offset, u64 Size, u8 *Out) override;
            virtual u64 WriteFileBlock(std::string Path, u8 *Data, u64 Size) override;
            virtual u64 GetFileSize(std::string Path) override;
            virtual u64 GetTotalSpace() override;
            virtual u64 GetFreeSpace() override;
            virtual void SetArchiveBit(std::string Path) override;
    };

    class FileSystemExplorer final : public StdExplorer
    {
        public:
            FileSystemExplorer(std::string MountName, std::string DisplayName, FsFileSystem *FileSystem, bool AutoClose);
            ~FileSystemExplorer();
            FsFileSystem *GetFileSystem();
            virtual u64 GetTotalSpace() override;
            virtual u64 GetFreeSpace() override;
        private:
            bool aclose;
            FsFileSystem *fs;
    };

    SdCardExplorer *GetSdCardExplorer();
    NANDExplorer *GetPRODINFOFExplorer();
    NANDExplorer *GetNANDSafeExplorer();
    NANDExplorer *GetNANDUserExplorer();
    NANDExplorer *GetNANDSystemExplorer();
    USBPCDriveExplorer *GetUSBPCDriveExplorer(std::string MountName);
    Explorer *GetExplorerForMountName(std::string MountName);
}