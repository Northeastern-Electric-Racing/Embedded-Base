#! /bin/bash
openocd -f interface/cmsis-dap.cfg -f target/stm32f4x.cfg -c "adapter speed 5000" -c "init" -c "reset halt" & sleep 1 && arm-none-eabi-gdb /home/app/build/*.elf -ex "target remote localhost:3333"
