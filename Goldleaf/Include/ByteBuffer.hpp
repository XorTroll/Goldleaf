
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
#include <Types.hpp>

class ByteBuffer {
    private:
        std::vector<u8> buffer;
    public:
        ByteBuffer(size_t reserve_size = 0);
        ~ByteBuffer();
        size_t GetSize();
        u8 *GetData();
        void Resize(size_t size);

        template<typename Type>
        Type Read(u64 offset) {
            if(offset + sizeof(Type) <= this->buffer.size()) {
                return *((Type*)&this->buffer.data()[offset]);
            }
            return {};
        }

        template<typename Type>
        void Write(Type data, u64 offset) {
            const auto required_size = offset + sizeof(Type);
            if(required_size > this->buffer.size()) {
                this->buffer.resize(required_size, 0);
            }
            memcpy(this->buffer.data() + offset, std::addressof(data), sizeof(Type));
        }

        template<typename Type>
        void Append(Type data) {
            this->Write(data, this->GetSize());
        }
};