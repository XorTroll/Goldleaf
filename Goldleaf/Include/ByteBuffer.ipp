
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch

    Copyright 2018 - 2019 Goldleaf project, developed by XorTroll, emerged from Adubbz's work with Tinfoil

    This project is licensed under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

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