# ==============================================================================
# Northeastern Electric Racing Firmware Build System
# v0.1 10.07.2024
# Original Author: Dylan Donahue
# Last Modified: 10.15.2024 by Dylan Donahue
#
# This script defines a custom build system wrapper for our combined docker and python virtual environment build system.
# Provided here is a comprehensive set of commands that allow for the complete interaction with NERs toolset.
# Each command is defined as a function and is called by the main function, and has help messages and configuration options.
# Commands can be run with the virtual environment active by prefixing the command with `ner <command>`.
# 
# See more documentation at https://nerdocs.atlassian.net/wiki/spaces/NER/pages/611516420/NER+Build+System
#
# To see a list of available commands and additional configuration options, run `ner --help`
# ==============================================================================
import shutil
import typer
from rich import print
import platform
import subprocess
import sys
import os
import glob
import time
from pathlib import Path

# custom modules for functinality that is too large to be included in this script directly
from .miniterm import main as miniterm

# ==============================================================================
# Typer application setup
# ==============================================================================
    
app = typer.Typer(help="Northeastern Electric Racing Firmware Build System",
                  epilog="For more information, visit https://nerdocs.atlassian.net/wiki/spaces/NER/pages/524451844/2024+Firmware+Onboarding+Master",
                  add_completion=False)

lp_app = typer.Typer(help="Install configure, and run launchpad environment items")
app.add_typer(lp_app, name="lp")

def unsupported_option_cb(value:bool):
    if value:
        print("[bold red] WARNING: the selected option is not currently implemented. This is either because is is an planned or deprecated feature")
        raise typer.Exit()
      
# ==============================================================================
# Build command
# ==============================================================================

@app.command(help="Build the project with GCC ARM Toolchain and Make/CMake")
def build(profile: str = typer.Option(None, "--profile", "-p", callback=unsupported_option_cb, help="(planned) Specify the build profile (e.g., debug, release)", show_default=True),
          clean: bool = typer.Option(False, "--clean", help="Clean the build directory before building", show_default=True)):

    is_cmake = os.path.exists("CMakeLists.txt")

    if is_cmake: # Repo uses CMake, so execute CMake commands.
        print("[bold blue]CMake project detected.")
        
        # If CMakeCache.txt doesn't exist or has wrong paths, configure CMake
        if not os.path.exists("build/CMakeCache.txt"):
            print("[blue]No CMake cache found. Configuring CMake...[/blue]")
            run_command(["docker", "compose", "run", "--rm", "ner-gcc-arm", "bash", "-c", "cd /home/app && mkdir -p build && cd build && cmake .."], stream_output=True)
        
        if clean:
            run_command(["docker", "compose", "run", "--rm", "ner-gcc-arm", "cmake", "--build", "build", "--target", "clean"], stream_output=True)
        else:
            run_command(["docker", "compose", "run", "--rm", "ner-gcc-arm", "cmake", "--build", "build"], stream_output=True)
    else: # Repo uses Make, so execute Make commands.
        print("[bold blue]Makefile project detected.")
        if clean:
            run_command(["docker", "compose", "run", "--rm", "ner-gcc-arm", "make", "clean"], stream_output=True)
        else:
            run_command(["docker", "compose", "run", "--rm", "ner-gcc-arm", "make", f"-j{os.cpu_count()}"], stream_output=True)

# ==============================================================================
# Clang command
# ==============================================================================

@app.command(help="Configure autoformatter settings")
def clang(disable: bool = typer.Option(False, "--disable","-d", help="Disable clang-format"),
          enable: bool = typer.Option(False, "--enable", "-e", help="Enable clang-format"),
          run: bool = typer.Option(False, "--run", "-r", help="Run clang-format")):
    
    if disable:
        command = ["pre-commit", "uninstall"]
    elif enable:
        command = ["pre-commit", "install"]
    elif run:
        command = ["pre-commit", "run", "--all-files"]
    else:
        print("[bold red] Error: No valid option specified")
        sys.exit(1)
        
    run_command(command)

# ==============================================================================
# Debug command
# ==============================================================================

@app.command(help="Start a debug session")
def debug(ftdi: bool = typer.Option(False, "--ftdi", help="DEPRECATED (On by default) Set this flag if the device uses an FTDI chip"),
        no_ftdi: bool = typer.Option(False, "--no-ftdi", help="Set this flag if the device uses an CMSIS DAP chip"),
          docker: bool = typer.Option(False, "--docker", callback=unsupported_option_cb, help="(deprecated) Use OpenOCD in the container instead of locally, requires linux")):

    command = ["openocd"]
    current_directory = os.getcwd()

    if not no_ftdi:
        ftdi_path = os.path.join(current_directory, "Drivers", "Embedded-Base", "ftdi_flash.cfg")
        command = command + ["-f", ftdi_path]
    else:
        command = command + ["-f", "interface/cmsis-dap.cfg"]

    build_directory = os.path.join("build", "*.elf")
    elf_files = glob.glob(build_directory)
    if not elf_files:
        print("[bold red] Error: No ELF file found in ./build/")
        sys.exit(1)

    elf_file = os.path.basename(os.path.normpath(elf_files[0]))  # Take the first ELF file found
    print(f"[bold blue] Found ELF file: [/bold blue] [blue] {elf_file}")

    halt_command = command + ["-f", "flash.cfg", "-f", os.path.join(current_directory, "Drivers", "Embedded-Base", "openocd.cfg"),
                              "-c", "init", "-c", "reset halt"]
    
    ocd = subprocess.Popen(halt_command)
    time.sleep(1)
    
    # for some reason the host docker internal thing is broken on linux despite compose being set correctly, hence this hack
    # TODO: fix this properly

    gdb_uri  = "host.docker.internal"
    if platform.system() == "Linux" and is_wsl() == 0:
        gdb_uri = "localhost"

    send_command = ["docker", "compose", "run", "--rm", "ner-gcc-arm", "gdb-multiarch", f"/home/app/build/{elf_file}", "-ex", f"target extended-remote {gdb_uri}:3333"]

    subprocess.run(send_command)

    # make terminal clearer
    time.sleep(4)
    print("\nKilling openocd...")
    ocd.terminate()
    time.sleep(1)

# ==============================================================================
# Flash command
# ==============================================================================

@app.command(help="Flash the firmware")
def flash(ftdi: bool = typer.Option(False, "--ftdi", help="DEPRECATED (On by default): Set this flag if the device uses an FTDI chip"),
        no_ftdi: bool = typer.Option(False, "--no-ftdi", help="Set this flag if the device uses an CMSIS DAP chip"),
          docker: bool = typer.Option(False, "--docker", help="Use OpenOCD in the container instead of locally, requires linux")):

    command = []

    if docker and ftdi:
        print("[bold red] Cannot flash ftdi from docker")
        sys.exit(1)

    if docker:
        command = ["docker", "compose", "run", "--rm", "ner-gcc-arm"]

    command = command + ["openocd"]

    if not no_ftdi:
        current_directory = os.getcwd()
        ftdi_path = os.path.join(current_directory, "Drivers", "Embedded-Base", "ftdi_flash.cfg")
        command = command + ["-f", ftdi_path]
    else:
        command = command + ["-f", "interface/cmsis-dap.cfg"]
    

    build_directory = os.path.join("build", "*.elf")
    elf_files = glob.glob(build_directory)
    if not elf_files:
        print("[bold red] Error: No ELF file found in ./build/")
        sys.exit(1)

    elf_file = elf_files[0]  # Take the first ELF file found
    print(f"[bold blue] Found ELF file: [/bold blue] [blue] {elf_file}")

    command = command + ["-f", "flash.cfg", "-c", f"program {elf_file} verify reset exit"]
    
    run_command(command, stream_output=True)

# ==============================================================================
# Serial command
# ==============================================================================

@app.command(help="Open UART terminal of conneced device")
def serial(ls: bool = typer.Option(False, "--list", help='''Specify the device to connect or disconnect (e.g., /dev/ttyACM0,/dev/ttyUSB0,/dev/ttyUSB1,COM1)
                                                            Mandatory on MacOS, may be optional on otherwise. On Mac, find this by running `ls /dev/tty.usb*' '''),
           device: str = typer.Option("", "--device", "-d", help="Specify the board to connect to")):

    if ls:
        miniterm(ls=True, device=device)
    else:
        miniterm(device=device)

# ==============================================================================
# Update command
# ==============================================================================

@app.command(help="Update the ner_environment package")
def update():

    dir = os.getcwd()
    if "bedded" in dir:
        command = ["pip", "install", "-e", "ner_environment"]

    else:  
        # app folder - go up one level to root                                      
        if os.path.exists(os.path.join(dir, "Drivers", "Embedded-Base")):
            os.chdir("..")


        # here from app or started from root
        if contains_subdir(os.getcwd(), "bedded"):
            command = ["pip", "install", "-e", os.path.join("Embedded-Base", "ner_environment")]
        else:
            print("[bold red] Error: No ner_environment found in current directory. Run from NER root, app, or Embedded-Base directories")
            sys.exit(1)

    run_command(command)
    
# ==============================================================================
# USBIP command
# ==============================================================================

@app.command(help="Interact with the USB over IP system")
def usbip(connect: bool = typer.Option(False, "--connect", help="Connect to a USB device"),
          disconnect: bool = typer.Option(False, "--disconnect", help="Disconnect the connected USB device"),
          device: str = typer.Option(None, "--device", "-d", help="Specify the device to connect or disconnect (e.g., shepherd, cerberus)")):

    if connect:
        if device is None:
            print("[bold red] Error --device must be specified when using --connect")
        
        disconnect_usbip() # Disconnect the current USB device
        
        if device == "shep" or device == "shepherd":
            commands = [
                ["sudo", "modprobe", "vhci_hcd"],
                ["sudo", "usbip", "attach", "-r", "192.168.100.12", "-b", "1-1.4"]
            ]

        elif device == "cerb" or device == "cerberus":
            commands = [
                 ["sudo", "modprobe", "vhci_hcd"],
                ["sudo", "usbip", "attach", "-r", "192.168.100.12", "-b", "1-1.3"]
            ]
        
        else:
            print("[bold red] Error: Invalid device name")
            sys.exit(1)
        
        run_command(commands[0])
        run_command(commands[1])
    
    elif disconnect:
        disconnect_usbip()

# ==============================================================================
# ---- Launchpad Section
# ==============================================================================


# ==============================================================================
# init
# ==============================================================================
@lp_app.command(help="Initialize launchpad, run before any commands")
def install():
    """Install PlatformIO package."""
    try:
        # Install the platformio package
        subprocess.check_call(['pip', 'install', 'platformio'])

    except subprocess.CalledProcessError as e:
        print(f"Failed to install PlatformIO: {e}", file=sys.stderr)
        sys.exit(1)

# ==============================================================================
# uninstall
# ==============================================================================
@lp_app.command(help="Remove launchpad")
def uninstall():
    """Uninstall PlatformIO package."""
    try:
        # Install the platformio package
        subprocess.check_call(['pip', 'uninstall', '-y', 'platformio'])

        platformio_dir = os.path.expanduser('~/.platformio')
        if os.path.isdir(platformio_dir):
            shutil.rmtree(platformio_dir)
            print("PlatformIO directory removed.")
        else:
            print("PlatformIO directory does not exist.")

    except subprocess.CalledProcessError as e:
        print(f"Failed to uninstall PlatformIO: {e}", file=sys.stderr)
        sys.exit(1)

# ==============================================================================
# build
# ==============================================================================
@lp_app.command(help="Build your launchpad project")
def build():
    subprocess.run(['platformio', 'run'])

# ==============================================================================
# flash
# ==============================================================================
@lp_app.command(help="Flash your launchpad project to a board")
def flash():
    subprocess.run(['platformio', 'run', '--target', 'upload'])

# ==============================================================================
# serial
# ==============================================================================
@lp_app.command(help="View serial output from the device")
def serial():
    subprocess.run(['platformio', 'device', 'monitor'])
    

# ==============================================================================
# ---- End Launchpad Section
# ==============================================================================


# ==============================================================================
# Helper functions - not direct commands
# ==============================================================================

def run_command(command, stream_output=False, exit_on_fail=True):
    """Run a shell command. Optionally stream the output in real-time."""
    
    if stream_output:
     
        process = subprocess.Popen(command, text=True)
  
        returncode = process.wait()
        if returncode != 0:
            print(f"Error: Command exited with code {returncode}", file=sys.stderr)
            if exit_on_fail:
                sys.exit(returncode)
    
    else:
        try:
            result = subprocess.run(command, check=True, capture_output=True, text=True)
            print(result.stdout)
        except subprocess.CalledProcessError as e:
            print(f"Error occurred: {e}", file=sys.stderr)
            print(e.stderr, file=sys.stderr)
            if exit_on_fail:
                sys.exit(e.returncode)

def disconnect_usbip():
    """Disconnect the current USB device."""
    command = ["sudo", "usbip", "detach", "-p", "0"]
    run_command(command, exit_on_fail=False)

def is_wsl() -> int:
    """Detects if Python is running in WSL by checking the system name."""
    if platform.system() == "Linux" and "microsoft" in platform.uname().release.lower():
        return 2  # WSL2
    return 0  # Not WSL

def contains_subdir(base_path, search_str):
    base_dir = Path(base_path)
    for item in base_dir.iterdir():
        if item.is_dir() and search_str in item.name:
            return True
    return False

# ==============================================================================
# Entry
# ==============================================================================

if __name__ == "__main__":
    app(prog_name="ner")


 
