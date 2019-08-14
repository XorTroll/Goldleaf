
// Based on libnx's usb_comms.h

#pragma once
#include <switch.h>


namespace usb::comms
{
    typedef bool (*CallbackFunction)(void);
    static constexpr size_t TotalInterfaces = 4;

    Result Initialize(void);
    Result InitializeEx(u32 num_interfaces, const UsbCommsInterfaceInfo *infos);
    void Exit(void);
    void SetErrorHandling(bool flag);
    Result Read(void* buffer, size_t size, size_t *read, CallbackFunction cb);
    size_t Write(const void* buffer, size_t size);
    Result ReadEx(void* buffer, size_t size, u32 interface, size_t *read, CallbackFunction cb);
    size_t WriteEx(const void* buffer, size_t size, u32 interface);
}