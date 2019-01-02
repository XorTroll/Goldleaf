using System;
using System.Collections.Generic;
using System.Text;

namespace Goldtree.Lib
{
    public interface IUsb
    {
        void Connect(string deviceId);

        void Write(Command command);
        void Write(uint value);
        void Write(ulong value);
        void Write(string value);
        void Write(byte[] value);


        Command Read();
        uint ReadInt32();

    }
}
