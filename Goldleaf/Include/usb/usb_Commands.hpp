
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

#pragma once
#include <usb/usb_Detail.hpp>

// TODO: C++20 concepts?

namespace usb {

    enum class CommandId {
        GetDriveCount = 1,
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

    constexpr u32 InputMagic = 0x49434C47; // GLCI
    constexpr u32 OutputMagic = 0x4F434C47; // GLCO

    constexpr size_t BlockSize = 0x1000;

    struct BlockBase {
        u64 position;
        u8 *block_buf;
    };

    struct InCommandBlock {
        BlockBase base;

        InCommandBlock(CommandId cmd_id);
        void Write32(u32 val);
        void Write64(u64 val);
        void WriteString(String val);
        void WriteBuffer(const void *buf, size_t size);
        Result Send();
    };

    struct OutCommandBlock {
        BlockBase base;
        u32 magic;
        Result res;

        OutCommandBlock();
        void Cleanup();
        bool IsValid();
        u32 Read32();
        u64 Read64();
        String ReadString();
        void ReadBuffer(void *buf, size_t size);
    };

    class CommandArgument {
        public:
            virtual void ProcessIn(InCommandBlock &block) = 0;
            virtual void ProcessAfterIn() = 0;
            virtual void ProcessOut(OutCommandBlock &block) = 0;
            virtual void ProcessAfterOut() = 0;
    };

    class In32 : public CommandArgument {
        private:
            u32 val;
        public:
            In32(u32 val);
            void ProcessIn(InCommandBlock &block);
            void ProcessAfterIn();
            void ProcessOut(OutCommandBlock &block);
            void ProcessAfterOut();
    };

    class Out32 : public CommandArgument {
        private:
            u32 &val;
        public:
            Out32(u32 &val);
            void ProcessIn(InCommandBlock &block);
            void ProcessAfterIn();
            void ProcessOut(OutCommandBlock &block);
            void ProcessAfterOut();
    };

    class In64 : public CommandArgument {
        private:
            u64 val;
        public:
            In64(u64 val);
            void ProcessIn(InCommandBlock &block);
            void ProcessAfterIn();
            void ProcessOut(OutCommandBlock &block);
            void ProcessAfterOut();
    };

    class Out64 : public CommandArgument {
        private:
            u64 &val;
        public:
            Out64(u64 &val);
            void ProcessIn(InCommandBlock &block);
            void ProcessAfterIn();
            void ProcessOut(OutCommandBlock &block);
            void ProcessAfterOut();
    };

    class InString : public CommandArgument {
        private:
            String val;
        public:
            InString(String val);
            void ProcessIn(InCommandBlock &block);
            void ProcessAfterIn();
            void ProcessOut(OutCommandBlock &block);
            void ProcessAfterOut();
    };

    class OutString : public CommandArgument {
        private:
            String &val;
        public:
            OutString(String &val);
            void ProcessIn(InCommandBlock &block);
            void ProcessAfterIn();
            void ProcessOut(OutCommandBlock &block);
            void ProcessAfterOut();
    };

    class InBuffer : public CommandArgument {
        private:
            const void *buf;
            size_t size;
        public:
            InBuffer(const void *buf, size_t size);
            void ProcessIn(InCommandBlock &block);
            void ProcessAfterIn();
            void ProcessOut(OutCommandBlock &block);
            void ProcessAfterOut();
    };

    class OutBuffer : public CommandArgument {
        private:
            void *buf;
            size_t size;
        public:
            OutBuffer(void *buf, size_t size);
            void ProcessIn(InCommandBlock &block);
            void ProcessAfterIn();
            void ProcessOut(OutCommandBlock &block);
            void ProcessAfterOut();
    };

    template<CommandId id, typename ...Args>
    Result ProcessCommand(Args &&...args) {
        InCommandBlock block(id);
        (args.ProcessIn(block), ...);
        const auto rc = block.Send();
        if(R_SUCCEEDED(rc)) {
            (args.ProcessAfterIn(), ...);
            OutCommandBlock out_block = {};
            if(out_block.IsValid()) {
                (args.ProcessOut(out_block), ...);
            }
            out_block.Cleanup();
            if(out_block.IsValid()) {
                (args.ProcessAfterOut(), ...);
            }
            return out_block.res;
        }
        return rc;
    }

}