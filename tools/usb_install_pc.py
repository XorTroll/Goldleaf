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

import usb.core
import usb.util
import struct
import sys
from binascii import hexlify as hx, unhexlify as uhx
from pathlib import Path

CMD_FILE_RANGE = 0

def poll_commands(nsp_dir, in_ep, out_ep):
    while True:
        magic = in_ep.read(0x4, timeout=0)

        if magic != b'TUP0': # Tinfoil USB PC 0
            continue

        command_id = in_ep.read(0x4)
        nsp_name_len = in_ep.read(0x4)
        ext_data_len = in_ep.read(0x4)




def send_nsp_list(nsp_dir, out_ep):
    nsp_path_list = list()
    nsp_path_list_len = 0

    # Add all files with the extension .nsp in the provided dir
    for nsp_path in [f for f in nsp_dir.iterdir() if f.is_file() and f.suffix == '.nsp']:
        nsp_path_list.append(nsp_path.__str__())
        nsp_path_list_len += len(nsp_path.__str__())

    print('Sending header...')

    out_ep.write(b'TUS0') # Tinfoil USB Switch 0
    out_ep.write(struct.pack('<I', nsp_path_list_len))
    out_ep.write(b'\x00' * 0x8) # Padding

    print('Sending NSP list: {}'.format(nsp_path_list))
    
    for nsp_path in nsp_path_list:
        out_ep.write(nsp_path)

def print_usage():
    print("""\
usb_install_pc.py

Used for the installation of NSPs over USB.

Usage: usb_install_pc.py <nsp folder>""")

if __name__ == '__main__':
    if len(sys.argv) != 2:
        print_usage()
        sys.exit(1)

    nsp_dir = Path(sys.argv[1])

    if not nsp_dir.is_dir():
        raise ValueError('1st argument must be a directory')

    # Find the switch
    dev = usb.core.find(idVendor=0x057E, idProduct=0x3000)

    if dev is None:
        raise ValueError('Switch is not found!')

    dev.set_configuration()
    cfg = dev.get_active_configuration()

    is_out_ep = lambda ep: usb.util.endpoint_direction(ep.bEndpointAddress) == usb.util.ENDPOINT_OUT
    is_in_ep = lambda ep: usb.util.endpoint_direction(ep.bEndpointAddress) == usb.util.ENDPOINT_IN
    out_ep = usb.util.find_descriptor(cfg[(0,0)], custom_match=is_out_ep)
    in_ep = usb.util.find_descriptor(cfg[(0,0)], custom_match=is_in_ep)

    assert out_ep is not None
    assert in_ep is not None

    send_nsp_list(nsp_dir, out_ep)
    poll_commands(nsp_dir, in_ep, out_ep)    