using System;
using System.Collections.Generic;
using Antmicro.Renode.Core;
using Antmicro.Renode.Core.Structure.Registers;
using Antmicro.Renode.Utilities;
using Antmicro.Renode.Logging;
using Antmicro.Renode.Peripherals.GPIOPort;
using System.Linq;

namespace Antmicro.Renode.Peripherals.I2C
{
    public class PCA9539 : BaseGPIOPort, II2CPeripheral
    {
        public PCA9539(Machine machine) : base(machine, NumberGPIOs)
        {
            registers = DefineRegisters();
        }

        public byte[] Read(int count = 1)
        {
            this.Log(LogLevel.Info, "READING FROM PCA: {0}", context);
            byte[] bytes = BitConverter.GetBytes((short)registers.Read((long)context));
            this.Log(LogLevel.Info, "RETURNING: {0}", string.Join(", ", bytes));
            return bytes;
        }

        public void Write(byte[] data)
        {
            this.Log(LogLevel.Info, "WRITING TO PCA: {0}", string.Join(", ", data));
            context = (Registers)data[0];
            if ((int)context < 0x2 || data.Length < 2) return; // Indicates that a read operation to input ports is next, but no changes are necessary
            registers.Write((long)context, data[1]);
        }

        private ByteRegisterCollection DefineRegisters()
        {
            var registersMap = new Dictionary<long, ByteRegister>
            {
                {
                    (long)Registers.InputPort0, new ByteRegister(this)
                                        .WithValueField(0, 8, FieldMode.Read, name: "INPUT0", valueProviderCallback: _ =>
                                        {
                                            var result = new Span<bool>(State, 0, 8).ToArray();
                                            return BitHelper.GetValueFromBitsArray(result);
                                        })
                },
                {
                    (long)Registers.InputPort1, new ByteRegister(this)
                                        .WithValueField(0, 8, FieldMode.Read, name: $"INPUT1", valueProviderCallback: _ =>
                                        {
                                            var result = new Span<bool>(State, 7, 8).ToArray();
                                            return BitHelper.GetValueFromBitsArray(result);
                                        })
                },
                {
                    (long)Registers.OutputPort0,
                    new ByteRegister(this)
                        .WithValueField(0, 8, name: $"OUPUT0", writeCallback: (_, val) => WriteCallback((byte) val))
                },
                {
                    (long)Registers.OutputPort1,
                    new ByteRegister(this)
                        .WithValueField(0, 8, name: $"OUPUT1", writeCallback: (_, val) => WriteCallback((byte) val, 8))
                },
                {
                    (long)Registers.PolarityInversionPort0,
                    new ByteRegister(this).WithValueField(0, 8, name: $"POLARITY0", writeCallback: (_, val) =>
                        {
                            BitHelper.ReplaceBits(ref polarityInversion, width: 8, source: (uint)val);
                        })
                },
                {
                    (long)Registers.PolarityInversionPort1,
                    new ByteRegister(this).WithValueField(0, 8, name: $"POLARITY1", writeCallback: (_, val) =>
                        {
                            BitHelper.ReplaceBits(ref polarityInversion, width: 8, source: (uint)val, destinationPosition: 8);
                        })
                },
                {
                    (long)Registers.ConfigurationPort0,
                    new ByteRegister(this).WithValueField(0, 8, name: $"CONFIGURATION0", writeCallback: (_, val) => ConfigurationCallback((byte) val))
                },
                {
                    (long)Registers.ConfigurationPort1,
                    new ByteRegister(this).WithValueField(0, 8, name: $"CONFIGURATION1", writeCallback: (_, val) => ConfigurationCallback((byte) val, 8))
                }
            };

            return new ByteRegisterCollection(this, registersMap);
        }

        private void WriteCallback (byte value, int offset = 0) {
            BitHelper.ReplaceBits(ref outputFlipFlop, width: 8, source: (byte)value, destinationPosition: offset);
            this.SetPins();
            this.Log(LogLevel.Info, "Writing {0}: {1} to {2}", offset, OutputFlipFlop, value);
        }

        private void ConfigurationCallback(byte value, int offset = 0) {
            BitHelper.ReplaceBits(ref configuration, width: 8, source: (byte)value, destinationPosition: offset);
            this.SetPins();
            this.Log(LogLevel.Info, "Configuring {0}: {1} to {2}", offset, Configuration, value);
        }

        private void SetPins() {
            var outputBits = new Span<bool>(BitHelper.GetBits(outputFlipFlop), 0, 16).ToArray();
            var configurationBits = new Span<bool>(BitHelper.GetBits(configuration), 0, 16).ToArray();

            for (var i = 0; i < outputBits.Length; i++) {
                if (!configurationBits[i]) { // False means that it is enabled as output
                    WritePin(i, outputBits[i]);
                }
            }
        }
        private void WritePin(int number, bool value)
        {
            this.Log(LogLevel.Info, "Setting {0} to {1}", number, value);
            State[number] = value;
            Connections[number].Set(value);
        }

        public void FinishTransmission()
        {
            this.Log(LogLevel.Debug, "Transmission Finished");
        }

        public GPIO IRQ { get; private set; }

        private const int NumberGPIOs = 16;
        private uint polarityInversion = 0xFFFF;
        private uint configuration = 0xFFFF;
        private uint outputFlipFlop = 0xFFFF;
        public uint PolarityInversion { get => polarityInversion; }
        public uint Configuration { get => configuration; }
        public uint OutputFlipFlop { get => outputFlipFlop; }
        private readonly ByteRegisterCollection registers;
        private Registers context;

        private enum Registers : byte
        {
            InputPort0 = 0x0,
            InputPort1 = 0x1,
            OutputPort0 = 0x2,
            OutputPort1 = 0x3,
            PolarityInversionPort0 = 0x4,
            PolarityInversionPort1 = 0x5,
            ConfigurationPort0 = 0x6,
            ConfigurationPort1 = 0x7
        }
    }
}