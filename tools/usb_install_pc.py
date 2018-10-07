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

CMD_ID_EXIT = 0
CMD_ID_FILE_RANGE = 1

CMD_TYPE_RESPONSE = 1

def send_response_header(out_ep, cmd_id, data_size):
    out_ep.write(b'TUC0') # Tinfoil USB List 0
    out_ep.write(struct.pack('<B', CMD_TYPE_RESPONSE))
    out_ep.write(b'\x00' * 3)
    out_ep.write(struct.pack('<I', cmd_id))
    out_ep.write(struct.pack('<Q', data_size))
    out_ep.write(b'\x00' * 0xC)

def file_range_cmd(nsp_dir, in_ep, out_ep, data_size):
    range_size = struct.unpack('<Q', in_ep.read(0x8))[0]
    range_offset = struct.unpack('<Q', in_ep.read(0x8))[0]
    nsp_name_len = struct.unpack('<Q', in_ep.read(0x8))[0]
    in_ep.read(0x8) # Reserved
    nsp_name = bytes(in_ep.read(nsp_name_len)).decode('utf-8')

    print('Range Size: {}, Range Offset: {}, Name len: {}, Name: {}'.format(range_size, range_offset, nsp_name_len, nsp_name))

    send_response_header(out_ep, CMD_ID_FILE_RANGE, range_size)

    with open(nsp_name, 'rb') as f:
        f.seek(range_offset)

        curr_off = 0x0
        end_off = range_size
        read_size = 0x8

        while curr_off < end_off:
            if curr_off + read_size >= end_off:
                read_size = end_off - curr_off

            buf = f.read(read_size)
            print('{}'.format(hx(buf)))
            out_ep.write(buf)
            curr_off += read_size

def poll_commands(nsp_dir, in_ep, out_ep):
    while True:
        magic = bytes(in_ep.read(0x4, timeout=0))

        print('Magic: {}'.format(magic), flush=True)

        if magic != b'TUC0': # Tinfoil USB Command 0
            continue

        cmd_type = struct.unpack('<B', in_ep.read(0x1))[0]
        in_ep.read(0x3)
        cmd_id = struct.unpack('<I', in_ep.read(0x4))[0]
        data_size = struct.unpack('<Q', in_ep.read(0x8))[0]
        in_ep.read(0xC) # Reserved

        print('Cmd Tye: {}, Command id: {}, Data size: {}'.format(cmd_type, cmd_id, data_size), flush=True)

        if cmd_id == CMD_ID_EXIT:
            print('Exiting...')
            break
        elif cmd_id == CMD_ID_FILE_RANGE:
            file_range_cmd(nsp_dir, in_ep, out_ep, data_size)

def send_nsp_list(nsp_dir, out_ep):
    nsp_path_list = list()
    nsp_path_list_len = 0

    # Add all files with the extension .nsp in the provided dir
    for nsp_path in [f for f in nsp_dir.iterdir() if f.is_file() and f.suffix == '.nsp']:
        nsp_path_list.append(nsp_path.__str__())
        nsp_path_list_len += len(nsp_path.__str__())

    print('Sending header...')

    out_ep.write(b'TUL0') # Tinfoil USB List 0
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

    dev.reset()
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