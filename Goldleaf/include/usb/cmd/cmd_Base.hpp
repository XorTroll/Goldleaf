
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2022 XorTroll

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
#include <usb/usb_Base.hpp>

// TODO (low priority): C++20 concepts?

namespace usb::cmd {

    constexpr u32 InputMagic = 0x49434C47; // 'GLCI'
    constexpr u32 OutputMagic = 0x4F434C47; // 'GLCO'

    constexpr size_t BlockSize = 0x1000;

    struct BlockBase {
        size_t position;
        u8 *block_buf;
    };

    class InCommandBlock {
        private:
            BlockBase base;
            bool ok;

        public:
            InCommandBlock(const u32 cmd_id);
        
            bool WriteBuffer(const void *buf, const size_t size);
            bool WriteString(const std::string &val);

            inline bool IsOk() {
                return this->ok;
            }

            template<typename T>
            inline bool WriteValue(const T val) {
                return this->WriteBuffer(&val, sizeof(T));
            }

            Result Send();
    };

    class OutCommandBlock {
        private:
            BlockBase base;
            u32 magic;
            Result res;

        public:
            OutCommandBlock();

            void Cleanup();

            inline bool IsValid() {
                if(this->magic != OutputMagic) {
                    return false;
                }
                else {
                    return R_SUCCEEDED(this->res);
                }
            }

            bool ReadBuffer(void *buf, const size_t size);
            bool ReadString(std::string &out_str);

            template<typename T>
            inline bool ReadValue(T &out_val) {
                return this->ReadBuffer(std::addressof(out_val), sizeof(T));
            }

            inline Result GetResult() {
                return this->res;
            }
    };

    class CommandArgument {
        public:
            virtual bool ProcessIn(InCommandBlock &block) { return true; }
            virtual bool ProcessAfterIn() { return true; }
            virtual bool ProcessOut(OutCommandBlock &block) { return true; }
            virtual bool ProcessAfterOut() { return true; }
    };

    template<typename T>
    class InValue : public CommandArgument {
        private:
            T val;

        public:
            InValue(const T val) : val(val) {}

            inline bool ProcessIn(InCommandBlock &block) override {
                return block.WriteValue(this->val);
            }
    };

    template<typename T>
    class OutValue : public CommandArgument {
        private:
            T &val;

        public:
            OutValue(T &val) : val(val) {}
            
            inline bool ProcessOut(OutCommandBlock &block) override {
                return block.ReadValue(this->val);
            }
    };

    class InString : public CommandArgument {
        private:
            std::string val;

        public:
            InString(const std::string &val) : val(val) {}
            
            inline bool ProcessIn(InCommandBlock &block) override {
                return block.WriteString(this->val);
            }
    };

    class OutString : public CommandArgument {
        private:
            std::string &val;

        public:
            OutString(std::string &val) : val(val) {}
            
            inline bool ProcessOut(OutCommandBlock &block) override {
                return block.ReadString(this->val);
            }
    };

    class InBuffer : public CommandArgument {
        private:
            const void *buf;
            size_t size;

        public:
            InBuffer(const void *buf, const size_t size) : buf(buf), size(size) {}

            bool ProcessAfterIn() override;
    };

    class OutBuffer : public CommandArgument {
        private:
            void *buf;
            size_t size;

        public:
            OutBuffer(void *buf, const size_t size) : buf(buf), size(size) {}

            bool ProcessAfterOut() override;
    };

    template<u32 CommandId, typename ...Args>
    inline Result ProcessCommand(Args &&...args) {
        InCommandBlock block(CommandId);

        auto in_ok = true;
        ((in_ok &= args.ProcessIn(block)), ...);
        if(!in_ok) {
            return 0xBAB1;
        }
        
        const auto rc = block.Send();
        if(R_SUCCEEDED(rc)) {
            auto after_in_ok = true;
            ((after_in_ok &= args.ProcessAfterIn()), ...);
            if(!after_in_ok) {
                return 0xBAB2;
            }

            OutCommandBlock out_block = {};
            auto out_ok = true;
            if(out_block.IsValid()) {
                ((out_ok &= args.ProcessOut(out_block)), ...);
            }
            if(!out_ok) {
                return 0xBAB3;
            }

            out_block.Cleanup();

            auto after_out_ok = true;
            if(out_block.IsValid()) {
                ((after_out_ok &= args.ProcessAfterOut()), ...);
                if(!after_out_ok) {
                    return 0xBAB4;
                }
            }

            return out_block.GetResult();
        }
        else {
            return rc;
        }
    }

}