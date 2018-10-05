# This script depends on PyUSB. You can get it with pip install pyusb.
# You will also need libusb installed

# My sincere apologies for this process being overly complicated. Apparently Python and Windows
# aren't very friendly :(
# Windows Instructions:
# 1. Download Zadig from https://zadig.akeo.ie/.
# 2. With your switch plugged in and on the Tinfoil USB install menu, 
#    choose "List All Devices" under the options menu in Zadig, and select libnx USB comms. 
# 3. Choose libusbK from the driver list and click the "Replace Driver" button.
# 4. Run this script

from binascii import hexlify as hx, unhexlify as uhx
import usb.core
import usb.util

print('Sending magic...')

# Find the switch
dev = usb.core.find(idVendor=0x057E, idProduct=0x3000)

if dev is None:
    raise ValueError('Switch is not found!')

dev.set_configuration()
cfg = dev.get_active_configuration()

is_out_ep = lambda ep: usb.util.endpoint_direction(ep.bEndpointAddress) == usb.util.ENDPOINT_OUT
out_ep = usb.util.find_descriptor(cfg[(0,0)], custom_match=is_out_ep)

assert out_ep is not None

buf = b'\x01' * 0x800000 

while True:
    out_ep.write(buf)
    