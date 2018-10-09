/**
 * @file usb_comms.h
 * @brief USB comms.
 * @author yellows8
 * @author plutoo
 * @copyright libnx Authors
 */
#pragma once
#include <switch/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/// Initializes usbComms with the default number of interfaces (1)
Result usbCommsInitialize(void);
/// Initializes usbComms with a specific number of interfaces.
Result usbCommsInitializeEx(u32 num_interfaces);

/// Exits usbComms.
void usbCommsExit(void);

/// Read data with the default interface.
size_t usbCommsRead(void* buffer, size_t size);

/// Write data with the default interface.
size_t usbCommsWrite(const void* buffer, size_t size);

/// Same as usbCommsRead except with the specified interface.
size_t usbCommsReadEx(void* buffer, size_t size, u32 interface);

/// Same as usbCommsWrite except with the specified interface.
size_t usbCommsWriteEx(const void* buffer, size_t size, u32 interface);

#ifdef __cplusplus
}
#endif