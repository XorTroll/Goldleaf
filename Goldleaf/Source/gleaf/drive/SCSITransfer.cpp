#include <switch.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gleaf/drive/SCSIContext.hpp>

namespace gleaf::drive
{
    SCSICommandStatus SCSIDevice::read_csw()
    {
        uint32_t num_bytes;
        Result res = usbHsEpPostBuffer(out_endpoint, usb_bounce_buffer_c, 0x10, &num_bytes);
        if(R_FAILED(res))
        {
            printf("read_csw usb fail %08x\n", res);
        }
        if(num_bytes != 13)
        {
            printf("read_csw usb short read of length %i\n", num_bytes);
        }

        return SCSICommandStatus(usb_bounce_buffer_c);
    }

    void SCSIDevice::push_cmd(SCSICommand *c)
    {
        // Push a 31 byte command.
        uint32_t num_bytes;
        memset(usb_bounce_buffer_a, 0, 0x1000);
        c->to_bytes(usb_bounce_buffer_a);

        Result res = usbHsEpPostBuffer(in_endpoint, usb_bounce_buffer_a, 31, &num_bytes);
        if(R_FAILED(res))
        {
            printf("push_cmd usb fail %08x\n", res);
            return;
        }
        if(num_bytes != 31)
        {
            printf("push_cmd usb fail short write of %i bytes????\n", num_bytes);
            return;
        }
    }

    SCSICommandStatus SCSIDevice::transfer_cmd(SCSICommand *c, uint8_t *buffer, size_t buffer_size)
    {
        push_cmd(c);

        uint32_t transfer_length = c->data_transfer_length;
        uint32_t transferred = 0;
        uint32_t total_transferred = 0;
        
        if(buffer_size < transfer_length)
        {
            printf("Buffer too small!!!!\n");
        }

        Result res;

        if(transfer_length > 0)
        {
            if(c->direction == SCSIDirection::In)
            {
                while(total_transferred < transfer_length)
                {
                    memset(usb_bounce_buffer_b, 0, 0x1000);
                    res = usbHsEpPostBuffer(out_endpoint, usb_bounce_buffer_b, transfer_length - total_transferred, &transferred);

                    if(R_FAILED(res))
                    {
                        printf("usbHsEpPostBuffer failed %08x\n", res);
                        abort();
                    }

                    if(transferred == 13)
                    {
                        uint32_t signature;
                        memcpy(&signature, usb_bounce_buffer_b, 4);
                        if(signature == CSW_SIGNATURE)
                        {
                            // We weren't expecting a CSW!
                            // But we got one anyway!
                            return SCSICommandStatus(usb_bounce_buffer_b);
                        }
                    }

                    memcpy(buffer + total_transferred, usb_bounce_buffer_b, transferred);
                    total_transferred += transferred;
                }
            }
            else
            {
                while(total_transferred < transfer_length)
                {
                    memcpy(usb_bounce_buffer_b, buffer + total_transferred, transfer_length - total_transferred);
                    res = usbHsEpPostBuffer(in_endpoint, usb_bounce_buffer_b, transfer_length - total_transferred, &transferred);
                    if(R_FAILED(res))
                    {
                        printf("usbHsEpPostBuffer failed %08x\n", res);
                        abort();
                    }
                    total_transferred += transferred;
                }
            }
        }

        SCSICommandStatus w = read_csw();
        if(w.tag != c->tag)
        {
            // ???
        }

        if(w.status != COMMAND_PASSED)
        {
            // ???
        }

        return w;
    }
}