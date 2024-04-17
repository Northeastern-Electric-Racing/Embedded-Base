#!/bin/bash

serial_num=$(udevadm info "$DEBUG_PORT" -a | sed -n -e 's/^.*ATTRS{serial}=="//p' | head -n 1 | sed 's/.$//')

echo "$serial_num"
