using Antmicro.Renode.Peripherals.GPIOPort;
using Antmicro.Renode.Core;
using Antmicro.Renode.Core.Structure.Registers;
using Antmicro.Renode.Logging;

namespace Antmicro.Renode.Peripherals.I2C
{
    public class VL6180X : BaseGPIOPort, II2CPeripheral, IProvidesRegisterCollection<ByteRegisterCollection>
    {
        public VL6180X(IMachine machine) : base(machine, NumberGPIOs)
        {
            RegistersCollection = new ByteRegisterCollection(this);
            DefineRegisters();
        }

        private void DefineRegisters()
        {
            Registers.ModelId.Define(this, 0xB4)
                .WithValueField(0, 8, name: "Model Id");

            Registers.ModelRevMajor.Define(this, 0x1)
                .WithValueField(0, 3, name: "Model Rev Major")
                .WithValueField(3, 5, FieldMode.Read, name: "Reserved");

            Registers.ModelRevMinor.Define(this, 0x3)
                .WithValueField(0, 3, name: "Model Rev Minor")
                .WithValueField(3, 5, FieldMode.Read, name: "Reserved");

            Registers.ModuleRevMajor.Define(this, 0x1)
                .WithValueField(0, 3, name: "Module Rev Major")
                .WithValueField(3, 5, FieldMode.Read, name: "Reserved");

            Registers.ModuleRevMinor.Define(this, 0x2)
                .WithValueField(0, 3, name: "Module Rev Minor")
                .WithValueField(3, 5, FieldMode.Read, name: "Reserved");

            Registers.DateHi.Define(this)
                .WithValueField(0, 4, name: "Identification Month")
                .WithValueField(4, 4, name: "Identification Year");

            Registers.DateLo.Define(this)
                .WithValueField(0, 3, name: "Identification Phase")
                .WithValueField(3, 5, name: "Identification Day");

            Registers.IdentificationTime1.DefineMany(this, 2, (register, registerOffset) =>
            {
                register.WithValueField(0, 8, name: $"Identification {registerOffset}");
            });

            Registers.GPIO0.Define(this, 0x60)
                .WithValueField(0, 1, name: "Reserved")
                .WithValueField(1, 4, name: "System GPIO 0 Select")
                .WithValueField(5, 1, name: "System GPIO 0 Polarity")
                .WithValueField(6, 1, name: "System GPIO 0 is xshutdown")
                .WithValueField(7, 1, FieldMode.Read, name: "Reserved End");

            Registers.GPIO1.Define(this, 0x20)
                .WithValueField(0, 1, name: "Reserved")
                .WithValueField(1, 4, name: "System GPIO 1 Select")
                .WithValueField(5, 1, name: "System GPIO 1 Polarity")
                .WithValueField(6, 2, FieldMode.Read, name: "Reserved End");

            Registers.HistoryControl.Define(this)
                .WithValueField(0, 1, name: "System History Buffer Enable")
                .WithValueField(1, 1, name: "System History Buffer Mode")
                .WithValueField(2, 1, name: "System History Buffer Clear")
                .WithValueField(3, 5, FieldMode.Read, name: "Reserved");

            Registers.InterruptConfigGPIO.Define(this)
                .WithValueField(0, 3, name: "Range Int Mode")
                .WithValueField(3, 3, name: "ALS int Mode")
                .WithValueField(6, 2, FieldMode.Read, name: "Reserved");

            Registers.InterruptClear.Define(this)
                .WithValueField(0, 3, name: "Int Clear Sig")
                .WithValueField(3, 5, FieldMode.Read, name: "Reserved");

            Registers.FreshOutOfReset.Define(this, 0x1)
                .WithValueField(0, 1, name: "Fresh out of Reset")
                .WithValueField(1, 7, FieldMode.Read, name: "Reserved");

            Registers.GroupedParameterHold.Define(this)
                .WithValueField(0, 1, name: "Grouped Parameter Hold")
                .WithValueField(1, 7, FieldMode.Read, name: "Reserved");

            Registers.SysrangeStart.Define(this)
                .WithValueField(0, 1, name: "Sysrange Startup")
                .WithValueField(1, 1, name: "Sysrange Mode Select")
                .WithValueField(2, 6, FieldMode.Read, name: "Reserved");

            Registers.SysrangeThreshHigh.Define(this, 0xFF)
                .WithValueField(0, 8, name: "Sysrange Thresh High");

            Registers.SysrangeThreshLow.Define(this)
                .WithValueField(0, 8, name: "Sysrange Thresh Low");

            Registers.SysrangeIntermeasurementPeriod.Define(this, 0xFF)
                .WithValueField(0, 8, name: "Sysrange Intermeasurement Period");

            Registers.SysrangeMaxConvergenceTime.Define(this, 0x31)
                .WithValueField(0, 6, name: "Sysrange Max Convergence Time")
                .WithValueField(6, 2, FieldMode.Read, name: "Reserved");

            Registers.SysrangeCrosstalkCompensationRate.DefineMany(this, 2, (register, idx) =>
            {
                register.WithValueField(0, 8, name: $"Sysrange Crosstalk compensation Rate {idx}");
            });

            Registers.SysrangeCrosstalkValidHeight.Define(this, 0x14)
                .WithValueField(0, 8, name: "Sysrange Crosstalk Valid Height");

            Registers.SysrangeEarlyConvergenceEstimate.DefineMany(this, 2, (register, idx) =>
            {
                register.WithValueField(0, 8, name: $"Sysrange Early Convergence Estimate {idx}");
            });

            Registers.SysrangePartToPartRangeOffset.Define(this)
                .WithValueField(0, 8, name: "Sysrange Part to Part Range Offset");

            Registers.SysrangeRangeIgnoreValidHeight.Define(this)
                .WithValueField(0, 8, name: "Sysrange Range Ignore Valid Height");

            Registers.SysrangeRangeIgnoreThreshold.DefineMany(this, 2, (register, idx) =>
            {
                register.WithValueField(0, 8, name: $"Sysrange Range Ignore Threshold {idx}");
            });

            Registers.SysrangeMaxAmbientLevelMult.Define(this, 0xA0)
                .WithValueField(0, 8, name: "Sysrange Max Ambient Level Mult");

            Registers.SysrangeRangeCheckEnables.Define(this, 0x11)
                .WithValueField(0, 1, name: "Sysrange early convergence enable")
                .WithValueField(1, 1, name: "Sysrange Range Ignore Enable")
                .WithValueField(2, 1, FieldMode.Read, name: "Reserved 1?")
                .WithValueField(3, 1, name: "Reserved 2?")
                .WithValueField(4, 1, name: "Sysrange Signal to Noise Enable")
                .WithValueField(5, 3, FieldMode.Read, name: "Reserved");

            Registers.SysrangeVHVRecalibrate.Define(this)
                .WithValueField(0, 1, name: "Sysrange VHV Recalibrate")
                .WithValueField(1, 1, name: "Sysrange VHV Status")
                .WithValueField(2, 6, FieldMode.Read, name: "Reserved");

            Registers.SysrangeVHVRepeatRate.Define(this)
                .WithValueField(0, 8, name: "Sysrange VHV Repeat Rate");

            Registers.SysalsStart.Define(this)
                .WithValueField(0, 1, name: "SYSALS Startstop")
                .WithValueField(1, 1, name: "Sysbus Mode Select")
                .WithValueField(2, 6, FieldMode.Read, name: "Reserved");

            Registers.SysalsThreshHigh.DefineMany(this, 2, (register, idx) =>
            {
                register.WithValueField(0, 8, name: $"Sysals Thresh High {idx}");
            }, resetValue: 0xFF);

            Registers.SysalsThreshLow.DefineMany(this, 2, (register, idx) =>
            {
                register.WithValueField(0, 8, name: $"Sysals Thresh Low {idx}");
            });

            Registers.SysalsIntermeasurementPeriod.Define(this, 0xFF)
                .WithValueField(0, 8, name: "Sysals Intermeasurement Period");

            Registers.SysaslAnalogueGain.Define(this, 0x06)
                .WithValueField(0, 3, name: "Sysals Analogue Gain Light")
                .WithValueField(3, 5, FieldMode.Read, name: "Reserved");

            Registers.SysalsIntegrationPeriod.DefineMany(this, 2, (register, idx) =>
            {
                if (idx == 1)
                {
                    register.WithValueField(0, 8, name: "Sysals Integration Period");
                }
                else
                {
                    register
                    .WithValueField(0, 1, name: "Sysals Integration Period")
                    .WithValueField(1, 7, FieldMode.Read, name: "Reserved");
                }
            });

            Registers.ResultRangeState.Define(this, 0x1)
                .WithValueField(0, 1, FieldMode.Read, name: "Result Range Device Ready")
                .WithValueField(1, 1, FieldMode.Read, name: "Result Range Measurement Ready")
                .WithValueField(2, 1, FieldMode.Read, name: "Result Range Max Threshold Hit")
                .WithValueField(3, 1, FieldMode.Read, name: "Result Range Min Threshold Hit")
                .WithValueField(4, 4, FieldMode.Read, name: "Result Range Error Code");

            Registers.ResultAlsStatus.Define(this, 0x1)
                .WithValueField(0, 1, FieldMode.Read, name: "Result ALS Device Ready")
                .WithValueField(1, 1, FieldMode.Read, name: "Result ALS Measurement Ready")
                .WithValueField(2, 1, FieldMode.Read, name: "Result ALS Max Threshold Hit")
                .WithValueField(3, 1, FieldMode.Read, name: "Result ALS Min Threshold Hit")
                .WithValueField(4, 4, FieldMode.Read, name: "Result ALS Error Code");

            Registers.ResultInterruptStatusGPIO.Define(this)
                .WithValueField(0, 3, FieldMode.Read, name: "Result Int Range GPIO")
                .WithValueField(3, 3, FieldMode.Read, name: "Result int ALS GPIO")
                .WithValueField(6, 2, FieldMode.Read, name: "Result Int Error GPIO");

            Registers.ResultAlsVal.DefineMany(this, 2, (register, idx) =>
            {
                register.WithValueField(0, 8, FieldMode.Read, name: $"Result ALS Ambient Light {idx}");
            });

            Registers.ResultHistoryBuffer0.DefineMany(this, 2, (register, idx) =>
            {
                register.WithValueField(0, 8, FieldMode.Read, name: "Result History Buffer 0");
            });

            Registers.ResultHistoryBuffer1.DefineMany(this, 2, (register, idx) =>
            {
                register.WithValueField(0, 8, FieldMode.Read, name: "Result History Buffer `");
            });

            Registers.ResultHistoryBuffer2.DefineMany(this, 2, (register, idx) =>
            {
                register.WithValueField(0, 8, FieldMode.Read, name: "Result History Buffer 2");
            });

            Registers.ResultHistoryBuffer3.DefineMany(this, 2, (register, idx) =>
            {
                register.WithValueField(0, 8, FieldMode.Read, name: "Result History Buffer 3");
            });

            Registers.ResultHistoryBuffer4.DefineMany(this, 2, (register, idx) =>
            {
                register.WithValueField(0, 8, FieldMode.Read, name: "Result History Buffer 4");
            });

            Registers.ResultHistoryBuffer5.DefineMany(this, 2, (register, idx) =>
            {
                register.WithValueField(0, 8, FieldMode.Read, name: "Result History Buffer 5");
            });

            Registers.ResultHistoryBuffer6.DefineMany(this, 2, (register, idx) =>
            {
                register.WithValueField(0, 8, FieldMode.Read, name: "Result History Buffer 6");
            });

            Registers.ResultHistoryBuffer7.DefineMany(this, 2, (register, idx) =>
            {
                register.WithValueField(0, 8, FieldMode.Read, name: "Result History Buffer 7");
            });

            Registers.ResultRangeValue.Define(this)
                .WithValueField(0, 8, FieldMode.Read, name: "Result Range Val");

            Registers.ResultRangeRaw.Define(this)
                .WithValueField(0, 8, FieldMode.Read, name: "Result Range Raw");

            Registers.ResultRangeReturnRate.DefineMany(this, 2, (register, idx) =>
            {
                register.WithValueField(0, 8, FieldMode.Read, name: $"Result Range Return Rate {idx}");
            });

            Registers.ResultRangeReferenceRate.DefineMany(this, 2, (register, idx) =>
            {
                register.WithValueField(0, 8, FieldMode.Read, name: $"Result Range Reference Rate {idx}");
            });

            Registers.ResultRangeReturnSignalCount.DefineMany(this, 4, (register, idx) =>
            {
                register.WithValueField(0, 8, FieldMode.Read, name: $"Result Range Return Signal Count");
            });

            Registers.ResultRangeReferenceSignalCount.DefineMany(this, 4, (register, idx) =>
            {
                register.WithValueField(0, 8, FieldMode.Read, name: $"Result Range Reference Signal Count");
            });

            Registers.ResultRangeReturnAmbCount.DefineMany(this, 4, (register, idx) =>
            {
                register.WithValueField(0, 8, FieldMode.Read, name: $"Result Range Return AMB Count");
            });

            Registers.ResultRangeReferenceAmbCount.DefineMany(this, 4, (register, idx) =>
            {
                register.WithValueField(0, 8, FieldMode.Read, name: $"Result Range Reference AMB Count");
            });

            Registers.ResultRangeReturnConvTime.DefineMany(this, 4, (register, idx) =>
            {
                register.WithValueField(0, 8, FieldMode.Read, name: $"Result Range Return Conversion Time");
            });

            Registers.ResultRangeReferenceConvTime.DefineMany(this, 4, (register, idx) =>
            {
                register.WithValueField(0, 8, FieldMode.Read, name: $"Result Range Reference Conversion Time");
            });

            Registers.ReadoutAveragingSamplePeriod.Define(this, 0x30)
                .WithValueField(0, 8, name: "Readout Averaging Sample Period");

            Registers.FirmwareBootup.Define(this, 0x1)
                .WithValueField(0, 1, name: "Firmware Bootup")
                .WithValueField(1, 7, FieldMode.Read, name: "Reserved");

            Registers.FirmwareResultScaler.Define(this, 0x1)
                .WithValueField(0, 4, name: "Firmware ALS Result Scalar")
                .WithValueField(4, 4, FieldMode.Read, name: "Reserved");

            Registers.I2CSlaveDeviceAddress.Define(this, 0x29)
                .WithValueField(0, 7, name: "Super I2C Slave Device Address")
                .WithValueField(7, 1, FieldMode.Read, name: "Reserved");

            Registers.InterleavedModeEnable.Define(this)
                .WithValueField(0, 8, name: "Interleaved Mode Enabled");
        }

        public void FinishTransmission()
        {
            this.DebugLog("Transmission Finished");
        }

        public byte[] Read(int count = 1)
        {
            byte[] data = new byte[] { RegistersCollection.Read(address) };
            this.DebugLog("Reading Data: {0} from {1}", string.Join(", ", data), address);
            return data;
        }

        public void Write(byte[] data)
        {
            this.DebugLog("Writing Data: {0}", string.Join(", ", data));
            byte addressMSB = data[0];
            byte addressLSB = data[1];

            address = (short)((addressMSB << 8) | addressLSB);

            if (data.Length > 2)
            {
                byte content = data[2];
                RegistersCollection.Write(address, content);
            }
        }

        private short address;
        private const int NumberGPIOs = 2;

        public ByteRegisterCollection RegistersCollection { get; private set; }

        private enum Registers : short
        {
            ModelId = 0x0,
            ModelRevMajor = 0x1,
            ModelRevMinor = 0x2,
            ModuleRevMajor = 0x3,
            ModuleRevMinor = 0x4,
            DateHi = 0x6,
            DateLo = 0x7,
            IdentificationTime1 = 0x8,
            IdentificationTime2 = 0x9,
            GPIO0 = 0x10,
            GPIO1 = 0x11,
            HistoryControl = 0x12,
            InterruptConfigGPIO = 0x14,
            InterruptClear = 0x15,
            FreshOutOfReset = 0x16,
            GroupedParameterHold = 0x17,
            SysrangeStart = 0x18,
            SysrangeThreshHigh = 0x19,
            SysrangeThreshLow = 0x1A,
            SysrangeIntermeasurementPeriod = 0x1B,
            SysrangeMaxConvergenceTime = 0x1C,
            SysrangeCrosstalkCompensationRate = 0x1E,
            SysrangeCrosstalkValidHeight = 0x21,
            SysrangeEarlyConvergenceEstimate = 0x22,
            SysrangePartToPartRangeOffset = 0x24,
            SysrangeRangeIgnoreValidHeight = 0x25,
            SysrangeRangeIgnoreThreshold = 0x26,
            SysrangeMaxAmbientLevelMult = 0x2C,
            SysrangeRangeCheckEnables = 0x2D,
            SysrangeVHVRecalibrate = 0x2E,
            SysrangeVHVRepeatRate = 0x31,
            SysalsStart = 0x38,
            SysalsThreshHigh = 0x3A,
            SysalsThreshLow = 0x3C,
            SysalsIntermeasurementPeriod = 0x3E,
            SysaslAnalogueGain = 0x3F,
            SysalsIntegrationPeriod = 0x40,
            ResultRangeState = 0x04D,
            ResultAlsStatus = 0x4E,
            ResultInterruptStatusGPIO = 0x4F,
            ResultAlsVal = 0x50,
            ResultHistoryBuffer0 = 0x52,
            ResultHistoryBuffer1 = 0x54,
            ResultHistoryBuffer2 = 0x56,
            ResultHistoryBuffer3 = 0x58,
            ResultHistoryBuffer4 = 0x5A,
            ResultHistoryBuffer5 = 0x5C,
            ResultHistoryBuffer6 = 0x5E,
            ResultHistoryBuffer7 = 0x60,
            ResultRangeValue = 0x62,
            ResultRangeRaw = 0x64,
            ResultRangeReturnRate = 0x66,
            ResultRangeReferenceRate = 0x68,
            ResultRangeReturnSignalCount = 0x6C,
            ResultRangeReferenceSignalCount = 0x70,
            ResultRangeReturnAmbCount = 0x74,
            ResultRangeReferenceAmbCount = 0x78,
            ResultRangeReturnConvTime = 0x7C,
            ResultRangeReferenceConvTime = 0x80,
            ReadoutAveragingSamplePeriod = 0x10A,
            FirmwareBootup = 0x119,
            FirmwareResultScaler = 0x120,
            I2CSlaveDeviceAddress = 0x212,
            InterleavedModeEnable = 0x2A3
        }
    }
}