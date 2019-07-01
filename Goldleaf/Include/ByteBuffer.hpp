
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch

    Copyright 2018 - 2019 Goldleaf project, developed by XorTroll, emerged from Adubbz's work with Tinfoil

    This project is licensed under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

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