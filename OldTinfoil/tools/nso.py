import lz4.block
import sys
from binascii import unhexlify as uhx, hexlify as hx
from Crypto.Hash import HMAC, SHA256
from pathlib import Path
from struct import unpack as up, pack as pk

def sha256(data):
    return uhx(SHA256.new(data).hexdigest())

def read_at(fp, off, len):
    fp.seek(off)
    return fp.read(len)

def read_u8(fp, off):
    return up('<B', read_at(fp, off, 1))[0]

def read_u16(fp, off):
    return up('<H', read_at(fp, off, 2))[0]

def read_u32(fp, off):
    return up('<I', read_at(fp, off, 4))[0]

def read_u64(fp, off):
    return up('<Q', read_at(fp, off, 8))[0]

def read_str(fp, off, l):
    if l == 0:
        return ''
    s = read_at(fp, off, l)
    if '\0' in s:
        s = s[:s.index('\0')]
    return s

def write_at(fp, off, data):
    fp.seek(off)
    fp.write(data)

def extract_nso(path_in, path_out):
    with open(path_in, 'rb') as f:
        text_off = read_u32(f, 0x10)
        text_loc = read_u32(f, 0x14)
        text_size = read_u32(f, 0x18)
        text_compressed_size = read_u32(f, 0x60)

        test_data = b'hello'
        lz4.block.decompress(lz4.block.compress(test_data))

        print('Text offset: {}'.format(text_off))
        print('Text compressed size: {}'.format(text_compressed_size))
        print('Text uncompressed size: {}'.format(text_size))

        compressed_patched_text = read_at(f, text_off, text_compressed_size)
        print(hx(compressed_patched_text)[0:10])
        text = lz4.block.decompress(compressed_patched_text, uncompressed_size=text_size)
        decompressed_hash = read_at(f, 0xA0, 0x20)
        calculated_hash = sha256(text)

        print('Compressed size: {}'.format(text_compressed_size))
        print('Decompressed hash: {}'.format(hx(decompressed_hash)))
        print('Calculated hash: {}'.format(hx(calculated_hash)))

        if decompressed_hash == calculated_hash:
            print('Validated decompressed hash')
        else:
            print('Failed to validate decompressed hash')
            return

        with open(path_out, 'wb') as o:
            o.write(text)

def repack_nso(path_original, path_patch, path_out):
    nso = b''

    patched_text_hash = b''

    # Read the original NSO
    with open(path_original, 'rb') as f:
        nso = bytearray(f.read())

    # Read the patched text
    with open(path_patch, 'rb') as f:
        data = f.read()
        patched_text_hash = sha256(data)
        compressed_patched_text = lz4.block.compress(data, store_size=False)


    text_off = up('<I', nso[0x10:0x14])[0]
    text_compressed_size = len(compressed_patched_text)

    # Retrieve original rodata segment
    rodata_off = up('<I', nso[0x20:0x24])[0]
    rodata_compressed_size = up('<I', nso[0x64:0x68])[0]
    compressed_rodata = nso[rodata_off:rodata_off+rodata_compressed_size]

    # Retrieve original data segment
    data_off = up('<I', nso[0x30:0x34])[0]
    data_compressed_size = up('<I', nso[0x68:0x6C])[0]
    compressed_data = nso[data_off:data_off+data_compressed_size]

    # Set to the offsets of the output nso
    rodata_off = text_off + text_compressed_size
    data_off = rodata_off + rodata_compressed_size

    # Create the output nso
    out_nso = bytearray(data_off + data_compressed_size)

    # Copy over the original header
    out_nso[0x0:0x100] = nso[0x0:text_off]

    # Write the new text hash
    out_nso[0xA0:0xC0] = patched_text_hash

    # Write the new compressed text size
    out_nso[0x60:0x64] = pk('<I', text_compressed_size)

    # Correct the header offsets
    out_nso[0x20:0x24] = pk('<I', rodata_off) # rodata offset
    out_nso[0x30:0x34] = pk('<I', data_off) # data offset

    # Write new data
    out_nso[text_off:text_off+text_compressed_size] = compressed_patched_text
    out_nso[rodata_off:rodata_off+rodata_compressed_size] = compressed_rodata
    out_nso[data_off:data_off+data_compressed_size] = compressed_data

    print('text:')

    print('\nrodata:')
    print('Offset                    {}'.format(rodata_off))
    print('len(compressed_rodata)    {}'.format(len(compressed_rodata)))
    print('Original header size      {}'.format(rodata_compressed_size))

    print('\ndata:')
    print('Offset                    {}'.format(data_off))
    print('len(compressed_rodata)    {}'.format(len(compressed_data)))
    print('Original header size      {}'.format(data_compressed_size))

    with open(path_out, 'wb') as f:
        f.write(out_nso)

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print('Incorrect number of arguments')
        sys.exit(1)

    mode = sys.argv[1]

    if mode == 'extract':
        path_in = Path(sys.argv[2])
        path_out = Path(sys.argv[3])
        extract_nso(path_in, path_out)
    elif mode == 'repack':
        path_original = Path(sys.argv[2])
        path_patch = Path(sys.argv[3])
        path_out = Path(sys.argv[4])
        repack_nso(path_original, path_patch, path_out)