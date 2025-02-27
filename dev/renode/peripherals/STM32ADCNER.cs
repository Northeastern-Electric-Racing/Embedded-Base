// Derived from 1.15.0 STM32_ADC.cs
//
// Fix required to avoid div-by-zero inside Antmicro.Renode.Peripherals.Analog.STM32_ADC.OnConversionFinished
// Allow 8- and 16-bit reads of the DataRegister (as per RM0033 Rev9 10.13)
// Fix sequence index when ADC_CR2:ADON off and on transitions to avoid DMA transfer data offset mismatch
// Add support for EXTEN/EXTSEL conversion trigger
//
// Copyright (c) 2010-2023 Antmicro
//
//  This file is licensed under the MIT License.
//  Full license text is available in 'licenses/MIT.txt'.
//

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using Antmicro.Renode.Core;
using Antmicro.Renode.Core.Structure.Registers;
using Antmicro.Renode.Exceptions;
using Antmicro.Renode.Logging;
using Antmicro.Renode.Utilities;
using Antmicro.Renode.Time;
using Antmicro.Renode.Peripherals.Bus;
using Antmicro.Renode.Peripherals.Timers;

// TODO: Add a new "private class" to cover the individual ADC[123] controller instances
// TODO: Provide common registers in main class
// NOTE: IRQ is common interrupt source for all the controller instances

// NOTE: According to RM0033 Rev9 10.13 the register writes MUST be
// 32-bits, but reads can be 8-, 16- or 32-bits.
//
// RM0033 Rev9: All registers reset to 0 apart from ADC_HTR default 0x00000FFF

namespace Antmicro.Renode.Peripherals.Analog
{
   // STM32 ADC has many features and only a partial subset are implemented here
   //
   // Supported:
   // * Software triggered conversion
   // * EXTSEL triggered conversion
   // * Single conversion
   // * Scan mode with regular group
   // * Continuous conversion
   // * Modes of use
   //   - Polling (read EOC status flag)
   //   - Interrupt (enable ADC interrupt for EOC)
   //   - DMA (enable DMA and configure stream for peripheral to memory transfer)
   //
   // Not Implemented:
   // * Analog watchdog
   // * Overrun detection
   // * Injected channels
   // * Sampling time (time is fixed)
   // * Discontinuous mode
   // * Multi-ADC (i.e. Dual/Triple) mode
   public class STM32ADCNER : BasicDoubleWordPeripheral, IWordPeripheral, IBytePeripheral, IKnownSize, IGPIOReceiver
   {
      public STM32ADCNER(IMachine machine) : base(machine)
      {
         channels = Enumerable.Range(0, NumberOfChannels).Select(x => new ADCChannel(this, x)).ToArray();

         DefineRegisters();

         // Sampling time fixed
         samplingTimer = new LimitTimer(
               machine.ClockSource, 1000000, this, "samplingClock",
               limit: 100,
               eventEnabled: true,
               direction: Direction.Ascending,
               enabled: false,
               autoUpdate: false,
               workMode: WorkMode.OneShot);
         samplingTimer.LimitReached += OnConversionFinished;
      }

      public void OnGPIO(int number, bool value)
      {
          this.Log(LogLevel.Debug, "OnGPIO: number {0} value {1} extEn {2} extSel {3}", number, value, extEn.Value, extSel.Value);
          if (ExtEn.Disabled != extEn.Value)
          {
              // number 0..15 for EXTSEL sources
              if (number == (int)extSel.Value)
              {
                  bool doConversion = false;
                  switch (extEn.Value)
                  {
                  case ExtEn.EdgeRising:
                      doConversion = value;
                      break;
                  case ExtEn.EdgeFalling:
                      doConversion = !value;
                      break;
                  case ExtEn.EdgeBoth:
                      doConversion = true;
                      break;
                  }
                  if (doConversion)
                  {
                      this.Log(LogLevel.Debug, "OnGPIO: triggering conversion: regularSequenceLen {0}", regularSequenceLen);
                      // Trigger sampling across ALL the active channels
		      for (uint ach =  0; (ach < regularSequenceLen); ach++)
		      {
			  OnConversionFinished();
		      }
                  }
              }
              else // CONSIDER: adding JEXTSEL sources for number 16..31
              {
                  this.Log(LogLevel.Warning, "OnGPIO: Ignoring external trigger {0} {1} since extSel {2} configured", number, value, (int)extSel.Value);
              }
          }
          else
          {
              this.Log(LogLevel.Warning, "OnGPIO: Ignoring external trigger {0} {1} since extEn disabled", number, value);
          }
      }

      private void UpdateExtEn()
      {
          this.Log(LogLevel.Debug, "UpdateExtEn: extEn {0}", extEn.Value);
	  if (ExtEn.Disabled == extEn.Value)
	  {
	      if (scanMode.Value && !samplingTimer.Enabled)
	      {
                  this.Log(LogLevel.Debug, "UpdateExtEn: enabling internal timer");
		  samplingTimer.Enabled = true;
	      }
	  }
	  else
	  {
	      samplingTimer.Enabled = false;
	  }
      }

      // Even though 8- and 16-bit reads are acceptable for all
      // registers we only model the data register for the moment:
      public byte ReadByte(long offset)
      {
          byte rval = 0;
          if((Registers)offset == Registers.RegularData)
          {
              uint drval = ReadDoubleWord(offset);
              rval = (byte)(drval & 0xFF);
          }
          else
          {
              this.LogUnhandledRead(offset);
          }
          this.Log(LogLevel.Debug, "ReadByte: offset 0x{0:X} rval 0x{1:X}", offset, rval);
          return rval;
      }

      public void WriteByte(long offset, byte value)
      {
          this.Log(LogLevel.Debug, "WriteByte: offset 0x{0:X} value 0x{1:X}", offset, value);
          this.LogUnhandledWrite(offset, value);
      }

      public ushort ReadWord(long offset)
      {
          ushort rval = 0;
          if((Registers)offset == Registers.RegularData)
          {
              uint drval = ReadDoubleWord(offset);
              rval = (ushort)(drval & 0xFFFF);
          }
          else
          {
              this.LogUnhandledRead(offset);
          }
          this.Log(LogLevel.Debug, "ReadWord: offset 0x{0:X} rval 0x{1:X}", offset, rval);
          return rval;
      }

      public void WriteWord(long offset, ushort value)
      {
          this.Log(LogLevel.Info, "WriteWord: offset 0x{0:X} value 0x{1:X}", offset, value);
          this.LogUnhandledWrite(offset, value);
      }

      public void FeedSample(uint value, uint channelIdx, int repeat = 1)
      {
         if(IsValidChannel(channelIdx))
         {
            this.Log(LogLevel.Info, "FeedSample: single value 0x{0:X} channelIdx {1} repeat {2}", value, channelIdx, repeat);
            channels[channelIdx].FeedSample(value, repeat);
         }
      }

      public void FeedSample(string path, uint channelIdx, int repeat = 1)
      {
         if(IsValidChannel(channelIdx))
         {
            var parsedSamples = ADCChannel.ParseSamplesFile(path);
            channels[channelIdx].FeedSample(parsedSamples, repeat);
         }
      }

      private bool IsValidChannel(uint channelIdx)
      {
         if(channelIdx >= NumberOfChannels)
         {
            throw new RecoverableException("Only channels 0/1 are supported");
         }
         return true;
      }

      public override void Reset()
      {
         base.Reset();
         foreach(var c in channels)
         {
            c.Reset();
         }
      }

      public long Size => 0x50;

      public GPIO IRQ { get; } = new GPIO();
      public GPIO DMARequest { get; } = new GPIO();

      private void DefineRegisters()
      {
         Registers.Status.Define(this)
            .WithTaggedFlag("Analog watchdog flag", 0)
            .WithFlag(1, out endOfConversion, name: "Regular channel end of conversion")
            .WithTaggedFlag("Injected channel end of conversion", 2)
            .WithTaggedFlag("Injected channel start flag", 3)
            .WithTaggedFlag("Regular channel start flag", 4)
            .WithTaggedFlag("Overrun", 5)
            .WithReservedBits(6, 26);

         Registers.Control1.Define(this)
            .WithTag("Analog watchdog channel select bits", 0, 5)
            .WithFlag(5, out eocInterruptEnable, name: "Interrupt enable for EOC")
            .WithTaggedFlag("Analog watchdog interrupt enable", 6)
            .WithTaggedFlag("Interrupt enable for injected channels", 7)
            .WithFlag(8, out scanMode, name: "Scan mode")
            .WithTaggedFlag("Enable the watchdog on a single channel in scan mode", 9)
            .WithTaggedFlag("Automatic injected group conversion", 10)
            .WithTaggedFlag("Discontinuous mode on regular channels", 11)
            .WithTaggedFlag("Discontinuous mode on injected channels", 12)
            .WithTag("Discontinuous mode channel count", 13, 3)
            .WithReservedBits(16, 6)
            .WithTaggedFlag("Analog watchdog enable on injected channels", 22)
            .WithTaggedFlag("Analog watchdog enable on regular channels", 23)
            .WithTag("Resolution", 24, 2)
            .WithTaggedFlag("Overrun interrupt enable", 26)
            .WithReservedBits(27, 5);

         Registers.Control2.Define(this, name: "Control2")
            .WithFlag(0, out adcOn,
                  name: "A/D Converter ON/OFF",
                  changeCallback: (_, val) => { if(val) { EnableADC(); } else { DisableADC(); }})
            .WithFlag(1, out continuousConversion, name: "Continous conversion")
            .WithReservedBits(2, 6)
            .WithFlag(8, out dmaEnabled, name: "Direct memory access mode")
            .WithFlag(9, out dmaIssueRequest, name: "DMA disable selection")
            .WithFlag(10, out endOfConversionSelect, name: "End of conversion select")
            .WithTaggedFlag("Data Alignment", 11)
            .WithReservedBits(12, 4)
            .WithTag("External event select for injected group", 16, 4)
            .WithTag("External trigger enable for injected channels", 20, 2)
            .WithTaggedFlag("Start conversion of injected channels", 22)
            .WithReservedBits(23, 1)
            .WithEnumField(24, 4, out extSel, name: "External event select for regular group (EXTSEL)")
            .WithEnumField(28, 2, out extEn, name: "External trigger enable for regular channels (EXTEN)",
                           writeCallback: (_, value) => { UpdateExtEn(); })
            .WithFlag(30,
                  name: "Start Conversion Of Regular Channels",
                  writeCallback: (_, value) => { if(value) StartConversion(); }, // auto-cleared by H/W
                  valueProviderCallback: _ => false)
            .WithReservedBits(31, 1);

         Registers.SampleTime1.Define(this)
            .WithTag("Channel 10 sampling time", 0, 3)
            .WithTag("Channel 11 sampling time", 3, 3)
            .WithTag("Channel 12 sampling time", 6, 3)
            .WithTag("Channel 13 sampling time", 9, 3)
            .WithTag("Channel 14 sampling time", 12, 3)
            .WithTag("Channel 15 sampling time", 15, 3)
            .WithTag("Channel 16 sampling time", 18, 3)
            .WithTag("Channel 17 sampling time", 21, 3)
            .WithTag("Channel 18 sampling time", 24, 3)
            .WithReservedBits(27, 5);

         Registers.SampleTime2.Define(this)
            .WithTag("Channel 0 sampling time", 0, 3)
            .WithTag("Channel 1 sampling time", 3, 3)
            .WithTag("Channel 2 sampling time", 6, 3)
            .WithTag("Channel 3 sampling time", 9, 3)
            .WithTag("Channel 4 sampling time", 12, 3)
            .WithTag("Channel 5 sampling time", 15, 3)
            .WithTag("Channel 6 sampling time", 18, 3)
            .WithTag("Channel 7 sampling time", 21, 3)
            .WithTag("Channel 8 sampling time", 24, 3)
            .WithTag("Channel 9 sampling time", 27, 3)
            .WithReservedBits(30, 2);

         Registers.InjectedChannelDataOffset1.Define(this)
            .WithTag("Data offset for injected channel 1", 0, 12)
            .WithReservedBits(12, 20);
         Registers.InjectedChannelDataOffset2.Define(this)
            .WithTag("Data offset for injected channel 2", 0, 12)
            .WithReservedBits(12, 20);
         Registers.InjectedChannelDataOffset3.Define(this)
            .WithTag("Data offset for injected channel 3", 0, 12)
            .WithReservedBits(12, 20);
         Registers.InjectedChannelDataOffset4.Define(this)
            .WithTag("Data offset for injected channel 4", 0, 12)
            .WithReservedBits(12, 20);

         Registers.RegularSequence1.Define(this)
            .WithValueField(0, 5, out regularSequence[12], name: "13th conversion in regular sequence")
            .WithValueField(5, 5, out regularSequence[13], name: "14th conversion in regular sequence")
            .WithValueField(10, 5, out regularSequence[14], name: "15th conversion in regular sequence")
            .WithValueField(15, 5, out regularSequence[15], name: "16th conversion in regular sequence")
            .WithValueField(20, 4, writeCallback: (_, val) => { regularSequenceLen = (uint)val + 1; }, name: "Regular channel sequence length");

         Registers.RegularSequence2.Define(this)
            .WithValueField(0, 5, out regularSequence[6], name: "7th conversion in regular sequence")
            .WithValueField(5, 5, out regularSequence[7], name: "8th conversion in regular sequence")
            .WithValueField(10, 5, out regularSequence[8], name: "9th conversion in regular sequence")
            .WithValueField(15, 5, out regularSequence[9], name: "10th conversion in regular sequence")
            .WithValueField(20, 5, out regularSequence[10], name: "11th conversion in regular sequence")
            .WithValueField(25, 5, out regularSequence[11], name: "12th conversion in regular sequence");

         Registers.RegularSequence3.Define(this)
            .WithValueField(0, 5, out regularSequence[0], name: "1st conversion in regular sequence")
            .WithValueField(5, 5, out regularSequence[1], name: "2nd conversion in regular sequence")
            .WithValueField(10, 5, out regularSequence[2], name: "3rd conversion in regular sequence")
            .WithValueField(15, 5, out regularSequence[3], name: "4th conversion in regular sequence")
            .WithValueField(20, 5, out regularSequence[4], name: "5th conversion in regular sequence")
            .WithValueField(25, 5, out regularSequence[5], name: "6th conversion in regular sequence");

         // Data register
         Registers.RegularData.Define(this)
            .WithValueField(0, 32,
                  valueProviderCallback: _ =>
                  {
                      this.Log(LogLevel.Debug, "Reading ADC data {0}", adcData);
                      // Reading ADC_DR should clear EOC
                      endOfConversion.Value = false;
                      IRQ.Set(false);
                      return adcData;
                  });
      }

      private void EnableADC()
      {
          currentChannel = channels[regularSequence[currentChannelIdx].Value];
          this.Log(LogLevel.Debug, "EnableADC: currentChannelIdx {0} channel {1}", currentChannelIdx, regularSequence[currentChannelIdx].Value);
          if (scanMode.Value && !samplingTimer.Enabled)
          {
              StartConversion(); // really a restart
          }
      }

      private void DisableADC()
      {
          this.Log(LogLevel.Debug, "DisableADC: setting currentChannelIdx = 0");
          currentChannelIdx = 0;
          // NOTE: RM0033 rev9 10.3.1 : we should stop conversion if ADC disabled

          // If the internal timer is enabled then
          // OnConversionFinished is repeatedly called from the
          // samplingTimer LimitReached support; (samplingTimer is not
          // needed when we are using an EXTSEL trigger for
          // conversion).

          samplingTimer.Enabled = false;
          currentChannel = null;
      }

      private void StartConversion()
      {
         if(adcOn.Value)
         {
             this.Log(LogLevel.Debug, "Starting conversion time={0}",
                   machine.ElapsedVirtualTime.TimeElapsed);

             if (ExtEn.Disabled == extEn.Value)
             {
                 // Enable timer, which will simulate conversion being performed.
                 samplingTimer.Enabled = true;
             }
         }
         else
         {
             this.Log(LogLevel.Warning, "Trying to start conversion while ADC off");
         }
      }

      private void OnConversionFinished()
      {
	 this.Log(LogLevel.Debug, "OnConversionFinished: time={0} currentChannelIdx {1} regularSequenceLen {2} channel {3}",
		  machine.ElapsedVirtualTime.TimeElapsed,
		  currentChannelIdx, regularSequenceLen, regularSequence[currentChannelIdx].Value);

         // Extra diagnostics to make tracking enabled channels easier:
         //for (var rs = 0; (rs < regularSequenceLen); rs++)
         //{
         //    this.Log(LogLevel.Debug, "OnConversionFinished: regularSequence[{0}] {1}", rs, regularSequence[rs].Value);
         //}

         if (null == currentChannel)
         {
             this.Log(LogLevel.Debug, "OnConversionFinished: early exit since currentChannel == null");
             return;
         }

         // Set data register and trigger DMA request
         currentChannel.PrepareSample();
         adcData = currentChannel.GetSample();
         this.Log(LogLevel.Debug, "OnConversionFinished: adcData=0x{0:X} dmaEnabled {1} dmaIssueRequest {2}", adcData, dmaEnabled.Value, dmaIssueRequest.Value);

         if(dmaEnabled.Value && dmaIssueRequest.Value)
         {
            // Issue DMA peripheral request, which when mapped to DMA
            // controller will trigger a peripheral to memory transfer
            DMARequest.Set();
            DMARequest.Unset();
         }

         this.Log(LogLevel.Debug, "OnConversionFinished: regularSequenceLen={0}", regularSequenceLen);

         var scanModeActive = scanMode.Value && currentChannelIdx <= regularSequenceLen - 1;
         var scanModeFinished = scanMode.Value && currentChannelIdx == regularSequenceLen - 1;

         this.Log(LogLevel.Debug, "OnConversionFinished: scanModeActive={0} scanModeFinished={1}", scanModeActive, scanModeFinished);

         // Signal EOC if EOCS set with scan mode enabled and finished or we finished scanning regular group
         endOfConversion.Value = scanModeActive ? (endOfConversionSelect.Value || scanModeFinished) : true;

         this.Log(LogLevel.Debug, "OnConversionFinished: endOfConversion.Value={0}", endOfConversion.Value);

         if(0 != regularSequenceLen)
         {
             // Iterate to next channel
             currentChannelIdx = (currentChannelIdx + 1) % regularSequenceLen;
             currentChannel = channels[regularSequence[currentChannelIdx].Value];
         }
         // currentChannel channelId is private so we cannot easily verify the correct channel
         this.Log(LogLevel.Debug, "OnConversionFinished: currentChannelIdx={0} channel={1} continuousConversion {2}", currentChannelIdx,
		  regularSequence[currentChannelIdx].Value,
		  continuousConversion.Value);

         // Auto trigger next conversion if we're scanning or CONT bit set
         if (ExtEn.Disabled == extEn.Value)
         {
             samplingTimer.Enabled = scanModeActive || continuousConversion.Value;
         }

         // Trigger EOC interrupt
         if(endOfConversion.Value && eocInterruptEnable.Value)
         {
            this.Log(LogLevel.Debug, "OnConversionFinished: Set IRQ");
            IRQ.Set(true);
         }
      }

      // Control 1/2 fields
      private IFlagRegisterField scanMode;
      private IFlagRegisterField endOfConversion;
      private IFlagRegisterField adcOn;
      private IFlagRegisterField endOfConversionSelect;
      private IFlagRegisterField eocInterruptEnable;
      private IFlagRegisterField continuousConversion;

      private IFlagRegisterField dmaEnabled;
      private IFlagRegisterField dmaIssueRequest;

      private IEnumRegisterField<ExtSel> extSel;
      private IEnumRegisterField<ExtEn> extEn;

      // Sampling timer. Provides time-based event for driving conversion of
      // regular channel sequence.
      private readonly LimitTimer samplingTimer;

      // Data sample to be returned from data register when read.
      private uint adcData;

      // Regular sequence settings, i.e. the channels and order of channels
      // for performing conversion
      private uint regularSequenceLen;
      private readonly IValueRegisterField[] regularSequence = new IValueRegisterField[19];

      // Channel objects, for managing input test data
      private uint currentChannelIdx; // index into regularSequence[] vector
      private ADCChannel currentChannel;
      private readonly ADCChannel[] channels;

      public const int NumberOfChannels = 19;

      private enum ExtSel
      {
           TIM1_CH1  = 0,
           TIM1_CH2  = 1,
           TIM1_CH3  = 2,
           TIM2_CH2  = 3,
           TIM2_CH3  = 4,
           TIM2_CH4  = 5,
           TIM2_TRGO = 6,
           TIM3_CH1  = 7,
           TIM3_TRGO = 8,
           TIM4_CH4  = 9,
           TIM5_CH1  = 10,
           TIM5_CH2  = 11,
           TIM5_CH3  = 12,
           TIM8_CH1  = 13,
           TIM8_TRGO = 14,
           EXTI_11   = 15,
      }

      private enum ExtEn
      {
          Disabled    = 0, // Trigger detection disabled
          EdgeRising  = 1, // Detection on the rising edge
          EdgeFalling = 2, // Detection on the falling edge
          EdgeBoth    = 3, // Detection on both the rising and falling edges
      }

      // Common registers (TODO)
      // ADC_CSR = 0x300, // read-only overview of all controller status bits
      // ADC_CCR = 0x304, // common ADC clock setup, etc.
      // ADC_CDR = 0x308, // read-only: dual and triple mode data access register

      // ADCx registers
      private enum Registers
      {
         Status = 0x0,
         Control1 = 0x4,
         Control2 = 0x8,
         SampleTime1 = 0x0C,
         SampleTime2 = 0x10,
         InjectedChannelDataOffset1 = 0x14,
         InjectedChannelDataOffset2 = 0x18,
         InjectedChannelDataOffset3 = 0x1C,
         InjectedChannelDataOffset4 = 0x20,
         WatchdogHigherThreshold = 0x24,
         WatchdogLowerThreshold = 0x28,
         RegularSequence1 = 0x2C,
         RegularSequence2 = 0x30,
         RegularSequence3 = 0x34,
         InjectedSequence = 0x38,
         InjectedData1 = 0x3C,
         InjectedData2 = 0x40,
         InjectedData3 = 0x44,
         InjectedData4 = 0x48,
         RegularData = 0x4C
      }
   }
}