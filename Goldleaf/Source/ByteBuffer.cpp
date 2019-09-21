
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

#include <ByteBuffer.hpp>

ByteBuffer::ByteBuffer(size_t ReserveSize)
{
    buffer.resize(ReserveSize);
}

ByteBuffer::~ByteBuffer()
{
    buffer.clear();
}

size_t ByteBuffer::GetSize()
{
    return buffer.size();
}

u8 *ByteBuffer::GetData()
{
    return buffer.data();
}

void ByteBuffer::Resize(size_t Size)
{
    buffer.resize(Size, 0);
}