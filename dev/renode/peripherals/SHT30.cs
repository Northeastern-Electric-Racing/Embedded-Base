//
// Copyright (c) 2010-2020 Antmicro
// Copyright (c) 2011-2015 Realtime Embedded
//
// This file is licensed under the MIT License.
// Full license text is available in 'licenses/MIT.txt'.
//
using System;
using System.Linq;
using Antmicro.Renode.Logging;
using Antmicro.Renode.Core;
using Antmicro.Renode.Peripherals.Sensor;
using Antmicro.Renode.Utilities;

namespace Antmicro.Renode.Peripherals.I2C
{
    public class SHT30 : II2CPeripheral, ITemperatureSensor, IHumiditySensor
    {
        public double MeanHumidity { get; set; }
        public SHT30()
        {
            MeanHumidity = 75.0;
            commands = new I2CCommandManager<Action<byte[]>>();

            commands.RegisterCommand(SetRepeatabilityAndClockStretching(Repeatability.HIGH, true), new byte[] { 0x2C, 0x06 });
            commands.RegisterCommand(SetRepeatabilityAndClockStretching(Repeatability.MEDIUM, true), new byte[] { 0x2C, 0x0D });
            commands.RegisterCommand(SetRepeatabilityAndClockStretching(Repeatability.LOW, true), new byte[] { 0x2C, 0x10 });

            commands.RegisterCommand(SetRepeatabilityAndClockStretching(Repeatability.HIGH, false), new byte[] { 0x24, 0x06 });
            commands.RegisterCommand(SetRepeatabilityAndClockStretching(Repeatability.MEDIUM, false), new byte[] { 0x24, 0x0D });
            commands.RegisterCommand(SetRepeatabilityAndClockStretching(Repeatability.LOW, false), new byte[] { 0x24, 0x10 });

            commands.RegisterCommand(SetRepeatabilityAndMPS(Repeatability.HIGH, 0.5), new byte[] { 0x20, 0x32 });
            commands.RegisterCommand(SetRepeatabilityAndMPS(Repeatability.MEDIUM, 0.5), new byte[] { 0x20, 0x24 });
            commands.RegisterCommand(SetRepeatabilityAndMPS(Repeatability.LOW, 0.5), new byte[] { 0x20, 0x2F });

            commands.RegisterCommand(SetRepeatabilityAndMPS(Repeatability.HIGH, 1), new byte[] { 0x21, 0x30 });
            commands.RegisterCommand(SetRepeatabilityAndMPS(Repeatability.MEDIUM, 1), new byte[] { 0x21, 0x26 });
            commands.RegisterCommand(SetRepeatabilityAndMPS(Repeatability.LOW, 1), new byte[] { 0x21, 0x2D });

            commands.RegisterCommand(SetRepeatabilityAndMPS(Repeatability.HIGH, 2), new byte[] { 0x22, 0x36 });
            commands.RegisterCommand(SetRepeatabilityAndMPS(Repeatability.MEDIUM, 2), new byte[] { 0x22, 0x20 });
            commands.RegisterCommand(SetRepeatabilityAndMPS(Repeatability.LOW, 2), new byte[] { 0x22, 0x2B });

            commands.RegisterCommand(SetRepeatabilityAndMPS(Repeatability.HIGH, 4), new byte[] { 0x23, 0x34 });
            commands.RegisterCommand(SetRepeatabilityAndMPS(Repeatability.MEDIUM, 4), new byte[] { 0x23, 0x22 });
            commands.RegisterCommand(SetRepeatabilityAndMPS(Repeatability.LOW, 4), new byte[] { 0x23, 0x29 });

            commands.RegisterCommand(SetRepeatabilityAndMPS(Repeatability.HIGH, 10), new byte[] { 0x27, 0x37 });
            commands.RegisterCommand(SetRepeatabilityAndMPS(Repeatability.MEDIUM, 10), new byte[] { 0x27, 0x21 });
            commands.RegisterCommand(SetRepeatabilityAndMPS(Repeatability.LOW, 10), new byte[] { 0x27, 0x2A });

            commands.RegisterCommand(_ => this.DebugLog("Setting Accelerate response time"), new byte[] { 0x2B, 0x32 });

            commands.RegisterCommand(_ => this.DebugLog("Break command received"), new Byte[] { 0x30, 0x93 });

            commands.RegisterCommand(_ => Reset(), new byte[] { 0x30, 0xA2 });

            commands.RegisterCommand(_ => HeaterEnabled = true, new byte[] { 0x30, 0x6D });
            commands.RegisterCommand(_ => HeaterEnabled = false, new byte[] { 0x30, 0x66 });

            commands.RegisterCommand(_ => readStatus = true, new byte[] { 0xF3, 0x2D });

            commands.RegisterCommand(_ => Status = 0x0000, new byte[] { 0x30, 0x41 });

            Reset();
        }

        public void Reset()
        {

        }

        public void Write(byte[] data)
        {
            // Parse the list bytes
            if (data.Length < 2)
            {
                // Requires Command MSB and Command LSB on all writes
                this.Log(LogLevel.Noisy, "Write - too few elements in list ({0}) - must be at least two", data.Length);
                return;
            }
            this.NoisyLog("Write {0}", string.Join(",", data));
            byte msb = data[0];
            byte lsb = data[1];
        }

        public byte[] Read(int count = 0)
        {
            if (readStatus)
            {
                readStatus = false;
                return BitConverter.GetBytes(Status);
            }
            byte[] temperature = BitConverter.GetBytes((short)GetTemperature());
            byte[] humidity = BitConverter.GetBytes((short)GetHumidity());
            byte temperatureCRC = GetCRC(temperature, 2);
            byte humidityCRC = GetCRC(humidity, 2);
            return new byte[] { temperature[0], temperature[1], temperatureCRC, humidity[0], humidity[1], humidityCRC };
        }

        public void FinishTransmission()
        {
            this.DebugLog("Transmission Finished");
        }

        private double SensorData(double mean, double sigma)
        {
            // mean = mean value of Gaussian (Normal) distribution and sigma = standard deviation
            int sign = random.Next(10);
            double x;
            if (sign > 5)
            {
                x = mean * (1.0 + random.NextDouble() / (2 * sigma));
            }
            else
            {
                x = mean * (1.0 - random.NextDouble() / (2 * sigma));
            }
            return x;
        }

        private double GetTemperature()
        {
            double temperature = SensorData(25.0, 1.0);
            double result = (temperature + 45) * (65536.0 - 1) / 175;
            return result;
        }

        private double GetHumidity()
        {
            double humidity = SensorData(MeanHumidity, 5.0);
            if (humidity > 99.0)
            {
                humidity = 99.0;
            }
            if (humidity < 1.0)
            {
                humidity = 1.0;
            }
            double result = humidity * (65536.0 - 1) / 100;
            return result;
        }

        private byte GetCRC(byte[] array, int nrOfBytes)
        {
            const uint POLYNOMIAL = 0x31;  // P(x)=x^8+x^5+x^4+1 = 100110001
            byte crc = 0xFF;
            for (byte i = 0; i < nrOfBytes; ++i)
            {
                crc ^= (array[i]);
                for (byte bit = 8; bit > 0; --bit)
                {
                    if ((crc & 0x80) == 0x80)
                    {
                        crc = (byte)(((uint)crc << 1) ^ POLYNOMIAL);
                    }
                    else
                    {
                        crc = (byte)(crc << 1);
                    }
                }
            }
            return crc;
        }

        private Action<byte[]> SetRepeatabilityAndClockStretching(Repeatability repeatability, bool clockStretchingEnabled)
        {
            return _ =>
            {
                this.CurrentRepeatability = repeatability;
                this.ClockStretching = clockStretchingEnabled;
            };
        }

        private Action<byte[]> SetRepeatabilityAndMPS(Repeatability repeatability, double mps)
        {
            return _ =>
            {
                this.CurrentRepeatability = repeatability;
                this.MeasurementPerSecond = mps;
            };
        }

        private bool readStatus = false;
        private decimal humidity;
        private decimal temperature;
        private readonly I2CCommandManager<Action<byte[]>> commands;

        public bool ClockStretching { get; private set; }
        public Repeatability CurrentRepeatability { get; private set; }
        public double MeasurementPerSecond { get; private set; }

        public decimal Humidity { get => humidity; set => humidity = value; }
        public decimal Temperature { get => temperature; set => temperature = value; }
        public bool HeaterEnabled { get; private set; }
        public short Status { get; private set; }

        public enum Repeatability
        {
            HIGH,
            MEDIUM,
            LOW
        }

        private readonly static PseudorandomNumberGenerator random = EmulationManager.Instance.CurrentEmulation.RandomGenerator;
    }
}

