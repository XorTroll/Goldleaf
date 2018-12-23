using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using libusbK;

namespace gleaf.tree
{
    public enum CommandType
    {
        Request,
        Response,
    }

    public enum CommandId
    {
        ConnectionRequest,
        ConnectionResponse,
        Content,
        Finish,
    }

    public static class UsbKExtra
    {
        public static void Write(this UsbK USB, byte[] Data)
        {
            USB.WritePipe(1, Data, Data.Length, out _, IntPtr.Zero);
        }

        public static byte[] Read(this UsbK USB, int Length)
        {
            byte[] b = new byte[0x1000];
            USB.ReadPipe(1, b, Length, out _, IntPtr.Zero);
            return b;
        }
    }

    public class Command
    {
        public uint Magic { get; set; }
        public byte CommandId { get; set; }
        public byte[] Padding { get; set; }

        public Command()
        {
            Magic = Commands.GLUC;
        }

        public Command(byte CommandId)
        {
            Magic = Commands.GLUC;
            this.CommandId = CommandId;
        }
    }

    public static class Commands
    {
        public static readonly uint GLUC = 0x43554c47;

        public static Command ReceiveCommand(UsbK USB)
        {
            Command cmd = new Command();
            byte[] rcmd = USB.Read(8);
            byte[] mg = new byte[]{ rcmd[0], rcmd[1], rcmd[2], rcmd[3] };
            byte cmdid = rcmd[4];
            cmd.Magic = BitConverter.ToUInt32(mg, 0);
            cmd.CommandId = rcmd[4];
            cmd.Padding = new byte[]{ rcmd[5], rcmd[6], rcmd[7] };
            return cmd;
        }

        public static void SendCommand(UsbK USB, Command Command)
        {
            List<byte> fcmd = new List<byte>();
            byte[] emg = BitConverter.GetBytes(Command.Magic);
            fcmd.AddRange(emg);
            fcmd.Add(Command.CommandId);
            fcmd.Add(0);
            fcmd.Add(0);
            fcmd.Add(0);
            USB.Write(fcmd.ToArray());
        }
    }
}
