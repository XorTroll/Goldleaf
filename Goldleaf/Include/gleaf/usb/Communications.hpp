
/*

    Goldleaf - Nintendo Switch title manager homebrew (open-source)

    Copyright 2018 - Goldleaf project, developed and managed by XorTroll
    This project is under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

*/

#include <gleaf/Types.hpp>

namespace gleaf::usb
{
    enum class CommandType
    {
        Response,
        Request,
    };

    struct CommandHeader
    {
        u32 Magic;
        u8 CommandId;
        u8 Pad[0x3];
    } PACKED;

    size_t Read(void *Out, size_t Size);
    size_t Write(const void *Buffer, size_t Size);
    void SendCommandHeader(u32 CommandId, u64 DataSize);
    void SendExitCommand();

    // New magic: Goldleaf USB command (GLUC)
    static const u32 GLUC = 0x43554c47;
}