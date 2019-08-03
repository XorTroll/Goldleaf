#region Copyright (c) Travis Robinson

// Copyright (c) 2012 Travis Robinson <libusbdotnet@gmail.com>
// All rights reserved.
//
// C# libusbK Bindings
// Auto-generated on: 04.28.2011
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
// IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
// TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
// PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL TRAVIS LEE ROBINSON
// BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
// THE POSSIBILITY OF SUCH DAMAGE.

#endregion Copyright (c) Travis Robinson

using System;
using System.Runtime.InteropServices;

namespace libusbK
{
    public static class Stuff
    {
        public static string path;
        public const int len = 256;
        public const string dll = "libusbK.dll";
    }

    internal static class AllKFunctions
    {
        [UnmanagedFunctionPointer(CallingConvention.Winapi, CharSet = CharSet.Ansi, SetLastError = true)]
        public delegate bool LibK_LoadDriverAPIDelegate([Out] out KUSB_DRIVER_API DriverAPI, int DriverID);

        [UnmanagedFunctionPointer(CallingConvention.Winapi, CharSet = CharSet.Ansi, SetLastError = true)]
        public delegate bool LstK_InitExDelegate([Out] out IntPtr DeviceList, int Flags, [In] ref KLST_PATTERN_MATCH PatternMatch);

        [UnmanagedFunctionPointer(CallingConvention.Winapi, CharSet = CharSet.Ansi, SetLastError = true)]
        public delegate bool LstK_MoveNextDelegate([In] IntPtr DeviceList, [Out] out KLST_DEVINFO_HANDLE DeviceInfo);

        public static LibK_LoadDriverAPIDelegate LibK_LoadDriverAPI;
        public static LstK_InitExDelegate LstK_InitEx;
        public static LstK_MoveNextDelegate LstK_MoveNext;

        private static readonly IntPtr mModuleLibusbK;

        static AllKFunctions()
        {
            if (string.IsNullOrEmpty(Stuff.path))
                mModuleLibusbK = LoadLibraryEx(Stuff.dll, IntPtr.Zero, 0);
            else
                mModuleLibusbK = LoadLibraryEx(Stuff.path, IntPtr.Zero, 8);
            if (mModuleLibusbK == IntPtr.Zero)
                throw new DllNotFoundException("libusbK driver not found.");
            LoadDynamicFunctions();
        }

        [DllImport("kernel32.dll", CharSet = CharSet.Ansi, ExactSpelling = true, SetLastError = true)]
        private static extern IntPtr GetProcAddress(IntPtr hModule, string procName);

        private static void LoadDynamicFunctions()
        {
            LibK_LoadDriverAPI = (LibK_LoadDriverAPIDelegate)Marshal.GetDelegateForFunctionPointer(GetProcAddress(mModuleLibusbK, "LibK_LoadDriverAPI"), typeof(LibK_LoadDriverAPIDelegate));
            LstK_InitEx = (LstK_InitExDelegate)Marshal.GetDelegateForFunctionPointer(GetProcAddress(mModuleLibusbK, "LstK_InitEx"), typeof(LstK_InitExDelegate));
            LstK_MoveNext = (LstK_MoveNextDelegate)Marshal.GetDelegateForFunctionPointer(GetProcAddress(mModuleLibusbK, "LstK_MoveNext"), typeof(LstK_MoveNextDelegate));
        }

        [DllImport("kernel32.dll")]
        private static extern IntPtr LoadLibraryEx(string lpFileName, IntPtr hReservedNull, int dwFlags);
    }

    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi, Pack = 1)]
    public struct WINUSB_SETUP_PACKET
    {
        public byte RequestType;
        public byte Request;
        public ushort Value;
        public ushort Index;
        public ushort Length;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct KLST_DEVINFO_HANDLE
    {
        public IntPtr Pointer;

        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
        private struct KLST_DEVINFO_MAP
        {
            public readonly int Vid;
            public readonly int Pid;
            public readonly int MI;
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = Stuff.len)]
            public readonly string InstanceID;
            private readonly int DriverID;
        }

        public int DriverID => Marshal.ReadInt32(Pointer, Marshal.OffsetOf(typeof(KLST_DEVINFO_MAP), "DriverID").ToInt32());
    }

    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi, Size = 1024)]
    public struct KLST_PATTERN_MATCH
    {
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = Stuff.len)]
        public string DeviceID;

        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = Stuff.len)]
        public string DeviceInterfaceGUID;

        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = Stuff.len)]
        public string ClassGUID;
    }

    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    public struct KUSB_DRIVER_API_INFO
    {
        public int DriverID;
        public int FunctionCount;
    }

    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi, Size = 512)]
    public struct KUSB_DRIVER_API
    {
        public KUSB_DRIVER_API_INFO Info;

        [MarshalAs(UnmanagedType.FunctionPtr)] public KUSB_InitDelegate Init;
        [MarshalAs(UnmanagedType.FunctionPtr)] public KUSB_FreeDelegate Free;
        [MarshalAs(UnmanagedType.FunctionPtr)] public KUSB_ClaimInterfaceDelegate ClaimInterface;
        [MarshalAs(UnmanagedType.FunctionPtr)] public KUSB_ReleaseInterfaceDelegate ReleaseInterface;
        [MarshalAs(UnmanagedType.FunctionPtr)] public KUSB_SetAltInterfaceDelegate SetAltInterface;
        [MarshalAs(UnmanagedType.FunctionPtr)] public KUSB_GetAltInterfaceDelegate GetAltInterface;
        [MarshalAs(UnmanagedType.FunctionPtr)] public KUSB_GetDescriptorDelegate GetDescriptor;
        [MarshalAs(UnmanagedType.FunctionPtr)] public KUSB_ControlTransferDelegate ControlTransfer;
        [MarshalAs(UnmanagedType.FunctionPtr)] public KUSB_SetPowerPolicyDelegate SetPowerPolicy;
        [MarshalAs(UnmanagedType.FunctionPtr)] public KUSB_GetPowerPolicyDelegate GetPowerPolicy;
        [MarshalAs(UnmanagedType.FunctionPtr)] public KUSB_SetConfigurationDelegate SetConfiguration;
        [MarshalAs(UnmanagedType.FunctionPtr)] public KUSB_GetConfigurationDelegate GetConfiguration;
        [MarshalAs(UnmanagedType.FunctionPtr)] public KUSB_ResetDeviceDelegate ResetDevice;
        [MarshalAs(UnmanagedType.FunctionPtr)] public KUSB_InitializeDelegate Initialize;
        [MarshalAs(UnmanagedType.FunctionPtr)] public KUSB_SelectInterfaceDelegate SelectInterface;
        [MarshalAs(UnmanagedType.FunctionPtr)] public KUSB_GetAssociatedInterfaceDelegate GetAssociatedInterface;
        [MarshalAs(UnmanagedType.FunctionPtr)] public KUSB_CloneDelegate Clone;
        [MarshalAs(UnmanagedType.FunctionPtr)] public KUSB_QueryInterfaceSettingsDelegate QueryInterfaceSettings;
        [MarshalAs(UnmanagedType.FunctionPtr)] public KUSB_QueryDeviceInformationDelegate QueryDeviceInformation;
        [MarshalAs(UnmanagedType.FunctionPtr)] public KUSB_SetCurrentAlternateSettingDelegate SetCurrentAlternateSetting;
        [MarshalAs(UnmanagedType.FunctionPtr)] public KUSB_GetCurrentAlternateSettingDelegate GetCurrentAlternateSetting;
        [MarshalAs(UnmanagedType.FunctionPtr)] public KUSB_QueryPipeDelegate QueryPipe;
        [MarshalAs(UnmanagedType.FunctionPtr)] public KUSB_SetPipePolicyDelegate SetPipePolicy;
        [MarshalAs(UnmanagedType.FunctionPtr)] public KUSB_GetPipePolicyDelegate GetPipePolicy;
        [MarshalAs(UnmanagedType.FunctionPtr)] public KUSB_ReadPipeDelegate ReadPipe;
        [MarshalAs(UnmanagedType.FunctionPtr)] public KUSB_WritePipeDelegate WritePipe;
    }

    [UnmanagedFunctionPointer(CallingConvention.Winapi, CharSet = CharSet.Ansi, SetLastError = true)]
    public delegate bool KUSB_InitDelegate([Out] out IntPtr InterfaceHandle, [In] KLST_DEVINFO_HANDLE DevInfo);

    [UnmanagedFunctionPointer(CallingConvention.Winapi, CharSet = CharSet.Ansi, SetLastError = true)]
    public delegate bool KUSB_FreeDelegate([In] IntPtr InterfaceHandle);

    [UnmanagedFunctionPointer(CallingConvention.Winapi, CharSet = CharSet.Ansi, SetLastError = true)]
    public delegate bool KUSB_ClaimInterfaceDelegate([In] IntPtr InterfaceHandle, byte NumberOrIndex, bool IsIndex);

    [UnmanagedFunctionPointer(CallingConvention.Winapi, CharSet = CharSet.Ansi, SetLastError = true)]
    public delegate bool KUSB_ReleaseInterfaceDelegate([In] IntPtr InterfaceHandle, byte NumberOrIndex, bool IsIndex);

    [UnmanagedFunctionPointer(CallingConvention.Winapi, CharSet = CharSet.Ansi, SetLastError = true)]
    public delegate bool KUSB_SetAltInterfaceDelegate([In] IntPtr InterfaceHandle, byte NumberOrIndex, bool IsIndex, byte AltSettingNumber);

    [UnmanagedFunctionPointer(CallingConvention.Winapi, CharSet = CharSet.Ansi, SetLastError = true)]
    public delegate bool KUSB_GetAltInterfaceDelegate([In] IntPtr InterfaceHandle, byte NumberOrIndex, bool IsIndex, out byte AltSettingNumber);

    [UnmanagedFunctionPointer(CallingConvention.Winapi, CharSet = CharSet.Ansi, SetLastError = true)]
    public delegate bool KUSB_GetDescriptorDelegate([In] IntPtr InterfaceHandle, byte DescriptorType, byte Index, ushort LanguageID, IntPtr Buffer, int BufferLength, out int LengthTransferred);

    [UnmanagedFunctionPointer(CallingConvention.Winapi, CharSet = CharSet.Ansi, SetLastError = true)]
    public delegate bool KUSB_ControlTransferDelegate([In] IntPtr InterfaceHandle, WINUSB_SETUP_PACKET SetupPacket, IntPtr Buffer, int BufferLength, out int LengthTransferred, IntPtr Overlapped);

    [UnmanagedFunctionPointer(CallingConvention.Winapi, CharSet = CharSet.Ansi, SetLastError = true)]
    public delegate bool KUSB_SetPowerPolicyDelegate([In] IntPtr InterfaceHandle, int PolicyType, int ValueLength, IntPtr Value);

    [UnmanagedFunctionPointer(CallingConvention.Winapi, CharSet = CharSet.Ansi, SetLastError = true)]
    public delegate bool KUSB_GetPowerPolicyDelegate([In] IntPtr InterfaceHandle, int PolicyType, ref int ValueLength, IntPtr Value);

    [UnmanagedFunctionPointer(CallingConvention.Winapi, CharSet = CharSet.Ansi, SetLastError = true)]
    public delegate bool KUSB_SetConfigurationDelegate([In] IntPtr InterfaceHandle, byte ConfigurationNumber);

    [UnmanagedFunctionPointer(CallingConvention.Winapi, CharSet = CharSet.Ansi, SetLastError = true)]
    public delegate bool KUSB_GetConfigurationDelegate([In] IntPtr InterfaceHandle, out byte ConfigurationNumber);

    [UnmanagedFunctionPointer(CallingConvention.Winapi, CharSet = CharSet.Ansi, SetLastError = true)]
    public delegate bool KUSB_ResetDeviceDelegate([In] IntPtr InterfaceHandle);

    [UnmanagedFunctionPointer(CallingConvention.Winapi, CharSet = CharSet.Ansi, SetLastError = true)]
    public delegate bool KUSB_InitializeDelegate(IntPtr DeviceHandle, [Out] out IntPtr InterfaceHandle);

    [UnmanagedFunctionPointer(CallingConvention.Winapi, CharSet = CharSet.Ansi, SetLastError = true)]
    public delegate bool KUSB_SelectInterfaceDelegate([In] IntPtr InterfaceHandle, byte NumberOrIndex, bool IsIndex);

    [UnmanagedFunctionPointer(CallingConvention.Winapi, CharSet = CharSet.Ansi, SetLastError = true)]
    public delegate bool KUSB_GetAssociatedInterfaceDelegate([In] IntPtr InterfaceHandle, byte AssociatedInterfaceIndex, [Out] out IntPtr AssociatedInterfaceHandle);

    [UnmanagedFunctionPointer(CallingConvention.Winapi, CharSet = CharSet.Ansi, SetLastError = true)]
    public delegate bool KUSB_CloneDelegate([In] IntPtr InterfaceHandle, [Out] out IntPtr DstInterfaceHandle);

    [UnmanagedFunctionPointer(CallingConvention.Winapi, CharSet = CharSet.Ansi, SetLastError = true)]
    public delegate bool KUSB_QueryInterfaceSettingsDelegate([In] IntPtr a, byte AltSettingIndex, [Out] out ulong b, out byte c);

    [UnmanagedFunctionPointer(CallingConvention.Winapi, CharSet = CharSet.Ansi, SetLastError = true)]
    public delegate bool KUSB_QueryDeviceInformationDelegate([In] IntPtr a, int InformationType, ref int BufferLength, IntPtr Buffer);

    [UnmanagedFunctionPointer(CallingConvention.Winapi, CharSet = CharSet.Ansi, SetLastError = true)]
    public delegate bool KUSB_SetCurrentAlternateSettingDelegate([In] IntPtr InterfaceHandle, byte AltSettingNumber);

    [UnmanagedFunctionPointer(CallingConvention.Winapi, CharSet = CharSet.Ansi, SetLastError = true)]
    public delegate bool KUSB_GetCurrentAlternateSettingDelegate([In] IntPtr InterfaceHandle, out byte AltSettingNumber);

    [UnmanagedFunctionPointer(CallingConvention.Winapi, CharSet = CharSet.Ansi, SetLastError = true)]
    public delegate bool KUSB_QueryPipeDelegate([In] IntPtr InterfaceHandle, byte AltSettingNumber, byte PipeIndex, [Out] out ulong PipeInfo);

    [UnmanagedFunctionPointer(CallingConvention.Winapi, CharSet = CharSet.Ansi, SetLastError = true)]
    public delegate bool KUSB_SetPipePolicyDelegate([In] IntPtr InterfaceHandle, byte PipeID, int PolicyType, int ValueLength, IntPtr Value);

    [UnmanagedFunctionPointer(CallingConvention.Winapi, CharSet = CharSet.Ansi, SetLastError = true)]
    public delegate bool KUSB_GetPipePolicyDelegate([In] IntPtr InterfaceHandle, byte PipeID, int PolicyType, ref int ValueLength, IntPtr Value);

    [UnmanagedFunctionPointer(CallingConvention.Winapi, CharSet = CharSet.Ansi, SetLastError = true)]
    public delegate bool KUSB_ReadPipeDelegate([In] IntPtr InterfaceHandle, byte PipeID, IntPtr Buffer, int BufferLength, out int LengthTransferred, IntPtr Overlapped);

    [UnmanagedFunctionPointer(CallingConvention.Winapi, CharSet = CharSet.Ansi, SetLastError = true)]
    public delegate bool KUSB_WritePipeDelegate([In] IntPtr InterfaceHandle, byte PipeID, IntPtr Buffer, int BufferLength, out int LengthTransferred, IntPtr Overlapped);

    public class LstK
    {
        protected IntPtr mHandleStruct;

        public LstK(int Flags, ref KLST_PATTERN_MATCH PatternMatch)
        {
            if(!AllKFunctions.LstK_InitEx(out mHandleStruct, Flags, ref PatternMatch)) throw new Exception();
        }

        public virtual bool MoveNext(out KLST_DEVINFO_HANDLE DeviceInfo) =>
            AllKFunctions.LstK_MoveNext(mHandleStruct, out DeviceInfo);
    }

    public class UsbK
    {
        protected KUSB_DRIVER_API driverAPI;
        protected IntPtr mHandleStruct;

        public UsbK(KLST_DEVINFO_HANDLE DevInfo)
        {
            var success = AllKFunctions.LibK_LoadDriverAPI(out driverAPI, DevInfo.DriverID);
            if (!success) throw new Exception();
            success = driverAPI.Init(out mHandleStruct, DevInfo);
            if (!success) throw new Exception();
        }

        public virtual bool ControlTransfer(WINUSB_SETUP_PACKET SetupPacket, Array Buffer, int BufferLength, out int LengthTransferred, IntPtr Overlapped) =>
            driverAPI.ControlTransfer(mHandleStruct, SetupPacket, Marshal.UnsafeAddrOfPinnedArrayElement(Buffer, 0), BufferLength, out LengthTransferred, Overlapped);

        public virtual bool ReadPipe(byte PipeID, Array Buffer, int BufferLength, out int LengthTransferred, IntPtr Overlapped) =>
            driverAPI.ReadPipe(mHandleStruct, PipeID, Marshal.UnsafeAddrOfPinnedArrayElement(Buffer, 0), BufferLength, out LengthTransferred, Overlapped);

        public virtual bool SetAltInterface(byte NumberOrIndex, bool IsIndex, byte AltSettingNumber) =>
            driverAPI.SetAltInterface(mHandleStruct, NumberOrIndex, IsIndex, AltSettingNumber);

        public virtual bool WritePipe(byte PipeID, Array Buffer, int BufferLength, out int LengthTransferred, IntPtr Overlapped) => 
            driverAPI.WritePipe(mHandleStruct, PipeID, Marshal.UnsafeAddrOfPinnedArrayElement(Buffer, 0), BufferLength, out LengthTransferred, Overlapped);     
    }
}