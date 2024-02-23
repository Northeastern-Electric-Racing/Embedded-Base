if [ $n -e /home/app/shepherd2.ioc ]; then echo \
" ______     __  __     ______     ______   __  __     ______     ______     _____
/\  ___\   /\ \_\ \   /\  ___\   /\  == \ /\ \_\ \   /\  ___\   /\  == \   /\  __-.
\ \___  \  \ \  __ \  \ \  __\   \ \  _-/ \ \  __ \  \ \  __\   \ \  __<   \ \ \/\ \ 
 \/\_____\  \ \_\ \_\  \ \_____\  \ \_\    \ \_\ \_\  \ \_____\  \ \_\ \_\  \ \____-
  \/_____/   \/_/\/_/   \/_____/   \/_/     \/_/\/_/   \/_____/   \/_/ /_/   \/____/"; fi;

if [ $n -e /home/app/cerberus.ioc ]; then echo \
"_________             ___
\_   ___ \  __________\_ |__   ___________ __ __  ______
/    \  \/_/ __ \_  __ \ __ \_/ __ \_  __ \  |  \/  ___/
\     \___\  ___/|  | \/ \_\ \  ___/|  | \/  |  /\___ \ 
 \______  /\___  >__|  |___  /\___  >__|  |____//____  >
        \/     \/          \/     \/                 \/ "; fi;

if [ $n -e /home/app/proteus.ioc ]; then echo \
" _______  ______    _______  _______  _______  __   __  _______
|       ||    _ |  |       ||       ||       ||  | |  ||       |
|    _  ||   | ||  |   _   ||_     _||    ___||  | |  ||  _____|
|   |_| ||   |_||_ |  | |  |  |   |  |   |___ |  |_|  || |_____
|    ___||    __  ||  |_|  |  |   |  |    ___||       ||_____  |
|   |    |   |  | ||       |  |   |  |   |___ |       | _____| |
|___|    |___|  |_||_______|  |___|  |_______||_______||_______|"; fi;

if [ $n -e /home/app/Charger-FW.ioc ]; then echo \
" /###### /#######   /######  /##   /##
|_  ##_/| ##__  ## /##__  ##| ##  | ##
  | ##  | ##  \ ##| ##  \ ##| ##  | ##
  | ##  | #######/| ##  | ##| ########
  | ##  | ##__  ##| ##  | ##| ##__  ##
  | ##  | ##  \ ##| ##  | ##| ##  | ##
 /######| ##  | ##|  ######/| ##  | ##
|______/|__/  |__/ \______/ |__/  |__/"; fi;

alias serial='/home/dev/scripts/serial.sh'
alias flash='/home/dev/scripts/flash.sh'
alias emulate='/home/dev/scripts/emulate.sh'
alias debug='/home/dev/scripts/gdb.sh'
alias ody-connect='/home/dev/scripts/ody-connect.sh'
