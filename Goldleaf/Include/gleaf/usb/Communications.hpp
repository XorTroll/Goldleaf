
/*

    Goldleaf - Nintendo Switch title manager homebrew

    Copyright 2018 - Goldleaf project, developed by XorTroll
    This project is under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

*/

#pragma once
#include <gleaf/usb/Commands.hpp>

namespace gleaf::usb
{
    size_t Read(void *Out, size_t Size);
    Command ReadCommand();
    u32 Read32();
    u64 Read64();
    std::string ReadString(u32 Length);
    size_t ReadFixed(void *Out, size_t Size);
    size_t Write(const void *Buffer, size_t Size);
    void WriteCommand(Command &Data);
    void Write32(u32 Data);
    void write64(u64 Data);
    void WriteString(std::string Data);
    size_t WriteFixed(const void *Buffer, size_t Size);
}