/**
 * @file usb.h
 * @brief USB (usb:*) service IPC wrapper.
 * @author SciresM, yellows8
 * @copyright libnx Authors
 */
#pragma once
#include <switch/types.h>
#include <switch/services/sm.h>
#include <switch/kernel/event.h>

#ifdef __cplusplus
extern "C" {
#endif

/// usb:ds Switch-as-device<>host USB comms, see also here: http://switchbrew.org/index.php?title=USB_services

/// Names starting with "libusb" were changed to "usb" to avoid collision with actual libusb if it's ever used.

#define USBDS_DEFAULT_InterfaceNumber 0x4 ///Value for usb_interface_descriptor bInterfaceNumber for automatically allocating the actual bInterfaceNumber.

/// Imported from libusb with changed names.
/* Descriptor sizes per descriptor type */
#define USB_DT_INTERFACE_SIZE        9
#define USB_DT_ENDPOINT_SIZE         7
#define USB_DT_DEVICE_SIZE         0x12
#define USB_DT_SS_ENDPOINT_COMPANION_SIZE 6

/// Imported from libusb, with some adjustments.
struct usb_endpoint_descriptor {
    uint8_t  bLength;
    uint8_t  bDescriptorType; /// Must match USB_DT_ENDPOINT.
    uint8_t  bEndpointAddress; /// Should be one of the usb_endpoint_direction values, the endpoint-number is automatically allocated.
    uint8_t  bmAttributes;
    uint16_t wMaxPacketSize;
    uint8_t  bInterval;
};

/// Imported from libusb, with some adjustments.
struct usb_interface_descriptor {
    uint8_t  bLength;
    uint8_t  bDescriptorType; /// Must match USB_DT_INTERFACE.
    uint8_t  bInterfaceNumber; /// See also USBDS_DEFAULT_InterfaceNumber.
    uint8_t  bAlternateSetting; /// Must match 0.
    uint8_t  bNumEndpoints;
    uint8_t  bInterfaceClass;
    uint8_t  bInterfaceSubClass;
    uint8_t  bInterfaceProtocol;
    uint8_t  iInterface; /// Ignored.
};

/// Imported from libusb, with some adjustments.
struct usb_device_descriptor {
    uint8_t  bLength;
    uint8_t  bDescriptorType; /// Must match USB_DT_Device.
    uint16_t bcdUSB;
    uint8_t  bDeviceClass;
    uint8_t  bDeviceSubClass;
    uint8_t  bDeviceProtocol;
    uint8_t  bMaxPacketSize0;
    uint16_t idVendor;
    uint16_t idProduct;
    uint16_t bcdDevice;
    uint8_t  iManufacturer;
    uint8_t  iProduct;
    uint8_t  iSerialNumber;
    uint8_t  bNumConfigurations;
};

/// Imported from libusb, with some adjustments.
struct usb_ss_endpoint_companion_descriptor {
    uint8_t  bLength;
    uint8_t  bDescriptorType; /// Must match USB_DT_SS_ENDPOINT_COMPANION.
    uint8_t  bMaxBurst;
    uint8_t  bmAttributes;
    uint16_t wBytesPerInterval;
};

/// Imported from libusb, with some adjustments.
struct usb_string_descriptor {
    uint8_t bLength;
    uint8_t bDescriptorType; /// Must match USB_DT_STRING.
    uint16_t wData[0x40];
};

typedef struct {
    u16 idVendor; /// VID
    u16 idProduct; /// PID
    u16 bcdDevice;
    char Manufacturer[0x20];
    char Product[0x20];
    char SerialNumber[0x20];
} UsbDsDeviceInfo;

typedef struct {
    u32 id; /// urbId from post-buffer cmds
    u32 requestedSize;
    u32 transferredSize;
    u32 urb_status;
} UsbDsReportEntry;

typedef struct {
    UsbDsReportEntry report[8];
    u32 report_count;
} UsbDsReportData;

typedef struct {
    bool initialized;
    u32 interface_index;
    Service  h;

    Event SetupEvent;
    Event CtrlInCompletionEvent;
    Event CtrlOutCompletionEvent;
} UsbDsInterface;

typedef struct {
    bool initialized;
    Service h;
    Event CompletionEvent;
} UsbDsEndpoint;

typedef enum {
    UsbComplexId_Default = 0x2
} UsbComplexId;

typedef enum {
    UsbDeviceSpeed_Full = 0x2,
    UsbDeviceSpeed_High = 0x3,
    UsbDeviceSpeed_Super = 0x4,
} UsbDeviceSpeed;

/// Imported from libusb, with changed names.
enum usb_class_code {
    USB_CLASS_PER_INTERFACE = 0,
    USB_CLASS_AUDIO = 1,
    USB_CLASS_COMM = 2,
    USB_CLASS_HID = 3,
    USB_CLASS_PHYSICAL = 5,
    USB_CLASS_PRINTER = 7,
    USB_CLASS_PTP = 6, /* legacy name from libusb-0.1 usb.h */
    USB_CLASS_IMAGE = 6,
    USB_CLASS_MASS_STORAGE = 8,
    USB_CLASS_HUB = 9,
    USB_CLASS_DATA = 10,
    USB_CLASS_SMART_CARD = 0x0b,
    USB_CLASS_CONTENT_SECURITY = 0x0d,
    USB_CLASS_VIDEO = 0x0e,
    USB_CLASS_PERSONAL_HEALTHCARE = 0x0f,
    USB_CLASS_DIAGNOSTIC_DEVICE = 0xdc,
    USB_CLASS_WIRELESS = 0xe0,
    USB_CLASS_APPLICATION = 0xfe,
    USB_CLASS_VENDOR_SPEC = 0xff
};

/// Imported from libusb, with changed names.
enum usb_descriptor_type {
    USB_DT_DEVICE = 0x01,
    USB_DT_CONFIG = 0x02,
    USB_DT_STRING = 0x03,
    USB_DT_INTERFACE = 0x04,
    USB_DT_ENDPOINT = 0x05,
    USB_DT_BOS = 0x0f,
    USB_DT_DEVICE_CAPABILITY = 0x10,
    USB_DT_HID = 0x21,
    USB_DT_REPORT = 0x22,
    USB_DT_PHYSICAL = 0x23,
    USB_DT_HUB = 0x29,
    USB_DT_SUPERSPEED_HUB = 0x2a,
    USB_DT_SS_ENDPOINT_COMPANION = 0x30
};

/// Imported from libusb, with changed names.
enum usb_endpoint_direction {
    USB_ENDPOINT_IN = 0x80,
    USB_ENDPOINT_OUT = 0x00
};

/// Imported from libusb, with changed names.
enum usb_transfer_type {
    USB_TRANSFER_TYPE_CONTROL = 0,
    USB_TRANSFER_TYPE_ISOCHRONOUS = 1,
    USB_TRANSFER_TYPE_BULK = 2,
    USB_TRANSFER_TYPE_INTERRUPT = 3,
    USB_TRANSFER_TYPE_BULK_STREAM = 4,
};

/// Imported from libusb, with changed names.
enum usb_iso_sync_type {
    USB_ISO_SYNC_TYPE_NONE = 0,
    USB_ISO_SYNC_TYPE_ASYNC = 1,
    USB_ISO_SYNC_TYPE_ADAPTIVE = 2,
    USB_ISO_SYNC_TYPE_SYNC = 3
};

/// Imported from libusb, with changed names.
enum usb_iso_usage_type {
    USB_ISO_USAGE_TYPE_DATA = 0,
    USB_ISO_USAGE_TYPE_FEEDBACK = 1,
    USB_ISO_USAGE_TYPE_IMPLICIT = 2,
};

/// Opens a session with usb:ds.
Result usbDsInitialize(void);
/// Closes the usb:ds session. Any interfaces/endpoints which are left open are automatically closed, since otherwise usb-sysmodule won't fully reset usb:ds to defaults.
void usbDsExit(void);

/// Helpers
Result usbDsWaitReady(u64 timeout);
Result usbDsParseReportData(UsbDsReportData *reportdata, u32 urbId, u32 *requestedSize, u32 *transferredSize);

/// IDsService
// Do not provide API access to these functions, as they're handled by usbDsInitialize().
// Result usbDsBindDevice(UsbComplexId complexId);
// Result usbDsBindClientProcess(Handle prochandle);
Event* usbDsGetStateChangeEvent(void);
Result usbDsGetState(u32* out);

/// Removed in 5.0.0
Result usbDsGetDsInterface(UsbDsInterface** out, struct usb_interface_descriptor* descriptor, const char* interface_name);
Result usbDsSetVidPidBcd(const UsbDsDeviceInfo* deviceinfo);

/// Added in 5.0.0
Result usbDsRegisterInterface(UsbDsInterface** out, u32 intf_num);
Result usbDsClearDeviceData(void);
Result usbDsAddUsbStringDescriptor(u8* out_index, const char* string);
Result usbDsAddUsbLanguageStringDescriptor(u8* out_index, const u16* lang_ids, u16 num_langs);
Result usbDsDeleteUsbStringDescriptor(u8 index);
Result usbDsSetUsbDeviceDescriptor(UsbDeviceSpeed speed, struct usb_device_descriptor* descriptor);
Result usbDsSetBinaryObjectStore(void* bos, size_t bos_size);
Result usbDsEnable(void);
Result usbDsDisable(void);

/// IDsInterface
void usbDsInterface_Close(UsbDsInterface* interface);

Result usbDsInterface_GetSetupPacket(UsbDsInterface* interface, void* buffer, size_t size);
Result usbDsInterface_EnableInterface(UsbDsInterface* interface);
Result usbDsInterface_DisableInterface(UsbDsInterface* interface);
Result usbDsInterface_CtrlInPostBufferAsync(UsbDsInterface* interface, void* buffer, size_t size, u32* urbId);
Result usbDsInterface_CtrlOutPostBufferAsync(UsbDsInterface* interface, void* buffer, size_t size, u32* urbId);
Result usbDsInterface_GetCtrlInReportData(UsbDsInterface* interface, UsbDsReportData* out);
Result usbDsInterface_GetCtrlOutReportData(UsbDsInterface* interface, UsbDsReportData* out);
Result usbDsInterface_StallCtrl(UsbDsInterface* interface);

/// Removed in 5.0.0
Result usbDsInterface_GetDsEndpoint(UsbDsInterface* interface, UsbDsEndpoint** endpoint, struct usb_endpoint_descriptor* descriptor);

/// Added in 5.0.0
Result usbDsInterface_RegisterEndpoint(UsbDsInterface* interface, UsbDsEndpoint** endpoint, u8 endpoint_address);
Result usbDsInterface_AppendConfigurationData(UsbDsInterface* interface, UsbDeviceSpeed speed, void* buffer, size_t size);


/// IDsEndpoint
void usbDsEndpoint_Close(UsbDsEndpoint* endpoint);

Result usbDsEndpoint_Cancel(UsbDsEndpoint* endpoint);
Result usbDsEndpoint_PostBufferAsync(UsbDsEndpoint* endpoint, void* buffer, size_t size, u32* urbId);
Result usbDsEndpoint_GetReportData(UsbDsEndpoint* endpoint, UsbDsReportData* out);
Result usbDsEndpoint_StallCtrl(UsbDsEndpoint* endpoint);
Result usbDsEndpoint_SetZlt(UsbDsEndpoint* endpoint, bool zlt);

#ifdef __cplusplus
}
#endif