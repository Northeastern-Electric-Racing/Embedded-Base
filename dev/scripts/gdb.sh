#! /bin/bash
elf_file=$(ls /home/app/build/*.elf)

printf "$elf_file"

openocd -f interface/cmsis-dap.cfg -f target/"$STM_TARGET_NAME"x.cfg -c "adapter speed 5000" -c "cmsis_dap_serial $(curr-serial)" -c "init" -c "reset halt" & sleep 1 && arm-none-eabi-gdb $elf_file -ex "target remote localhost:3333"

kill $(pidof openocd)
