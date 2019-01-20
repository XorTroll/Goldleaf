using Goldtree.Lib;
using Microsoft.AspNetCore.Http;
using Microsoft.AspNetCore.Http.Features;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Goldtree.Usb
{
    class ResponseFeature : IHttpResponseFeature
    {
        private readonly IUsb usb;

        public int StatusCode { get; set; }
        public string ReasonPhrase { get; set; }
        public IHeaderDictionary Headers { get; set; } = new HeaderDictionary();
        public Stream Body { get; set; }

        public bool HasStarted { get; private set; }

        public ResponseFeature(IUsb usb)
        {
            this.usb = usb ?? throw new ArgumentNullException(nameof(usb));
        }

        public void OnCompleted(Func<object, Task> callback, object state)
        {
            Body = new UsbStream(usb);
        }

        public void OnStarting(Func<object, Task> callback, object state)
        {
            HasStarted = true;
            callback(state);
        }

        class UsbStream : Stream
        {
            private readonly IUsb usb;

            public UsbStream(IUsb usb) => this.usb = usb ?? throw new ArgumentNullException(nameof(usb));

            public override bool CanRead => false;

            public override bool CanSeek => false;

            public override bool CanWrite => true;

            public override long Length => 0;

            public override long Position { get; set; }

            public override void Flush() { }

            public override int Read(byte[] buffer, int offset, int count) => throw new NotSupportedException();

            public override long Seek(long offset, SeekOrigin origin) => throw new NotSupportedException();

            public override void SetLength(long value) { }

            public override void Write(byte[] buffer, int offset, int count) => usb.Write(buffer.Skip(offset).Take(count).ToArray());
        }
    }
}
