
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright © 2018-2025 XorTroll

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

#include <usb/usb_Base.hpp>

namespace usb {

    namespace {

        constexpr u32 DefaultInterfaceNumber = 0;
        constexpr u32 HighSpeedMaxPacketLength = 0x200;
        constexpr u32 SuperSpeedMaxPacketLength = 0x400;

        constexpr UsbCommsInterfaceInfo InterfaceInfo = {
            .bInterfaceClass = USB_CLASS_VENDOR_SPEC,
            .bInterfaceSubClass = USB_CLASS_VENDOR_SPEC,
            .bInterfaceProtocol = USB_CLASS_VENDOR_SPEC,
        };

        bool g_Initialized = false;
        UsbDsInterface *g_Interface;
        UsbDsEndpoint *g_EndpointIn;
        UsbDsEndpoint *g_EndpointOut;

        Result InitializeInterface5x() {
            auto rc = rc::ResultSuccess;
    
            struct usb_interface_descriptor interface_descriptor = {
                .bLength = USB_DT_INTERFACE_SIZE,
                .bDescriptorType = USB_DT_INTERFACE,
                .bInterfaceNumber = DefaultInterfaceNumber,
                .bNumEndpoints = 2,
                .bInterfaceClass = InterfaceInfo.bInterfaceClass,
                .bInterfaceSubClass = InterfaceInfo.bInterfaceSubClass,
                .bInterfaceProtocol = InterfaceInfo.bInterfaceProtocol,
            };

            struct usb_endpoint_descriptor endpoint_descriptor_in = {
                .bLength = USB_DT_ENDPOINT_SIZE,
                .bDescriptorType = USB_DT_ENDPOINT,
                .bEndpointAddress = USB_ENDPOINT_IN,
                .bmAttributes = USB_TRANSFER_TYPE_BULK,
                .wMaxPacketSize = HighSpeedMaxPacketLength,
            };

            struct usb_endpoint_descriptor endpoint_descriptor_out = {
                .bLength = USB_DT_ENDPOINT_SIZE,
                .bDescriptorType = USB_DT_ENDPOINT,
                .bEndpointAddress = USB_ENDPOINT_OUT,
                .bmAttributes = USB_TRANSFER_TYPE_BULK,
                .wMaxPacketSize = HighSpeedMaxPacketLength,
            };
            
            struct usb_ss_endpoint_companion_descriptor endpoint_companion = {
                .bLength = sizeof(struct usb_ss_endpoint_companion_descriptor),
                .bDescriptorType = USB_DT_SS_ENDPOINT_COMPANION,
                .bMaxBurst = 0x0F,
                .bmAttributes = 0x00,
                .wBytesPerInterval = 0x00,
            };
            
            rc = usbDsRegisterInterface(&g_Interface);
            if(R_FAILED(rc)) {
                return rc;
            }
            
            interface_descriptor.bInterfaceNumber = g_Interface->interface_index;
            endpoint_descriptor_in.bEndpointAddress += interface_descriptor.bInterfaceNumber + 1;
            endpoint_descriptor_out.bEndpointAddress += interface_descriptor.bInterfaceNumber + 1;

            /*
            // Full Speed Config
            rc = usbDsInterface_AppendConfigurationData(interface->interface, UsbDeviceSpeed_Full, &interface_descriptor, USB_DT_INTERFACE_SIZE);
            if(R_FAILED(rc)) {
                return rc;
            }
            rc = usbDsInterface_AppendConfigurationData(interface->interface, UsbDeviceSpeed_Full, &endpoint_descriptor_in, USB_DT_ENDPOINT_SIZE);
            if(R_FAILED(rc)) {
                return rc;
            }
            rc = usbDsInterface_AppendConfigurationData(interface->interface, UsbDeviceSpeed_Full, &endpoint_descriptor_out, USB_DT_ENDPOINT_SIZE);
            if(R_FAILED(rc)) {
                return rc;
            }
            */
            
            // High Speed Config
            endpoint_descriptor_in.wMaxPacketSize = HighSpeedMaxPacketLength;
            endpoint_descriptor_out.wMaxPacketSize = HighSpeedMaxPacketLength;
            rc = usbDsInterface_AppendConfigurationData(g_Interface, UsbDeviceSpeed_High, &interface_descriptor, USB_DT_INTERFACE_SIZE);
            if(R_FAILED(rc)) {
                return rc;
            }
            rc = usbDsInterface_AppendConfigurationData(g_Interface, UsbDeviceSpeed_High, &endpoint_descriptor_in, USB_DT_ENDPOINT_SIZE);
            if(R_FAILED(rc)) {
                return rc;
            }
            rc = usbDsInterface_AppendConfigurationData(g_Interface, UsbDeviceSpeed_High, &endpoint_descriptor_out, USB_DT_ENDPOINT_SIZE);
            if(R_FAILED(rc)) {
                return rc;
            }
            
            // Super Speed Config
            endpoint_descriptor_in.wMaxPacketSize = SuperSpeedMaxPacketLength;
            endpoint_descriptor_out.wMaxPacketSize = SuperSpeedMaxPacketLength;
            rc = usbDsInterface_AppendConfigurationData(g_Interface, UsbDeviceSpeed_Super, &interface_descriptor, USB_DT_INTERFACE_SIZE);
            if(R_FAILED(rc)) {
                return rc;
            }
            rc = usbDsInterface_AppendConfigurationData(g_Interface, UsbDeviceSpeed_Super, &endpoint_descriptor_in, USB_DT_ENDPOINT_SIZE);
            if(R_FAILED(rc)) {
                return rc;
            }
            rc = usbDsInterface_AppendConfigurationData(g_Interface, UsbDeviceSpeed_Super, &endpoint_companion, USB_DT_SS_ENDPOINT_COMPANION_SIZE);
            if(R_FAILED(rc)) {
                return rc;
            }
            rc = usbDsInterface_AppendConfigurationData(g_Interface, UsbDeviceSpeed_Super, &endpoint_descriptor_out, USB_DT_ENDPOINT_SIZE);
            if(R_FAILED(rc)) {
                return rc;
            }
            rc = usbDsInterface_AppendConfigurationData(g_Interface, UsbDeviceSpeed_Super, &endpoint_companion, USB_DT_SS_ENDPOINT_COMPANION_SIZE);
            if(R_FAILED(rc)) {
                return rc;
            }
            
            //Setup endpoints.    
            rc = usbDsInterface_RegisterEndpoint(g_Interface, &g_EndpointIn, endpoint_descriptor_in.bEndpointAddress);
            if(R_FAILED(rc)) {
                return rc;
            }
            
            rc = usbDsInterface_RegisterEndpoint(g_Interface, &g_EndpointOut, endpoint_descriptor_out.bEndpointAddress);
            if(R_FAILED(rc)) {
                return rc;
            }

            rc = usbDsInterface_EnableInterface(g_Interface);
            if(R_FAILED(rc)) {
                return rc;
            }
            
            return rc;
        }


        Result InitializeInterface1x() {
            auto rc = rc::ResultSuccess;

            struct usb_interface_descriptor interface_descriptor = {
                .bLength = USB_DT_INTERFACE_SIZE,
                .bDescriptorType = USB_DT_INTERFACE,
                .bInterfaceNumber = DefaultInterfaceNumber,
                .bInterfaceClass = InterfaceInfo.bInterfaceClass,
                .bInterfaceSubClass = InterfaceInfo.bInterfaceSubClass,
                .bInterfaceProtocol = InterfaceInfo.bInterfaceProtocol,
            };

            struct usb_endpoint_descriptor endpoint_descriptor_in = {
                .bLength = USB_DT_ENDPOINT_SIZE,
                .bDescriptorType = USB_DT_ENDPOINT,
                .bEndpointAddress = USB_ENDPOINT_IN,
                .bmAttributes = USB_TRANSFER_TYPE_BULK,
                .wMaxPacketSize = HighSpeedMaxPacketLength,
            };

            struct usb_endpoint_descriptor endpoint_descriptor_out = {
                .bLength = USB_DT_ENDPOINT_SIZE,
                .bDescriptorType = USB_DT_ENDPOINT,
                .bEndpointAddress = USB_ENDPOINT_OUT,
                .bmAttributes = USB_TRANSFER_TYPE_BULK,
                .wMaxPacketSize = HighSpeedMaxPacketLength,
            };

            //Setup interface.
            rc = usbDsGetDsInterface(&g_Interface, &interface_descriptor, "usb");
            if(R_FAILED(rc)) {
                return rc;
            }

            //Setup endpoints.
            rc = usbDsInterface_GetDsEndpoint(g_Interface, &g_EndpointIn, &endpoint_descriptor_in);//device->host
            if(R_FAILED(rc)) {
                return rc;
            }

            rc = usbDsInterface_GetDsEndpoint(g_Interface, &g_EndpointOut, &endpoint_descriptor_out);//host->device
            if(R_FAILED(rc)) {
                return rc;
            }

            rc = usbDsInterface_EnableInterface(g_Interface);
            if(R_FAILED(rc)) {
                return rc;
            }

            return rc;
        }

        inline Result InitializeInterface() {
            if(hosversionAtLeast(5,0,0)) {
                return InitializeInterface5x();
            }
            else {
                return InitializeInterface1x();
            }
        }

        Result TransferImpl(void *buf, const size_t size, UsbDsEndpoint *ep) {
            auto state = UsbState_Detached;
            usbDsGetState(&state);
            if(state != UsbState_Configured) {
                return MAKERESULT(Module_Libnx, LibnxError_BadUsbCommsRead);
            }

            u32 urb_id = 0;
            auto rc = usbDsEndpoint_PostBufferAsync(ep, buf, size, &urb_id);
            if(R_SUCCEEDED(rc)) {
                rc = eventWait(&ep->CompletionEvent, UINT64_MAX);
                eventClear(&ep->CompletionEvent);
                
                if(R_SUCCEEDED(rc)) {
                    UsbDsReportData report_data;
                    rc = usbDsEndpoint_GetReportData(ep, &report_data);
                    u32 report_size = 0;
                    if(R_SUCCEEDED(rc)) {
                        rc = usbDsParseReportData(&report_data, urb_id, nullptr, &report_size);
                    }
                    if(report_size != size) {
                        return MAKERESULT(Module_Libnx, LibnxError_BadUsbCommsRead);
                    }
                }
            }
            return rc;
        }

        Result InitializeImpl() {
            auto rc = rc::ResultSuccess;

            if(!g_Initialized) {
                rc = usbDsInitialize();
                
                if(R_SUCCEEDED(rc)) {
                    if(hosversionAtLeast(5,0,0)) {
                        u8 iManufacturer, iProduct, iSerialNumber;
                        const u16 supported_langs[1] = { 0x0409 };
                        // Send language descriptor
                        rc = usbDsAddUsbLanguageStringDescriptor(nullptr, supported_langs, sizeof(supported_langs) / sizeof(u16));
                        // Send manufacturer
                        if(R_SUCCEEDED(rc)) {
                            rc = usbDsAddUsbStringDescriptor(&iManufacturer, "XorTroll");
                        }
                        // Send product
                        if(R_SUCCEEDED(rc)) {
                            rc = usbDsAddUsbStringDescriptor(&iProduct, "Goldleaf");
                        }
                        // Send serial number
                        if(R_SUCCEEDED(rc)) {
                            rc = usbDsAddUsbStringDescriptor(&iSerialNumber, GLEAF_VERSION);
                        }
                        
                        // Send device descriptors
                        struct usb_device_descriptor device_descriptor = {
                            .bLength = USB_DT_DEVICE_SIZE,
                            .bDescriptorType = USB_DT_DEVICE,
                            .bcdUSB = 0x0200,
                            .bDeviceClass = 0x00,
                            .bDeviceSubClass = 0x00,
                            .bDeviceProtocol = 0x00,
                            .bMaxPacketSize0 = 0x40,
                            .idVendor = 0x057E,
                            .idProduct = 0x3000,
                            .bcdDevice = 0x0100,
                            .iManufacturer = iManufacturer,
                            .iProduct = iProduct,
                            .iSerialNumber = iSerialNumber,
                            .bNumConfigurations = 0x01,
                        };

                        /*
                        // Full Speed is USB 1.1
                        if(R_SUCCEEDED(rc)) {
                            rc = usbDsSetUsbDeviceDescriptor(UsbDeviceSpeed_Full, &device_descriptor);
                        }
                        */
                        
                        // High Speed is USB 2.0
                        device_descriptor.bcdUSB = 0x0200;
                        if(R_SUCCEEDED(rc)) {
                            rc = usbDsSetUsbDeviceDescriptor(UsbDeviceSpeed_High, &device_descriptor);
                        }
                        
                        // Super Speed is USB 3.0
                        device_descriptor.bcdUSB = 0x0300;
                        // Upgrade packet size to 512
                        device_descriptor.bMaxPacketSize0 = 0x09;
                        if(R_SUCCEEDED(rc)) {
                            rc = usbDsSetUsbDeviceDescriptor(UsbDeviceSpeed_Super, &device_descriptor);
                        }
                        
                        // Define Binary Object Store
                        u8 bos[0x16] = {
                            0x05, // .bLength
                            USB_DT_BOS, // .bDescriptorType
                            0x16, 0x00, // .wTotalLength
                            0x02, // .bNumDeviceCaps
                            
                            // USB 2.0
                            0x07, // .bLength
                            USB_DT_DEVICE_CAPABILITY, // .bDescriptorType
                            0x02, // .bDevCapabilityType
                            0x02, 0x00, 0x00, 0x00, // dev_capability_data
                            
                            // USB 3.0
                            0x0A, // .bLength
                            USB_DT_DEVICE_CAPABILITY, // .bDescriptorType
                            0x03, // .bDevCapabilityType
                            0x00, 0x0C, 0x00, 0x03, 0x00, 0x00, 0x00
                        };
                        if(R_SUCCEEDED(rc)) {
                            rc = usbDsSetBinaryObjectStore(bos, sizeof(bos));
                        }
                    }
                    
                    if(R_SUCCEEDED(rc)) {
                        rc = InitializeInterface();
                    }
                }
                
                if(R_SUCCEEDED(rc) && hosversionAtLeast(5,0,0)) {
                    rc = usbDsEnable();
                }

                if(R_FAILED(rc)) {
                    diagAbortWithResult(rc);
                }
            }
            
            if (R_SUCCEEDED(rc)) {
                g_Initialized = true;
            }

            return rc;
        }

    }

    Result Initialize() {
        return InitializeImpl();
    }

    void Finalize() {
        if(g_Initialized) {
            usbDsExit();

            g_EndpointIn = nullptr;
            g_EndpointOut = nullptr;
            g_Interface = nullptr;

            g_Initialized = false;
        }
    }

    bool IsStateOk() {
        auto state = UsbState_Detached;
        usbDsGetState(&state);
        return state == UsbState_Configured;
    }

    Result Read(void *buf, const size_t size) {
        return TransferImpl(buf, size, g_EndpointOut);
    }

    Result Write(const void *buf, const size_t size) {
        return TransferImpl(const_cast<void*>(buf), size, g_EndpointIn);
    }

}
