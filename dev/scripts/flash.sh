#! /bin/bash
elf_file=$(ls /home/app/build/*.elf)

printf "$elf_file"

openocd -f interface/cmsis-dap.cfg -f target/stm32f4x.cfg -c "adapter speed 5000" -c "program $elf_file verify reset exit"