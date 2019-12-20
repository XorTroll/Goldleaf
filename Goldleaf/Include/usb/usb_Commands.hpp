
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

#pragma once
#include <Types.hpp>
#include <usb/usb_Detail.hpp>

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
        StartFile,
        ReadFile,
        WriteFile,
        EndFile,
        Create,
        Delete,
        Rename,
        GetSpecialPathCount,
        GetSpecialPath,
        SelectFile
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
        void WriteString(String Value);
        void WriteBuffer(void *Buf, size_t Size);
        Result Send();
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
        String ReadString();
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
            InString(String Value);
            void ProcessIn(InCommandBlock &block);
            void ProcessAfterIn();
            void ProcessOut(OutCommandBlock &block);
            void ProcessAfterOut();
        private:
            String val;
    };

    class OutString : public CommandArgument
    {
        public:
            OutString(String &Value);
            void ProcessIn(InCommandBlock &block);
            void ProcessAfterIn();
            void ProcessOut(OutCommandBlock &block);
            void ProcessAfterOut();
        private:
            String &val;
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

    template<CommandId id, typename ...Args>
    Result ProcessCommand(Args &&...args)
    {
        InCommandBlock block(id);
        (args.ProcessIn(block), ...);
        auto rc = block.Send();
        if(R_SUCCEEDED(rc))
        {
            (args.ProcessAfterIn(), ...);
            OutCommandBlock outblock;
            if(outblock.IsValid()) (args.ProcessOut(outblock), ...);
            outblock.Cleanup();
            if(outblock.IsValid()) (args.ProcessAfterOut(), ...);
            rc = outblock.res;
        }
        return rc;
    }
}