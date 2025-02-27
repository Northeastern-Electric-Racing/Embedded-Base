//
// Copyright (c) 2010-2025 Antmicro
//
// This file is licensed under the MIT License.
// Full license text is available in 'licenses/MIT.txt'.
//

using System;
using Antmicro.Renode.Core;
using Antmicro.Renode.Logging;
using Antmicro.Renode.Peripherals.I2C;
using Antmicro.Renode.Utilities;

namespace Antmicro.Renode.Peripherals.Sensors
{
    public class LSM6DSO_IMU_I2C : LSM6DSO_IMU_NER, II2CPeripheral
    {
        public LSM6DSO_IMU_I2C(IMachine machine) : base(machine)
        {
            
        }

        public byte[] Read(int count = 1)
        {
            var dequeued = false;
            switch (address)
            {
                case (byte)Registers.AccelerometerXLow:
                    dequeued = commonFifo.TryDequeueNewSample();
                    break;

                case (byte)Registers.GyroscopeXLow:
                    dequeued = commonFifo.TryDequeueNewSample();
                    break;

                case (byte)Registers.TemperatureLow:
                    // temperature data is not queued
                    commonFifo.TryDequeueNewSample();
                    break;
            }

            if (dequeued)
            {
                if (currentReportedFifoDepth > 0)
                {
                    currentReportedFifoDepth--;
                }
            }

            var result = RegistersCollection.Read(address);
            this.InfoLog("Reading register {1} (0x{1:X}) from device: 0x{0:X}", result, (Registers)address);
            TryIncrementAddress();

            return new byte[] { result };
        }


        public void Write(byte[] data)
        {
            this.Log(LogLevel.Info, "Written {0} bytes: {1}", data.Length, Misc.PrettyPrintCollectionHex(data));
            foreach (var b in data)
            {
                WriteByte(b);
            }
        }

        public void WriteByte(byte b)
        {
            switch (state)
            {
                case State.Idle:
                    address = BitHelper.GetValue(b, offset: 0, size: 7);
                    this.Log(LogLevel.Noisy, "Setting register address to {0} (0x{0:X})", (Registers)address);
                    state = State.Processing;
                    break;

                case State.Processing:
                    this.Log(LogLevel.Noisy, "Writing value 0x{0:X} to register {1} (0x{1:X})", b, (Registers)address);
                    RegistersCollection.Write(address, b);
                    TryIncrementAddress();
                    break;

                default:
                    throw new ArgumentException($"Unexpected state: {state}");
            }
        }

        private enum State
        {
            Idle,
            Processing
        }

        private uint currentReportedFifoDepth;
        private State state;
    }
}
