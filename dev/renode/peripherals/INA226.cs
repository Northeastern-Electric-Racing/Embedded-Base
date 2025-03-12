using System;
using System.Collections.Generic;
using Antmicro.Renode.Core.Structure.Registers;
using Antmicro.Renode.Logging;

namespace Antmicro.Renode.Peripherals.I2C
{
    public class INA226 : II2CPeripheral
    {
        public INA226()
        {
            registers = DefineRegisters();
        }

        private WordRegisterCollection DefineRegisters()
        {
            var registersMap = new Dictionary<long, WordRegister> {
                {
                    (long)Registers.Configuration,
                    new WordRegister(this)
                        .WithValueField(0, 16, name: "CONFIGURATION", valueProviderCallback: _ => {
                            return configuration;
                        }, writeCallback: (_, val) => {
                            int resetBit = (int)((val >> 15) & 1);
                            int averaging = ExtractValue(9, 3, (long)val);
                            int bsct = ExtractValue(6, 3, (long)val);
                            int svct = ExtractValue(3, 3, (long)val);
                            int opMode = ExtractValue(0, 3, (long)val);
                            if (resetBit == 1) {
                                Reset();
                            } else {
                                configuration = (ushort)val;

                                switch (averaging) {
                                    case 0:
                                        numAverages = 0;
                                        break;
                                    case 1:
                                        numAverages = 4;
                                        break;
                                    case 2:
                                        numAverages = 16;
                                        break;
                                    case 3:
                                        numAverages = 64;
                                        break;
                                    case 4:
                                        numAverages = 128;
                                        break;
                                    case 5:
                                        numAverages = 256;
                                        break;
                                    case 6:
                                        numAverages = 512;
                                        break;
                                    case 7:
                                        numAverages = 1024;
                                        break;
                                }
                            }

                            switch (bsct) {
                                    case 0:
                                        busVoltageConversionTime = 140;
                                        break;
                                    case 1:
                                        busVoltageConversionTime = 204;
                                        break;
                                    case 2:
                                        busVoltageConversionTime = 332;
                                        break;
                                    case 3:
                                        busVoltageConversionTime = 588;
                                        break;
                                    case 4:
                                        busVoltageConversionTime = 1100;
                                        break;
                                    case 5:
                                        busVoltageConversionTime = 2116;
                                        break;
                                    case 6:
                                        busVoltageConversionTime = 4156;
                                        break;
                                    case 7:
                                        busVoltageConversionTime = 8244;
                                        break;
                            }

                            switch (svct) {
                                    case 0:
                                        shortVoltageConversionTime = 140;
                                        break;
                                    case 1:
                                        shortVoltageConversionTime = 204;
                                        break;
                                    case 2:
                                        shortVoltageConversionTime = 332;
                                        break;
                                    case 3:
                                        shortVoltageConversionTime = 588;
                                        break;
                                    case 4:
                                        shortVoltageConversionTime = 1100;
                                        break;
                                    case 5:
                                        shortVoltageConversionTime = 2216;
                                        break;
                                    case 6:
                                        shortVoltageConversionTime = 4156;
                                        break;
                                    case 7:
                                        shortVoltageConversionTime = 8244;
                                        break;
                            }

                            switch (opMode) {
                                case 0:
                                        operatingMode = OperatingMode.PowerDown;
                                        break;
                                    case 1:
                                        operatingMode = OperatingMode.ShuntVoltage_Triggered;
                                        break;
                                    case 2:
                                        operatingMode = OperatingMode.BusVoltage_Triggered;
                                        break;
                                    case 3:
                                        operatingMode = OperatingMode.Shunt_Bus_Triggered;
                                        break;
                                    case 4:
                                        operatingMode = OperatingMode.PowerDown;
                                        break;
                                    case 5:
                                        operatingMode = OperatingMode.ShuntVoltage_Continuous;
                                        break;
                                    case 6:
                                        operatingMode = OperatingMode.BusVoltage_Continuous;
                                        break;
                                    case 7:
                                        operatingMode = OperatingMode.Shunt_Bus_Continous;
                                        break;
                            }
                        })
                },
                {
                    (long)Registers.ShuntVoltage,
                    new WordRegister(this)
                        .WithValueField(0, 16, FieldMode.Read, name: "SHUNT_VOLTAGE", valueProviderCallback: _ => (ulong)shuntVoltage)
                },
                {
                    (long)Registers.BusVoltage,
                    new WordRegister(this)
                        .WithValueField(0, 16, FieldMode.Read, name: "BUS_VOLTAGE", valueProviderCallback: _ => busVoltage)
                },
                {
                    (long)Registers.Power,
                    new WordRegister(this)
                        .WithValueField(0, 16, FieldMode.Read, name: "POWER", valueProviderCallback: _ => Power)
                },
                {
                    (long)Registers.Current,
                    new WordRegister(this)
                        .WithValueField(0, 16, FieldMode.Read, name: "CURRENT", valueProviderCallback: _ => (ulong)Current)
                },
                {
                    (long)Registers.Calibration,
                    new WordRegister(this)
                        .WithValueField(0, 16, name: "CALIBRATION", valueProviderCallback: _ => calibration, writeCallback: (_, val) => calibration = (ushort)val)
                },
                {
                    (long)Registers.Mask_Enable,
                    new WordRegister(this)
                        .WithValueField(0, 16, name: "MASK/ENABLE")
                },
                {
                    (long)Registers.Alert_Limit,
                    new WordRegister(this)
                        .WithValueField(0, 16, name: "ALERT_LIMIT")
                },
                {
                    (long)Registers.Manufacturer_ID,
                    new WordRegister(this)
                        .WithValueField(0, 16, FieldMode.Read, name: "MANUFACTURER_ID", valueProviderCallback: _ => manufacturerId)
                },
                {
                    (long)Registers.Die_ID,
                    new WordRegister(this)
                        .WithValueField(0, 16, FieldMode.Read, valueProviderCallback: _ => dieId)
                }
            };

            return new WordRegisterCollection(this, registersMap);
        }

        public void FinishTransmission()
        {
            this.Log(LogLevel.Warning, "INA226 Transmission finished");
        }

        public byte[] Read(int count = 1)
        {
            this.Log(LogLevel.Debug, "READING FROM INA226: {0}", context);
            byte[] bytes = BitConverter.GetBytes((short)registers.Read((long)context));
            this.Log(LogLevel.Debug, "RETURNING: {0}", string.Join(", ", bytes));
            return bytes;
        }

        public void Write(byte[] data)
        {
            this.Log(LogLevel.Debug, "WRITING TO INA226: {0}", string.Join(", ", data));
            context = (Registers)data[0];
            if ((int)context < 0x2 || data.Length < 3) return; // Indicates that a read operation to input ports is next, but no changes are necessary
            registers.Write((long)context, (ushort)((data[1] << 8) | data[2]));
        }

        private static int ExtractValue(int startBit, int numBits, long value)
        {
            return (int)((value >> startBit) & (1 << numBits << 1));
        }

        public void Reset()
        {
            configuration = 0x4127;
            shuntVoltage = 0;
            busVoltage = 0;
            calibration = 0;
        }

        private readonly WordRegisterCollection registers;
        private ushort configuration = 0x4127;
        private ushort manufacturerId = 0x5449;
        private ushort dieId = 0x2260;
        private ushort calibration = 0x0000;
        private short shuntVoltage = 0x7FFF; //uV
        private ushort busVoltage = 0x0000; // uV
        private ushort numAverages = 0x01; // TODO: Implement
        private ushort busVoltageConversionTime = 1100; //us
        private ushort shortVoltageConversionTime = 1100; //us
        private short Current { get => (short)(shuntVoltage * calibration / 2048); }
        private ushort Power { get => (ushort)(Current * busVoltage / 20000); }
        private OperatingMode operatingMode = OperatingMode.Shunt_Bus_Continous;
        private Registers context;
        private enum OperatingMode
        {
            PowerDown,
            ShuntVoltage_Triggered,
            BusVoltage_Triggered,
            Shunt_Bus_Triggered,
            ShuntVoltage_Continuous,
            BusVoltage_Continuous,
            Shunt_Bus_Continous
        }


        private enum Registers : byte
        {
            Configuration = 0x00,
            ShuntVoltage = 0x01,
            BusVoltage = 0x02,
            Power = 0x03,
            Current = 0x04,
            Calibration = 0x05,
            Mask_Enable = 0x06, // Unimplemented
            Alert_Limit = 0x07, // Unimplemented
            Manufacturer_ID = 0xFE,
            Die_ID = 0xFF
        }
    }
}