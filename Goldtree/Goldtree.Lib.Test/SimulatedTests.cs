using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace Goldtree.Lib.Test
{
    [TestClass]
    public class SimulatedTests
    {
        [TestMethod]
        public void EmptyTest()
        {
            CLI.IsArgumentsOnly = true;
            VirtualUsb usb = new VirtualUsb(
                new Command(CommandId.ConnectionResponse),
                new Command(CommandId.Start)
                );
            CLI cli = new CLI(usb);
            cli.ConnectUsb();
            cli.ConnectToGoldleaf();
            bool result = cli.SendFile(TestUtils.GetResource("empty.nsp"));
            Assert.IsFalse(result, "Sending an empty(/invalid) nsp must not succeed");
        }

        [TestMethod]
        public void EmptyTxtTest()
        {
            CLI.IsArgumentsOnly = true;
            VirtualUsb usb = new VirtualUsb(
                new Command(CommandId.ConnectionResponse),
                new Command(CommandId.Start)
                );
            CLI cli = new CLI(usb);
            cli.ConnectUsb();
            cli.ConnectToGoldleaf();
            bool result = cli.SendFile(TestUtils.GetResource("empty.txt"));
            Assert.IsFalse(result, "Sending an empty(/invalid) txt must not succeed");
        }
    }
}
