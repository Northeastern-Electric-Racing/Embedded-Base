#!/bin/bash

# this script has to be sourced!!!

ody_connect_main() {
    if [[ -z "${USBIP_PORT}" || -z "${PROJECT_NAME}" ]]
    then
        echo "This project doesnt use USB/IP"
        return 1
    fi

    # check if symlink exits and can be resolved to the actual device
    if [ -L "/dev/tty$PROJECT_NAME" ] && [ -e "/dev/tty$PROJECT_NAME" ]
    then
        echo "Found /dev/tty$PROJECT_NAME, setting it to be used by container"
        DEBUG_PORT="/dev/tty$PROJECT_NAME"
        return 0
    else
        echo "Didn't find existing tty USB/IP connected"
        rm "/dev/tty$PROJECT_NAME"
    fi


    # do not allow inherit of prev targets
    local prev_targets=$(ls /dev/tty*)

    if usbip attach -r 192.168.100.12 -b "$USBIP_PORT"
    then
        echo "Successfully attached, linking to /dev/tty$PROJECT_NAME"
        
        # add a sleep time due to a race condition of the population of dev
        sleep 5s
        
        # find the new target in the dev fs and symlink it to project name by diffing it
        ln -s $(diff -n  <(echo "$prev_targets")  <(ls /dev/tty*) | tail -n 1) "/dev/tty$PROJECT_NAME"
        
        # set new debug port variable (inherited)
        DEBUG_PORT="/dev/tty$PROJECT_NAME"
        
        echo "Done!"
    else
        echo "Failed to connect, please see below if any USB devices are available"
        usbip list -r 192.168.100.12
        return 1
    fi
}

# run kmain then remove it from env
ody_connect_main
unset -f ody_connect_main
