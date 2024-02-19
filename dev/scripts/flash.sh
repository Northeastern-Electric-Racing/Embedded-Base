#! /bin/bash
openocd -f interface/cmsis-dap.cfg -f target/stm32f4x.cfg -c "adapter speed 5000" -c "program /home/app/build/*.elf verify reset exit"
