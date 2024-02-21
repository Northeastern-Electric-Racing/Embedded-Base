#!/bin/bash

if [ $n -e /home/app/shepherd2.ioc ]
then 
    usbip attach -r 192.168.100.12 -b 1-1.4
elif [ $n -e /home/app/cerberus.ioc ]
then 
    usbip attach -r 192.168.100.12 -b 1-1.3
else
    echo "No project found, to connect to usb port X at tpu IP P: usbip attach -r P -b 1-1.X"
fi
