# This is simply a script I used to quickly test the desired interactions 
# between the PC and the Switch. It is NOT useful for most people!
#
# There will be a few differences between this and the actual implementation used by Tinfoil
# due to the API differences documented here http://pycurl.io/docs/latest/unimplemented.html.
# However, pycurl is still close enough to make it worth doing this as a reference

import pycurl
import socket
from io import BytesIO

class HTTPHeader:
    def __init__(self, url):
        self.values = {}
        self.url = url

        curl = pycurl.Curl()
        curl.setopt(curl.URL, self.url)
        curl.setopt(curl.NOBODY, True) 
        curl.setopt(curl.SSL_VERIFYPEER, False)
        curl.setopt(curl.HEADERFUNCTION, self.parse_header_line)
        curl.perform()

        http_code = curl.getinfo(curl.HTTP_CODE)
        curl.close()

        if http_code != 200 and http_code != 204:
            raise Exception('Unexpected HTTP code when retrieving header: {}'.format(http_code))

    def parse_header_line(self, line):
        header_line = line.decode('iso-8859-1')

        if ':' not in header_line:
            return

        name, value = header_line.split(':', 1)
        
        # Remove newlines and whitespace
        name = name.strip()
        value = value.strip()

        name = name.lower()
        self.values[name] = value

class HTTPDownloader:
    def __init__(self, url):
        self.url = url
        self.header = HTTPHeader(self.url)
        self.check_ranges_supported()

    def check_ranges_supported(self):
        # Check whether ranges are explicitly accepted
        if 'accept-ranges' in self.header.values:
            if self.header.values['accept-ranges'] == 'bytes':
                print('Header specifies ranges supported')
                self.ranges_supported = True
                return
            else:
                print('Header specifies ranges unsupported')
                self.ranges_supported = False
                return

        # See if the server supports ranges anyway
        curl = pycurl.Curl()
        curl.setopt(curl.URL, self.url)
        curl.setopt(curl.NOBODY, True) # We only want the header
        curl.setopt(curl.SSL_VERIFYPEER, False)
        curl.setopt(curl.RANGE, '0-0') # First byte
        curl.perform()

        http_code = curl.getinfo(curl.HTTP_CODE)
        curl.close()

        if http_code == 206: # Byte ranges are supported
            print('Ranges supported by response code')
            self.ranges_supported = True
            return
        else:
            print('Ranges unsupported by response code')
            self.ranges_supported = False
            return

    def request_range(self, url, start_off, end_off):
        if not self.ranges_supported:
            raise Exception('Attempted to request range when ranges are unsupported by the server')

        buffer = BytesIO()
        curl = pycurl.Curl()
        curl.setopt(curl.URL, url)
        curl.setopt(curl.WRITEDATA, buffer)
        curl.setopt(curl.SSL_VERIFYPEER, False)
        curl.setopt(curl.RANGE, '{}-{}'.format(start_off, end_off))
        curl.perform()

        http_code = curl.getinfo(curl.HTTP_CODE)
        curl.close()

        if http_code != 206:
            raise Exception('Failed to request range! Response code is {}'.format(http_code))

        return buffer.getvalue()

MAX_URL_SIZE = 1024
MAX_URLS = 256
HOST_IP = [(s.connect(('8.8.8.8', 53)), s.getsockname()[0], s.close()) for s in [socket.socket(socket.AF_INET, socket.SOCK_DGRAM)]][0][1]

def wait_for_urls(connection):
    size = int.from_bytes(connection.recv(4), byteorder='big', signed=False)
    print('URL Buffer size: 0x{:x}'.format(size), flush=True)

    if size > MAX_URL_SIZE * MAX_URLS:
        raise Exception('URL size {} is too large!'.format(size))

    urls = list(filter(None, connection.recv(size).decode('utf-8').split('\n'))) # Each URL should be its own entry in the list, and empty strings shouldn't be included
    print('URLS: {}'.format(urls))
    return urls

try:
    print('Creating URL socket', flush=True)
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        print('Binding URL socket', flush=True)
        sock.bind((HOST_IP, 2000))

        print('Listening on URL socket', flush=True)
        sock.listen(10)

        print('Waiting for a connection...', flush=True)
        connection, address = sock.accept()
        print('Connection accepted', flush=True)

        print('Host IP is: {}'.format(HOST_IP))
        urls = wait_for_urls(connection)

        print('Downloading from URLs', flush=True)
        for url in urls:
            print('Downloading from {}'.format(url), flush=True)
            downloader = HTTPDownloader(url)

            if downloader.ranges_supported:
                # From this point, Tinfoil will be calling request_range for the nsp base header, the full header,
                # then all of the NCAs, starting with the cnmt nca then the rest.
                buf = downloader.request_range(url, 0, 10)

                print(buf)
            else:
                print('Ranges are unsupported.')

        connection.send(b'\x00') # Send 1 byte ack to close the server

except Exception as e:
    print(e)

