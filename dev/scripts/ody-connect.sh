#!/bin/bash

# this script has to be sourced

if [[ -z "${USBIP_PORT}" || -z "${PROJECT_NAME}" ]]
then
    echo "This project doesnt use USB/IP"
    return 1
fi

if ls /dev/tty* | grep -q "$PROJECT_NAME" && [[ ! $(wc -l <(usbip port) | head -c 1) -eq 2 ]]
then
    echo "Found /dev/tty$PROJECT_NAME, setting it to be used by container"
    DEBUG_PORT="/dev/tty$PROJECT_NAME"
    return 0
else
    echo "Didn't find existing tty USB/IP connected"
    rm "/dev/tty$PROJECT_NAME"
fi


PREV_LIST_OF_TARGETS=$(ls /dev/tty*)

if usbip attach -r 192.168.100.12 -b "$USBIP_PORT"
then
    echo "Successfully attached, linking to /dev/tty$PROJECT_NAME"
    # find the new target in the dev fs and symlink it to project name
    ln -s /dev/tty$(diff -n  <(echo "$PREV_LIST_OF_TARGETS")  <(ls /dev/tty*) | tail -n 1) /dev/tty"$PROJECT_NAME"
    DEBUG_PORT="/dev/tty$PROJECT_NAME"
else
    echo "Failed to connect, please see below if any USB devices are available"
    usbip list -r 192.168.100.12
    return 1
fi
