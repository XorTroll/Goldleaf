
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch

    Copyright 2018 - 2019 Goldleaf project, developed by XorTroll, emerged from Adubbz's work with Tinfoil

    This project is licensed under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

*/

#pragma once
#include <usb/usb_Communications.hpp>

namespace usb
{
    enum class CommandId
    {
        GetDriveCount,
        GetDriveInfo,
        StatPath,
        GetFileCount,
        GetFile,
        GetDirectoryCount,
        GetDirectory,
        ReadFile,
        WriteFile,
        Create,
        Delete,
        Rename,
        GetSpecialPathCount,
        GetSpecialPath,
        SelectFile,
        Max,
    };

    static constexpr u32 InputMagic = 0x49434C47; // GLCI
    static constexpr u32 OutputMagic = 0x4F434C47; // GLCO

    static constexpr size_t BlockSize = 0x1000;

    struct BlockBase
    {
        u32 position;
        u8 *blockbuf;
    };

    struct InCommandBlock
    {
        BlockBase base;

        InCommandBlock(CommandId CmdId);
        void Write32(u32 Value);
        void Write64(u64 Value);
        void WriteString(pu::String Value);
        void WriteBuffer(void *Buf, size_t Size);
        void Send();
    };

    struct OutCommandBlock
    {
        BlockBase base;
        u32 magic;
        Result res;

        OutCommandBlock();
        void Cleanup();
        bool IsValid();
        u32 Read32();
        u64 Read64();
        pu::String ReadString();
        void ReadBuffer(void *Buf, size_t Size);
    };

    class CommandArgument
    {
        public:
            virtual void ProcessIn(InCommandBlock &block) = 0;
            virtual void ProcessAfterIn() = 0;
            virtual void ProcessOut(OutCommandBlock &block) = 0;
            virtual void ProcessAfterOut() = 0;
    };

    class In32 : public CommandArgument
    {
        public:
            In32(u32 Value);
            void ProcessIn(InCommandBlock &block);
            void ProcessAfterIn();
            void ProcessOut(OutCommandBlock &block);
            void ProcessAfterOut();
        private:
            u32 val;
    };

    class Out32 : public CommandArgument
    {
        public:
            Out32(u32 &Value);
            void ProcessIn(InCommandBlock &block);
            void ProcessAfterIn();
            void ProcessOut(OutCommandBlock &block);
            void ProcessAfterOut();
        private:
            u32 &val;
    };

    class In64 : public CommandArgument
    {
        public:
            In64(u64 Value);
            void ProcessIn(InCommandBlock &block);
            void ProcessAfterIn();
            void ProcessOut(OutCommandBlock &block);
            void ProcessAfterOut();
        private:
            u64 val;
    };

    class Out64 : public CommandArgument
    {
        public:
            Out64(u64 &Value);
            void ProcessIn(InCommandBlock &block);
            void ProcessAfterIn();
            void ProcessOut(OutCommandBlock &block);
            void ProcessAfterOut();
        private:
            u64 &val;
    };

    class InString : public CommandArgument
    {
        public:
            InString(pu::String Value);
            void ProcessIn(InCommandBlock &block);
            void ProcessAfterIn();
            void ProcessOut(OutCommandBlock &block);
            void ProcessAfterOut();
        private:
            pu::String val;
    };

    class OutString : public CommandArgument
    {
        public:
            OutString(pu::String &Value);
            void ProcessIn(InCommandBlock &block);
            void ProcessAfterIn();
            void ProcessOut(OutCommandBlock &block);
            void ProcessAfterOut();
        private:
            pu::String &val;
    };

    class InBuffer : public CommandArgument
    {
        public:
            InBuffer(void *Buf, size_t Sz);
            void ProcessIn(InCommandBlock &block);
            void ProcessAfterIn();
            void ProcessOut(OutCommandBlock &block);
            void ProcessAfterOut();
        private:
            void *buf;
            size_t sz;
    };

    class OutBuffer : public CommandArgument
    {
        public:
            OutBuffer(void *Buf, size_t Sz);
            void ProcessIn(InCommandBlock &block);
            void ProcessAfterIn();
            void ProcessOut(OutCommandBlock &block);
            void ProcessAfterOut();
        private:
            void *buf;
            size_t sz;
    };

    template<CommandId CmdId, typename ...Args>
    Result ProcessCommand(Args &&...CmdArgs)
    {
        InCommandBlock block(CmdId);
        (CmdArgs.ProcessIn(block), ...);
        block.Send();
        (CmdArgs.ProcessAfterIn(), ...);
        OutCommandBlock outblock;
        if(outblock.IsValid()) (CmdArgs.ProcessOut(outblock), ...);
        outblock.Cleanup();
        if(outblock.IsValid()) (CmdArgs.ProcessAfterOut(), ...);
        return outblock.res;
    }
}