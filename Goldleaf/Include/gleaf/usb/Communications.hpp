
/*

    Goldleaf - Nintendo Switch title manager homebrew (open-source)

    Copyright 2018 - Goldleaf project, developed and managed by XorTroll
    This project is under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

*/

#pragma once
#include <gleaf/Types.hpp>

namespace gleaf::usb
{
    size_t Read(void *Out, size_t Size);
    size_t ReadFixed(void *Out, size_t Size);
    size_t Write(const void *Buffer, size_t Size);
    size_t WriteFixed(const void *Buffer, size_t Size);
    bool WaitForConnection(u64 Timeout = UINT64_MAX);
}