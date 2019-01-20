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
    class RequestFeature : IHttpRequestFeature, IDisposable
    {
        public string Protocol { get; set; }
        public string Scheme { get; set; }
        public string Method { get; set; }
        public string PathBase { get; set; }
        public string Path { get; set; }
        public string QueryString { get; set; }
        public string RawTarget { get; set; }
        public IHeaderDictionary Headers { get; set; }
        public Stream Body { get; set; }

        public RequestFeature(in string method, in string path, in Stream body)
        {
            Protocol = "USB";
            Scheme = "USB";
            Method = method ?? throw new ArgumentNullException(nameof(method));
            PathBase = string.Empty;
            Path = path ?? throw new ArgumentNullException(nameof(path));
            QueryString = string.Empty;
            RawTarget = path ?? throw new ArgumentNullException(nameof(path));
            Headers = new HeaderDictionary();
            Body = body ?? throw new ArgumentNullException(nameof(body));
        }

        public void Dispose()
        {
            Body?.Dispose();
        }
    }
}
