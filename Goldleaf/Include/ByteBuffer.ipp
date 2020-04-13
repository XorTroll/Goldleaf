
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

template<typename Type>
Type ByteBuffer::Read(u64 Offset)
{
    if(Offset + sizeof(Type) <= buffer.size()) return *((Type*)&buffer.data()[Offset]);
    Type def;
    memset(&def, 0, sizeof(Type));
    return def;
}

template<typename Type>
void ByteBuffer::Write(Type Data, u64 Offset)
{
    size_t requiredSize = Offset + sizeof(Type);
    if(requiredSize > buffer.size()) buffer.resize(requiredSize, 0);
    memcpy(buffer.data() + Offset, &Data, sizeof(Type));
}

template<typename Type>
void ByteBuffer::Append(Type Data)
{
    this->Write(Data, this->GetSize());
}