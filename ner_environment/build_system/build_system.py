# ==============================================================================
# Northeastern Electric Racing Firmware Build System
# v0.1 10.07.2024
# Original Author: Dylan Donahue
# Last Modified: 10.08.2024 by Dylan Donahue
#
# This script defines a custom build system wrapper for our combined docker and python virtual environment build system.
# Provided here is a comprehensive set of commands that allow for the complete interaction with NERs toolset.
# Each command is defined as a function and is called by the main function, and has help messages and configuration options.
# Commands can be run with the virtual environment active by prefixing the command with `ner <command>`.
# 
# See more documentation at <confluence link>
#
# To see a list of available commands and additional configuration options, run `ner --help`
# ==============================================================================
import argparse
import subprocess
import sys
import glob
import os

# custom modules for functinality that is too large to be included in this script directly
from .miniterm import main as miniterm

# ==============================================================================
# Build command
# ==============================================================================

def build(args):

    if args.clean:
        command = ["docker", "compose", "run", "--rm", "ner-gcc-arm", "make", "clean"]
    else:
        command = ["docker", "compose", "run", "--rm", "ner-gcc-arm", "make"]
    run_command(command)

# ==============================================================================
# Clang command
# ==============================================================================

def clang(args):
    if args.disable:
        command = ["pre-commit", "uninstall"]
    elif args.enable:
        command = ["pre-commit", "install"]
    elif args.run:
        command = ["pre-commit", "run", "--all-files"]
    else:
        print("Error: No valid option specified")
        sys.exit(1)
        
    run_command(command)

# ==============================================================================
# Debug command
# ==============================================================================

def debug(args):
    pass

# ==============================================================================
# Flash command
# ==============================================================================

def flash(args):

    if args.docker:
        
        command = [
            "docker", "run", "--rm",
            "-v", "/home/app/build:/build",  
            "ner-gcc-arm",
            "sh", "-c", "flash"
        ]

    else:
        build_directory = os.path.join("build", "*.elf")
        elf_files = glob.glob(build_directory)

        if not elf_files:
            print("Error: No ELF file found in ./build/")
            sys.exit(1)

        elf_file = elf_files[0]  # Take the first ELF file found
        print(f"Found ELF file: {elf_file}")

        
        command = [
            "openocd",
            "-f", "interface/cmsis-dap.cfg",
            "-f", f"target/{args.device}.cfg",
            "-c", "adapter speed 5000",
            "-c", f"program {elf_file} verify reset exit"
        ]

    run_command(command)


# ==============================================================================
# Serial command
# ==============================================================================

def serial(args):
    miniterm()

# ==============================================================================
# Update command
# ==============================================================================

def update(args):
    pass

# ==============================================================================
# USBIP command
# ==============================================================================

def usbip(args):
    if args.connect:
        if args.device is None:
            print("Error --device must be specified when using --connect")
        
        disconnect_usbip() # Disconnect the current USB device
        
        if args.device == "shep" or args.device == "shepherd":
            commands = [
                ["sudo", "modprobe", "vhci_hcd"],
                ["sudo", "usbip", "attach", "-r", "192.168.100.12", "-b", "1-1.4"]
            ]

        elif args.device == "cerb" or args.device == "cerberus":
            commands = [
                 ["sudo", "modprobe", "vhci_hcd"],
                ["sudo", "usbip", "attach", "-r", "192.168.100.12", "-b", "1-1.3"]
            ]
        
        else:
            print("Error: Invalid device name")
            sys.exit(1)
    
    elif args.disconnect:
        disconnect_usbip()

# ==============================================================================
# Main function
# ==============================================================================

def main():

    parser = argparse.ArgumentParser(
        description="NER: Embedded Build System", 
        formatter_class=argparse.ArgumentDefaultsHelpFormatter
    )

    subparsers = parser.add_subparsers(
        title="Commands", 
        description="Available commands: build, flash, debug, update, usbip",
        dest="command",
        required=True
    )

    # ==============================================================================

    parser_build = subparsers.add_parser('build', help="Build the project with CMake")
    parser_build.add_argument(
        '--profile', 
        type=str, 
        help="Specify the build profile (e.g., debug, release)", 
        default="debug"
    )
    parser_build.add_argument(
        '--clean', 
        action="store_true", 
        help="Clean the build directory before building"
    )
    parser_build.set_defaults(func=build)

    # ==============================================================================

    # ==============================================================================

    parser_clang = subparsers.add_parser('clang', help="Configure autoformatter settings")
    parser_clang.add_argument(
        '--disable',
        action="store_true",
        help="Disable clang-format"
    )
    parser_clang.add_argument(
        '--enable',
        action="store_true",
        help="Enable clang-format"
    )
    parser_clang.add_argument(
        '--run',
        action="store_true",
        help="Run clang-format"
    )
    parser_clang.set_defaults(func=clang)

    # ==============================================================================

    # ==============================================================================

    parser_flash = subparsers.add_parser('flash', help="Flash the firmware")
    parser_flash.add_argument(
        '--device', 
        type=str, 
        help="Specify the target device (e.g., stm32f405, stm32f407)", 
        default="stm32f405"
    )
    parser_flash.add_argument(
        '--docker', 
        action="store_true", 
        help="Use OpenOCD in the container instead of locally, requires linux"
    )
    parser_flash.set_defaults(func=flash)

    # ==============================================================================

    # ==============================================================================

    parser_debug = subparsers.add_parser('debug', help="Start a debug session")
    parser_debug.set_defaults(func=debug)

    # ==============================================================================

    # ==============================================================================

    parser_usbip = subparsers.add_parser('usbip', help="Connect or disconnect USB devices over IP")
    parser_usbip.add_argument(
        '--connect',
        action="store_true",
        help="Connect a USB device"
    )
    parser_usbip.add_argument(
        '--disconnect',
        action="store_true",
        help="Disconnect a USB device"
    )
    parser_usbip.add_argument(
        '--device',
        type=str,
        help="Specify the device to connect or disconnect (e.g., shepherd, cerberus)"
    )
    parser_usbip.set_defaults(func=usbip)

    # ==============================================================================

    args = parser.parse_args()
    args.func(args)
    
# ==============================================================================
# Helper functions - not direct commands
# ==============================================================================

def run_command(command):
    """Run a shell command."""
    try:
        result = subprocess.run(command, check=True, capture_output=True, text=True)
        print(result.stdout)
    except subprocess.CalledProcessError as e:
        print(f"Error occurred: {e}", file=sys.stderr)
        print(e.stderr, file=sys.stderr)
        sys.exit(e.returncode)

def disconnect_usbip():
    """Disconnect the current USB device."""
    command = ["sudo", "usbip", "detach", "-p", "0"]
    run_command(command)


if __name__ == "__main__":
    main()


 
