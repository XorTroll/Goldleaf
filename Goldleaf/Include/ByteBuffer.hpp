
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

#pragma once
#include <switch.h>
#include <vector>

class ByteBuffer
{
    public:
        ByteBuffer(size_t ReserveSize = 0);
        ~ByteBuffer();
        size_t GetSize();
        u8 *GetData();
        void Resize(size_t Size);
        template<typename Type>
        Type Read(u64 Offset);
        template<typename Type>
        void Write(Type Data, u64 Offset);
        template<typename Type>
        void Append(Type Data);
    private:
        std::vector<u8> buffer;
};

#include <ByteBuffer.ipp>