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

# macOS Instructions:
# 1. Install Homebrew https://brew.sh
# 2. Install Python 3
#      sudo mkdir /usr/local/Frameworks
#      sudo chown $(whoami) /usr/local/Frameworks
#      brew install python
# 3. Install PyUSB 
#      pip3 install pyusb
# 4. Install libusb
#      brew install libusb
# 5. Plug in your Switch and go to Tinfoil > Title Management > USB Install NSP
# 6. Run this script
#      python3 usb_install_pc.py <path/to/nsp_folder>

import usb.core
import usb.util
import struct
import sys
from binascii import hexlify as hx, unhexlify as uhx
from pathlib import Path
from glob import glob
import os

CMD_ID_EXIT = 0
CMD_ID_FILE_RANGE = 1

CMD_TYPE_RESPONSE = 1

def flatMap(func, myList):
    return [j for i in myList for j in func(i)]

def send_response_header(out_ep, cmd_id, data_size):
    out_ep.write(b'TUC0') # Tinfoil USB Command 0
    out_ep.write(struct.pack('<B', CMD_TYPE_RESPONSE))
    out_ep.write(b'\x00' * 3)
    out_ep.write(struct.pack('<I', cmd_id))
    out_ep.write(struct.pack('<Q', data_size))
    out_ep.write(b'\x00' * 0xC)

def file_range_cmd(in_ep, out_ep, data_size):
    file_range_header = in_ep.read(0x20)

    range_size = struct.unpack('<Q', file_range_header[:8])[0]
    range_offset = struct.unpack('<Q', file_range_header[8:16])[0]
    nsp_name_len = struct.unpack('<Q', file_range_header[16:24])[0]
    #in_ep.read(0x8) # Reserved
    nsp_name = bytes(in_ep.read(nsp_name_len)).decode('utf-8')

    print('Range Size: {}, Range Offset: {}, Name len: {}, Name: {}'.format(range_size, range_offset, nsp_name_len, nsp_name))
    send_response_header(out_ep, CMD_ID_FILE_RANGE, range_size)

    with open(nsp_name, 'rb') as f:
        f.seek(range_offset)

        curr_off = 0x0
        end_off = range_size
        read_size = 0x100000

        while curr_off < end_off:
            if curr_off + read_size >= end_off:
                read_size = end_off - curr_off

            buf = f.read(read_size)
            out_ep.write(data=buf, timeout=0)
            curr_off += read_size

def poll_commands(in_ep, out_ep):
    while True:
        cmd_header = bytes(in_ep.read(0x20, timeout=0))
        magic = cmd_header[:4]
        print('Magic: {}'.format(magic), flush=True)

        if magic != b'TUC0': # Tinfoil USB Command 0
            continue

        cmd_type = struct.unpack('<B', cmd_header[4:5])[0]
        cmd_id = struct.unpack('<I', cmd_header[8:12])[0]
        data_size = struct.unpack('<Q', cmd_header[12:20])[0]

        print('Cmd Type: {}, Command id: {}, Data size: {}'.format(cmd_type, cmd_id, data_size), flush=True)

        if cmd_id == CMD_ID_EXIT:
            print('Exiting...')
            break
        elif cmd_id == CMD_ID_FILE_RANGE:
            file_range_cmd(in_ep, out_ep, data_size)

def send_nsp_list(nsp_path_list, out_ep):
    print('Sending header...')

    nsp_list = list(map(lambda path: str(path) + '\n', nsp_path_list))

    out_ep.write(b'TUL0') # Tinfoil USB List 0
    out_ep.write(struct.pack('<I', sum(map(len, nsp_list))))
    out_ep.write(b'\x00' * 0x8) # Padding

    print('Sending NSP list: {}'.format(nsp_list))
    
    for nsp_path in nsp_list:
        out_ep.write(nsp_path)


def print_usage():
    print("""\
usb_install_pc.py

Used for the installation of NSPs over USB.

Usage: usb_install_pc.py <nsp path|file pattern> [<nsp path|file pattern> ...]""")

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print_usage()
        sys.exit(1)

    nsps = list(
        filter(lambda path: os.path.isfile(path) and path.endswith('.nsp'),
            flatMap(
                lambda path: map(str, Path(path).iterdir()) if os.path.isdir(path) else [path] if os.path.isfile(path) else glob(path),
                sys.argv[1:]
            )
        )
    )

    if len(nsps) == 0:
        print("No nsp files found/specified")
        sys.exit(1)

    # Find the switch
    dev = usb.core.find(idVendor=0x057E, idProduct=0x3000)

    if dev is None:
        raise ValueError('Switch is not found!')

    dev.reset()
    dev.set_configuration()
    cfg = dev.get_active_configuration()

    is_out_ep = lambda ep: usb.util.endpoint_direction(ep.bEndpointAddress) == usb.util.ENDPOINT_OUT
    is_in_ep = lambda ep: usb.util.endpoint_direction(ep.bEndpointAddress) == usb.util.ENDPOINT_IN
    out_ep = usb.util.find_descriptor(cfg[(0,0)], custom_match=is_out_ep)
    in_ep = usb.util.find_descriptor(cfg[(0,0)], custom_match=is_in_ep)

    assert out_ep is not None
    assert in_ep is not None

    send_nsp_list(nsps, out_ep)
    poll_commands(in_ep, out_ep)