//
// Copyright (c) 2010-2024 Antmicro
//
// This file is licensed under the MIT License.
// Full license text is available in 'licenses/MIT.txt'.
//
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Threading;
using Antmicro.Migrant;
using Antmicro.Renode.Core;
using Antmicro.Renode.Core.CAN;
using Antmicro.Renode.Core.Structure;
using Antmicro.Renode.Exceptions;
using Antmicro.Renode.Logging;
using Antmicro.Renode.Utilities;
using Antmicro.Renode.Utilities.Packets;

namespace Antmicro.Renode.Peripherals.CAN
{
    public static class SocketCANBridgeNERExtensions
    {
        public static void CreateSocketCANBridgeNER(this IMachine machine, string name, string canInterfaceName = "vcan0", bool ensureFdFrames = false, bool ensureXlFrames = false)
        {
            var bridge = new SocketCANBridgeNER(canInterfaceName, ensureFdFrames, ensureXlFrames);
            machine.RegisterAsAChildOf(machine.SystemBus, bridge, NullRegistrationPoint.Instance);
            machine.SetLocalName(bridge, name);
        }
    }

    public class SocketCANBridgeNER : ICAN
    {
        public SocketCANBridgeNER(string canInterfaceName = "vcan0", bool ensureFdFrames = false, bool ensureXlFrames = false)
        {
            if (canInterfaceName.Length >= InterfaceRequest.InterfaceNameSize)
            {
                throw new ConstructionException($"Parameter '{nameof(canInterfaceName)}' is too long, name of CAN device \"{canInterfaceName}\" exceeds {InterfaceRequest.InterfaceNameSize - 1} bytes");
            }

            canSocket = LibCWrapper.Socket(ProtocolFamilyCan, SocketTypeRaw, ProtocolFamilyCanRaw);
            if (canSocket == -1)
            {
                throw new ConstructionException($"Could not create a socket: {LibCWrapper.GetLastError()}");
            }
            maximumTransmissionUnit = ClassicalSocketCANFrame.Size;

            if (TryEnableSocketOption(ensureFdFrames, SocketOptionFdFrames, "FD CAN frames"))
            {
                maximumTransmissionUnit = FlexibleSocketCANFrame.Size;
            }

            if (TryEnableSocketOption(ensureXlFrames, SocketOptionXlFrames, "XL CAN frames"))
            {
                maximumTransmissionUnit = XLSocketCANFrame.Size;
            }

            var request = new InterfaceRequest(canInterfaceName);
            if (LibCWrapper.Ioctl(canSocket, SocketConfigurationControlFindIndex, ref request) == -1)
            {
                throw new ConstructionException($"Could not get the \"{canInterfaceName}\" interface index: {LibCWrapper.GetLastError()}");
            }

            var address = new SocketAddressCan(request.InterfaceIndex);
            if (LibCWrapper.Bind(canSocket, address, Marshal.SizeOf(typeof(SocketAddressCan))) == -1)
            {
                throw new ConstructionException($"Binding the CAN socket to the interface failed: {LibCWrapper.GetLastError()}");
            }

            StartTransmitThread();
        }

        public void Reset()
        {
            // intentionally left empty
        }

        public void OnFrameReceived(CANMessageFrame message)
        {
            this.Log(LogLevel.Debug, "Received {0}", message);

            byte[] frame;
            try
            {
                // NER CHANGES: The endianess that the network encoding does doesnt seem to work, the Ids end up wrong by a number of bits, we manually adjust the id to make it right
                frame = message.ToSocketCAN(true);
                byte[] bts = BitConverter.GetBytes(message.Id);
                this.Log(LogLevel.Debug, "STDID: {0}", message.Id >> 21);
                uint stdId = message.Id >> 21;
                byte low = (byte)(stdId & 0xFF);
                byte high = (byte)((stdId >> 8) & 0x07);
                this.Log(LogLevel.Debug, "low: {0}", low);
                this.Log(LogLevel.Debug, "high: {0}", high);

                frame[0] = low;
                frame[1] = high;
            }
            catch (RecoverableException e)
            {
                this.Log(LogLevel.Warning, "Failed to create SocketCAN from {0}: {1}", message, e.Message);
                return;
            }

            this.Log(LogLevel.Debug, "FRAME: {0}", BitConverter.ToString(frame));

            var handle = GCHandle.Alloc(frame, GCHandleType.Pinned);
            try
            {
                if (!LibCWrapper.Write(canSocket, handle.AddrOfPinnedObject(), frame.Length))
                {
                    this.Log(LogLevel.Error, "Encountered an error while writing to the socket: {0}", LibCWrapper.GetLastError());
                }
            }
            finally
            {
                handle.Free();
            }
        }

        public event Action<CANMessageFrame> FrameSent;

        private bool TryEnableSocketOption(bool ensure, int option, string optionName)
        {
            var optionValue = 1;
            if (LibCWrapper.SetSocketOption(canSocket, SocketOptionLevelCanRaw, option, ref optionValue) != -1)
            {
                return true;
            }

            var error = Marshal.GetLastWin32Error();
            if (ensure || error != ProtocolNotAvailableError)
            {
                throw new ConstructionException($"Could not enable {optionName} on the socket: {LibCWrapper.Strerror(error)}");
            }

            this.Log(LogLevel.Info, "Attempted to enable {0}, but it's not supported by this host", optionName);
            return false;
        }

        private void StartTransmitThread()
        {
            cancellationTokenSource = new CancellationTokenSource();
            thread = new Thread(() => TransmitLoop(cancellationTokenSource.Token))
            {
                Name = $"{nameof(SocketCANBridgeNER)} transmit thread",
                IsBackground = true
            };
            thread.Start();
        }

        private void TransmitLoop(CancellationToken token)
        {
            Func<bool> isCancellationRequested = () => token.IsCancellationRequested;
            var buffer = new List<byte>();
            while (true)
            {
                if (maximumTransmissionUnit - buffer.Count <= 0)
                {
                    throw new Exception("Unreachable");
                }

                var data = LibCWrapper.Read(canSocket, maximumTransmissionUnit - buffer.Count, ReadSocketTimeout, isCancellationRequested);
                if (token.IsCancellationRequested)
                {
                    return;
                }
                if (data == null)
                {
                    continue;
                }

                buffer.AddRange(data);

                if (!buffer.TryDecodeAsSocketCANFrame(out var frame, false))
                {
                    // not enough bytes
                    continue;
                }
                buffer.RemoveRange(0, frame.Size);
                this.Log(LogLevel.Noisy, "Frame read from socket: {0}", frame);

                if (!CANMessageFrame.TryFromSocketCAN(frame, out var message))
                {
                    this.Log(LogLevel.Warning, "Failed to convert SocketCAN frame to CANMessageFrame");
                    continue;
                }

                this.Log(LogLevel.Debug, "Transmitting {0}", message);
                FrameSent?.Invoke(message);
            }
        }

        private int canSocket;
        [Transient]
        private CancellationTokenSource cancellationTokenSource;
        [Transient]
        private Thread thread;

        private int maximumTransmissionUnit;

        // PF_CAN
        private const int ProtocolFamilyCan = 29;
        // SOCK_RAW
        private const int SocketTypeRaw = 3;
        // CAN_RAW
        private const int ProtocolFamilyCanRaw = 1;
        // SIOCGIFINDEX
        private const int SocketConfigurationControlFindIndex = 0x8933;
        // SOL_CAN_RAW
        private const int SocketOptionLevelCanRaw = 100 + ProtocolFamilyCanRaw;
        // CAN_RAW_FD_FRAMES
        private const int SocketOptionFdFrames = 5;
        // CAN_RAW_XL_FRAMES
        private const int SocketOptionXlFrames = 7;
        // ENOPROTOOPT
        private const int ProtocolNotAvailableError = 92;
        private const int ReadSocketTimeout = 1000;
    }
}
