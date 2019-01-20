using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace Goldtree.Lib.LibUsb.Tests
{
    [TestClass]
    public class EchoTests
    {
        [TestMethod]
        public void IncrementalEchoTest()
        {
            Usb usb = new Usb();
            usb.Connect(0x057E, 0x3000);

            byte next = 0;
            byte[] payload = new byte[0x1000];
            for(int i = 0; i < 0x1000; i++)
            {
                byte start = next;
                for (int j = 0; j < payload.Length; j++)
                {
                    payload[j] = next;
                    if (j == byte.MaxValue)
                        j = 0;
                    else
                        j++;
                }

                usb.Write(new Command(CommandId.Echo));
                usb.Write((uint)payload.Length);
                usb.Write(payload);

                Command command = usb.Read();
                Assert.IsNotNull(command);
                Assert.AreEqual(CommandId.Echo, command.CommandId);

                for (int j = 0; j < payload.Length; j++)
                {
                    Assert.AreEqual(next, usb.ReadByte());
                    if (next == byte.MaxValue)
                        next = 0;
                    else
                        next++;
                }
            }
        }
    }
}
