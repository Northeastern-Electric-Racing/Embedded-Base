#! /bin/bash
elf_file=$(ls /home/app/build/*.elf)

printf "$elf_file"

openocd -f interface/cmsis-dap.cfg -f target/"$STM_TARGET_NAME"x.cfg -c "adapter speed 5000" -c "cmsis_dap_serial serial_string $(curr-serial)" -c "program $elf_file verify reset exit"
