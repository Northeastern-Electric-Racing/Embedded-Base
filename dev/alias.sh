if [ $n -e /home/app/shepherd2.ioc ]; then echo \
" ______     __  __     ______     ______   __  __     ______     ______     _____\n\
/\  ___\   /\ \_\ \   /\  ___\   /\  == \ /\ \_\ \   /\  ___\   /\  == \   /\  __-.\n\
\ \___  \  \ \  __ \  \ \  __\   \ \  _-/ \ \  __ \  \ \  __\   \ \  __<   \ \ \/\ \ \n\
 \/\_____\  \ \_\ \_\  \ \_____\  \ \_\    \ \_\ \_\  \ \_____\  \ \_\ \_\  \ \____- \n\
  \/_____/   \/_/\/_/   \/_____/   \/_/     \/_/\/_/   \/_____/   \/_/ /_/   \/____/"; fi;

if [ $n -e /home/app/cerberus.ioc ]; then echo \
"_________             ___.               \n\
\_   ___ \  __________\_ |__   ___________ __ __  ______\n\
/    \  \/_/ __ \_  __ \ __ \_/ __ \_  __ \  |  \/  ___/\n\
\     \___\  ___/|  | \/ \_\ \  ___/|  | \/  |  /\___ \ \n\
 \______  /\___  >__|  |___  /\___  >__|  |____//____  >\n\
        \/     \/          \/     \/                 \/ ";

alias serial='minicom -b 115200 -o -D /dev/ttyACM0'
alias flash='openocd -f interface/cmsis-dap.cfg -f target/stm32f4x.cfg -c "adapter speed 5000" -c "program /home/app/build/cerberus.elf verify reset exit"'
alias emulate='renode /home/app/*.resc'
alias debug='openocd -f interface/cmsis-dap.cfg -f target/stm32f4x.cfg -c \ "adapter speed 5000\" -c \ "init" -c "reset halt" & sleep 1 && arm-none-eabi-gdb /home/app/build/cerberus.elf -ex "target remote localhost:3333"'