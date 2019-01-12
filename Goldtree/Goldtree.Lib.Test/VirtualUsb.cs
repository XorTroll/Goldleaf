using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Goldtree.Lib.Test
{
    class VirtualUsb : IUsb
    {
        private bool connected = false;

        private readonly Action<int, object> writeHandler;
        private readonly Func<int, object> readHandler;

        private object[] input;
        private List<object> output = new List<object>();

        private int inputCounter = 0;
        private int outputCounter = 0;

        public object[] Output { get => output.ToArray(); }

        public VirtualUsb(Action<int, object> writeHandler, Func<int, object> readHandler)
        {
            this.writeHandler = writeHandler;
            this.readHandler = readHandler;
        }

        public VirtualUsb(IEnumerable<object> input) : this(input.ToArray()) { }
        public VirtualUsb() : this(new object[0]) { }

        public VirtualUsb(params object[] input)
        {
            this.input = input;
            this.readHandler = index =>
            {
                Assert.IsTrue(index < this.input.Length, "Tried to read an input that doesn't exist");
                return this.input[index];
            };
            this.writeHandler = (_, value) => output.Add(value);
        }

        public void Connect(int vid, int pid)
        {
            Assert.IsFalse(connected, "Usb is already connected.");
            connected = true;
        }

        public Command Read()
        {
            Assert.IsTrue(connected, "Usb was read without being connected");
            object obj = readHandler(inputCounter++);
            Assert.IsInstanceOfType(obj, typeof(Command), $"Read a command but received {obj.GetType()}");
            return (Command)obj;
        }

        public uint ReadInt32()
        {
            Assert.IsTrue(connected, "Usb was read without being connected");
            object obj = readHandler(inputCounter++);
            Assert.IsInstanceOfType(obj, typeof(uint), $"Read a uint but received {obj.GetType()}");
            return (uint)obj;
        }

        public void Write(Command command)
        {
            Assert.IsTrue(connected, "Usb was written without being connected");
            writeHandler(outputCounter++, command);
        }

        public void Write(uint value)
        {
            Assert.IsTrue(connected, "Usb was written without being connected");
            writeHandler(outputCounter++, value);
        }

        public void Write(ulong value)
        {
            Assert.IsTrue(connected, "Usb was written without being connected");
            writeHandler(outputCounter++, value);
        }

        public void Write(string value)
        {
            Assert.IsTrue(connected, "Usb was written without being connected");
            writeHandler(outputCounter++, value);
        }

        public void Write(byte[] value)
        {
            Assert.IsTrue(connected, "Usb was written without being connected");
            writeHandler(outputCounter++, value);
        }
    }
}
