using System;
using System.Collections.Generic;
using System.Text;

namespace Goldtree.Lib
{
    public interface IUsb
    {
        bool Connect(int vid, int pid);
        
        void Write(uint value);
        void Write(byte[] value);
        
        uint ReadInt32();
        byte[] ReadBytes(uint length);
    }
}
