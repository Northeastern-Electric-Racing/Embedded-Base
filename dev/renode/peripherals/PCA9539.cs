using System;
using System.Collections.Generic;
using Antmicro.Renode.Core;
using Antmicro.Renode.Core.Structure.Registers;
using Antmicro.Renode.Utilities;
using Antmicro.Renode.Logging;
using Antmicro.Renode.Peripherals.GPIOPort;
using Antmicro.Renode.Peripherals.Bus;
using Antmicro.Renode.Utilities.Collections;

namespace Antmicro.Renode.Peripherals.I2C
{
    public class PCA9539 : BaseGPIOPort, II2CPeripheral
    {
        public PCA9539(Machine machine) : base(machine, NumberGPIOs)
        {
            IRQ = new GPIO();
            irqManager = new GPIOInterruptManager(IRQ, State);
            registers = DefineRegisters();
        }

        public byte[] Read(int count = 1)
        {
            return BitConverter.GetBytes(registers.Read((long)context));
        }

        public void Write(byte[] data)
        {
            context = (Registers)data[0];
            if ((int)context < 0x2) return; // Indicates that a read operation to input ports is next, but no changes are necessary
            registers.Write((long)context, data[0]);
        }

        private ByteRegisterCollection DefineRegisters()
        {
            var registersMap = new Dictionary<long, ByteRegister>
            {
                {
                    (long)(Registers.InputPort0), new ByteRegister(this)
                                        .WithValueField(0, 8, name: "INPUT0", FieldMode.Read, valueProviderCallback: _ =>
                                        {
                                            var gpioPins = irqManager.PinDirection.Select(x => (x & GPIOInterruptManager.Direction.Input) != 0);
                                            var result = gpioPins.Zip(State, (pins, state) => pin && state);
                                            return BitHelper.getValueFromBitsArray(result);
                                        })
                },
                {
                    (long)(Registers.InputPort1), new ByteRegister(this)
                                        .WithValueField(0, 8, name: $"INPUT1", FieldMode.Read, valueProviderCallback: _ =>
                                        {
                                            var gpioPins = irqManager.PinDirection.Select(x => (x & GPIOInterruptManager.Direction.Input) != 0);
                                            var result = gpioPins.Zip(State, (pins, state) => pin && state);
                                            return BitHelper.getValueFromBitsArray(result);
                                        })
                },
                {
                    (long)(Registers.OutputPort0),
                    new ByteRegister(this)
                        .WithValueField(0, 8, name: $"OUPUT0", valueProviderCallback: val => {
                            var gpioPins = irqManager.PinDirection.Select(x => (x & GPIOInterruptManager.Direction.Output) != 0);
                            var result = gpioPins.Zip(Connection.Values, (pin, state) => pin && state.IsSet);
                            return BitHelper.GetValueFromBitsArrray(result);
                        }, writeCallback: (_, val) =>
                        {
                            var bits = BitHelper.GetBits(val);
                            for (var i = 0; i < bits.length; i++) {
                                if ((irqManager.PinDirection[i] & GPIOInterruptManager.Direction.Output) != 0) {
                                    Connections[i].Set(bits[i]);
                                }
                            }
                            BitHelper.ReplaceBits(ref outputValue, width: 8, source: val, destinationPosition: 16);
                        })
                },
                {
                    (long)(Registers.OutputPort1),
                    new ByteRegister(this)
                        .WithValueField(0, 8, name: $"OUPUT1", valueProviderCallback: val => {
                            var gpioPins = irqManager.PinDirection.Select(x => (x & GPIOInterruptManager.Direction.Output) != 0);
                            var result = gpioPins.Zip(Connection.Values, (pin, state) => pin && state.IsSet);
                            return BitHelper.GetValueFromBitsArrray(result);
                        }, writeCallback: (_, val) =>
                        {
                            var bits = BitHelper.GetBits(val);
                            for (var i = 0; i < bits.length; i++) {
                                if ((irqManager.PinDirection[i] & GPIOInterruptManager.Direction.Output) != 0) {
                                    Connections[i].Set(bits[i]);
                                }
                            }
                            BitHelper.ReplaceBits(ref outputValue, width: 8, source: val, destinationPosition: 16 - 8);
                        })
                },
                {
                    (long)(Registers.PolarityInversionPort0),
                    new ByteRegister(this).WithFieldValue(0, 8, name: $"POLARITY0", writeCallback: (_, val) =>
                        {
                            BitHelper.ReplaceBits(ref polarityInversion, width: 8, source: val, destinationPosition: 16);
                        })
                },
                {
                    (long)(Registers.PolarityInversionPort1),
                    new ByteRegister(this).WithFieldValue(0, 8, name: $"POLARITY1", writeCallback: (_, val) =>
                        {
                            BitHelper.ReplaceBits(ref polarityInversion, width: 8, source: val, destinationPosition: 16 - 8);
                        })
                },
                {
                    (long)(Registers.ConfigurationPort0),
                    new ByteRegister(this).WithFieldValue(0, 8, name: $"CONFIGURATION0", writeCallback: (_, val) =>
                        {
                            BitHelper.ReplaceBits(ref configuration, width: 8, source: val, destinationPosition: 16);
                        })
                },
                {
                    (long)(Registers.ConfigurationPort1),
                    new ByteRegister(this).WithFieldValue(0, 8, name: $"CONFIGURATION1", writeCallback: (_, val) =>
                        {
                            BitHelper.ReplaceBits(ref configuration, width: 8, source: val, destinationPosition: 16 - 8);
                        })
                }
            };

            return new ByteRegisterCollection(this, registersMap);
        }

        public void FinishTransmission()
        {
            this.Log(LogLevel.Debug, "Transmission Finished");
        }

        public GPIO IRQ { get; private set; }

        private const int NumberGPIOs = 16;
        public uint SubregistersCount = 2;
        private uint inputValue;
        private uint outputValue = 0xFFFF;
        private uint polarityInversion = 0x0000;
        private uint configuration = 0xFFFF;
        private readonly ByteRegisterCollection registers;
        private Registers context;
        private readonly GPIOInterruptManager irqManager;

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