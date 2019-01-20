using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Goldtree
{
    public class NSPInfo
    {
        public string Name { get; set; }
        public long Offset { get; set; }
        public long Size { get; set; }

        public NSPInfo()
        {

        }

        public NSPInfo(string name, long offset, long size)
        {
            Name = name ?? throw new ArgumentNullException(nameof(name));
            Offset = offset;
            Size = size;
        }
    }
}
