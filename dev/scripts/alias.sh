cat "/home/app/art"

alias serial='/home/dev/scripts/serial.sh'
alias flash='/home/dev/scripts/flash.sh'
alias emulate='/home/dev/scripts/emulate.sh'
alias debug='/home/dev/scripts/gdb.sh'
alias ody-connect='source /home/dev/scripts/ody-connect.sh'

function curr-serial() {
    udevadm info "$DEBUG_PORT" -a | sed -n -e 's/^.*ATTRS{serial}=="//p' | head -n 1 | sed 's/.$//'
}

export -f curr-serial
