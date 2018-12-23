using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using libusbK;

namespace gtree
{
    // Command Ids for USB connections:
    public enum CommandId
    {
        ConnectionRequest, // Sent by Goldtree to Goldleaf to ask for connection
        ConnectionResponse, // Sent by Goldleaf to Goldtree like "accepting" the connection (to ensure the USB connection is with Goldleaf)
        NSPName, // Sent by Goldtree to Goldleaf with the name of the selected NSP, as a piece of information
        Start, // Sent by Goldleaf to Goldtree as an approval to start sending NSP contents

        // ... (more commands have to be added here)

        Finish, 
    }

    public static class UsbKWriteExtras
    {
        public static void Write(this UsbK USB, byte[] Data)
        {
            USB.WritePipe(0x1, Data, Data.Length, out _, IntPtr.Zero);
        }

        public static void Write(this UsbK USB, Command Data)
        {
            USB.Write(Data.AsData());
        }

        public static void Write(this UsbK USB, uint Data)
        {
            USB.Write(BitConverter.GetBytes(Data));
        }

        public static void Write(this UsbK USB, ulong Data)
        {
            USB.Write(BitConverter.GetBytes(Data));
        }

        public static void Write(this UsbK USB, string Data)
        {
            USB.Write(Encoding.UTF8.GetBytes(Data));
        }
    }

    public static class UsbKReadExtras
    {
        public static byte[] Read(this UsbK USB, int Length)
        {
            byte[] b = new byte[Length];
            USB.ReadPipe(0x81, b, Length, out _, IntPtr.Zero);
            return b;
        }

        public static Command Read(this UsbK USB)
        {
            Command cmd = new Command();
            USB.Read(out uint magic);
            USB.Read(out uint cmdid);
            cmd.Magic = magic;
            cmd.CommandId = (CommandId)cmdid;
            return cmd;
        }

        public static void Read(this UsbK USB, out uint Data)
        {
            Data = BitConverter.ToUInt32(USB.Read(4), 0);
        }

        public static void Read(this UsbK USB, out ulong Data)
        {
            Data = BitConverter.ToUInt64(USB.Read(8), 0);
        }

        public static void Read(this UsbK USB, out string Data, uint Length)
        {
            Data = Encoding.UTF8.GetString(USB.Read((int)Length));
        }
    }

    public class Command
    {
        public uint Magic { get; set; }
        public CommandId CommandId { get; set; }

        public static readonly uint GLUC = 0x43554c47;

        public Command()
        {
            Magic = GLUC;
        }

        public Command(CommandId CommandId)
        {
            Magic = GLUC;
            this.CommandId = CommandId;
        }

        public bool MagicOk()
        {
            return (Magic == GLUC);
        }

        public bool IsCommandId(CommandId Id)
        {
            return (CommandId == Id);
        }

        public byte[] AsData()
        {
            List<byte> fcmd = new List<byte>();
            byte[] emg = BitConverter.GetBytes(Magic);
            fcmd.AddRange(emg);
            fcmd.AddRange(BitConverter.GetBytes((uint)CommandId));
            return fcmd.ToArray();
        }
    }
}
