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
from enum import Enum
import shutil
import requests
import typer
from rich import print
import platform
import subprocess
import sys
import os
import glob
import re
import time
import json
from pathlib import Path

# custom modules for functinality that is too large to be included in this script directly
from .miniterm import main as miniterm
from .serial2 import main as serial2_start

# ==============================================================================
# Typer application setup
# ==============================================================================

app = typer.Typer(help="Northeastern Electric Racing Firmware Build System",
                  epilog="For more information, visit https://nerdocs.atlassian.net/wiki/spaces/NER/pages/524451844/2024+Firmware+Onboarding+Master",
                  add_completion=False)

def unsupported_option_cb(value:bool):
    if value:
        print("[bold red] WARNING: the selected option is not currently implemented. This is either because is is an planned or deprecated feature")
        raise typer.Exit()

class OpenOCDLocation(str, Enum):
    cube = "cube"
    sys = "sys"
    dl = "dl"

# ==============================================================================
# Build command
# ==============================================================================

@app.command(help="Build the project with GCC ARM Toolchain and Make/CMake")
def build(profile: str = typer.Option(None, "--profile", "-p", callback=unsupported_option_cb, help="(planned) Specify the build profile (e.g., debug, release)", show_default=True),
          clean: bool = typer.Option(False, "--clean", help="Clean the build directory before building", show_default=True)):
    is_cmake = os.path.exists("CMakeLists.txt")
    if is_cmake: # Repo uses CMake, so execute CMake commands.
        print("[#cccccc](ner build):[/#cccccc] [blue]CMake-based project detected.[/blue]")
        if clean:
            run_command_docker('cmake --build build --target clean ; find . -type d -name "build" -exec rm -rf {} +')
            print("[#cccccc](ner build):[/#cccccc] [green]Ran build-cleaning command.[/green]")
        else:
            run_command_docker("mkdir -p build && cd build && cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=cmake/gcc-arm-none-eabi.cmake .. && cmake --build .", stream_output=True)
            run_command_docker('chmod 777  -R ./build/*')
    else: # Repo uses Make, so execute Make commands.
        print("[#cccccc](ner build):[/#cccccc] [blue]Makefile-based project detected.[/blue]")
        if clean:
            run_command_docker("make clean", stream_output=True)
        else:
            run_command_docker(f"make -j{os.cpu_count()}", stream_output=True)

    if not clean and is_cmake:
        fix_compile_commands()

# ==============================================================================
# CGEN command
# ==============================================================================

@app.command(help="Generate CGEN code for your project")
def cgen(path: Path = typer.Option(Path("./Core"), "--output-path","-o", help="Output path for generated files (must contain Inc and Src subdirectories"),
         project: str | None = typer.Option(None, "--project", "-p", help="The project/node to generate for.  This will autodetect project if unset.")):
    def generate_project() -> str:
        curr_project = os.path.basename(os.getcwd())
        if curr_project == "TSECU-Shepherd":
            return "BMS"
        if curr_project == "Cerberus-2.0":
            return "VCU"
        if curr_project == "Lightning":
            return "Lightning"
        if curr_project == "MSB-FW-2":
            return "MSB"
        print("[bold red] Error: No valid project found")
        sys.exit(1)

    if project is None:
        project = generate_project()

    run_command(["python3", "./Drivers/Odyssey-Definitions/code-gen/codegen.py", project, path], stream_output=True)


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
          custom: bool = typer.Option(False, "--custom", help="Set this flag if your flash.cfg has all definitions (LAUNCHPAD)."),
          docker: bool = typer.Option(False, "--docker", callback=unsupported_option_cb, help="(deprecated) Use OpenOCD in the container instead of locally, requires linux"),
          openocd_loc: OpenOCDLocation = typer.Option(OpenOCDLocation.dl, "--openocd-loc", help="Use the OpenOCD binary from where"),
          ):

    command = fetch_openocd_command(openocd_loc)
    current_directory = os.getcwd()

    if not no_ftdi:
        ftdi_path = os.path.join(current_directory, "Drivers", "Embedded-Base", "ftdi_flash.cfg")
        command = command + ["-f", ftdi_path]
    elif not custom:
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
          custom: bool = typer.Option(False, "--custom", help="Set this flag if your flash.cfg has all definitions (LAUNCHPAD)."),
          docker: bool = typer.Option(False, "--docker", help="Use OpenOCD in the container instead of locally, requires linux"),
          openocd_loc: OpenOCDLocation = typer.Option(OpenOCDLocation.dl, "--openocd-loc", help="Use the OpenOCD binary from where"),
          ):

    command = []

    if docker and ftdi:
        print("[bold red] Cannot flash ftdi from docker")
        sys.exit(1)

    if docker:
        command = ["docker", "compose", "run", "--rm", "ner-gcc-arm"]
        command = command + ["openocd"]
    else:
        command = command + fetch_openocd_command(openocd_loc)

    if not no_ftdi:
        current_directory = os.getcwd()
        ftdi_path = os.path.join(current_directory, "Drivers", "Embedded-Base", "ftdi_flash.cfg")
        command = command + ["-f", ftdi_path]
    elif not custom:
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
# Serial2 command
# ==============================================================================

@app.command(help="Like 'ner serial', but with some extra custom features (message filtering, graphing, and monitoring).")
def serial2(
            ls: bool = typer.Option(False, "--list", help="List available serial devices and exit."),
            device: str = typer.Option("", "--device", "-d", help="Specify the board to connect to."),
            monitor: str = typer.Option(None, "--monitor", help="Opens a monitor window of the specified title. (Note: A monitor window can be created/configured using the serial_monitor() function from serial.c)"),
            graph: str = typer.Option(None, "--graph", help="Opens a live graph window of the specified title. (Note: A graph window can be created/configured using the serial_graph() function from serial.c)"),
            filter: str = typer.Option(None, "--filter", help="Only shows specific messages. Ex. 'ner serial2 --filter EXAMPLE' will only show printfs that contain the substring 'EXAMPLE'. ")):
    """Custom serial terminal."""

    serial2_start(ls=ls, device=device, monitor=monitor, graph=graph, filter=filter)

# ==============================================================================
# Test command
# ==============================================================================

@app.command(help="Run Unity Test source file")
def test(clean: bool = typer.Option(False, "--clean", help="Clean the build directory before building", show_default=True),
        tests: list[str] = typer.Argument(None, help="Specific test file to run (optional)")):

    if (clean):
        run_command_docker(f"rm -r Tests/Mocks/* Tests/build")
        return

    if tests == None:
        run_command_docker("python3 Drivers/Embedded-Base/ner_environment/build_system/test_runner.py", stream_output=True)
    else:
        run_command_docker(f"python3 Drivers/Embedded-Base/ner_environment/build_system/test_runner.py {' '.join(tests)}", stream_output=True)


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
# ---- End Launchpad Section
# ==============================================================================


# ==============================================================================
# Helper functions - not direct commands
# ==============================================================================

def run_command(command, stream_output=False, exit_on_fail=False):
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
            if result.stdout and result.stdout.strip():  # Only print if stdout is not empty or just whitespace
                print(result.stdout)
        except subprocess.CalledProcessError as e:
            print(f"Error occurred: {e}", file=sys.stderr)
            print(e.stderr, file=sys.stderr)
            if exit_on_fail:
                sys.exit(e.returncode)

def fix_compile_commands():
    '''
    Attempt to fix compile_commands.json so LSPs can read the project
    If this command fails it should be ignored as compile_commands.json is only for user experience
    '''
    # download the toolchain if needed, we always need the toolchain name
    os_type = platform.system()
    toolchain_name = ""
    if os_type == "Linux":
        toolchain_name = "arm-gnu-toolchain-14.3.rel1-x86_64-arm-none-eabi"
    else:
        toolchain_name = "arm-gnu-toolchain-14.3.rel1-darwin-arm64-arm-none-eabi"

    toolchain_name_ext = toolchain_name + ".tar.xz"

    if os.path.exists(f"{os.environ.get('VIRTUAL_ENV')}/{toolchain_name}"):
        pass
    else:
        # download the toolchain if it doesnt exist
        print('Downloading ', toolchain_name)
        subprocess.run([
            "wget",
            f"https://developer.arm.com/-/media/Files/downloads/gnu/14.3.rel1/binrel/{toolchain_name_ext}",
        ])
        subprocess.run(["tar", "-xvf", toolchain_name_ext, "-C", os.environ.get('VIRTUAL_ENV')])
        os.remove(toolchain_name_ext)

    jsf = None
    with open(f'{os.getcwd()}/build/compile_commands.json', "r") as f:
        jsf = json.load(f)


    if jsf is not None:
        docker_toolchain_name = re.search(r"arm-gnu-toolchain.*?\/", jsf[0]['command']).group(0)[:-1]
        print('Found docker toolchain ', docker_toolchain_name)
        for item in jsf:
            item['directory'] = item['directory'].replace('/home/app', os.getcwd())
            item['command'] = item['command'].replace('/home/app', os.getcwd())
            item['file'] = item['file'].replace('/home/app', os.getcwd())
            item['output'] = item['output'].replace('/home/app', os.getcwd())

            item['directory'] = item['directory'].replace(f"/home/dev/{docker_toolchain_name}", f"{os.environ.get('VIRTUAL_ENV')}/{toolchain_name}")
            item['command'] = item['command'].replace(f"/home/dev/{docker_toolchain_name}", f"{os.environ.get('VIRTUAL_ENV')}/{toolchain_name}")
            item['file'] = item['file'].replace(f"/home/dev/{docker_toolchain_name}", f"{os.environ.get('VIRTUAL_ENV')}/{toolchain_name}")
            item['output'] = item['output'].replace(f"/home/dev/{docker_toolchain_name}", f"{os.environ.get('VIRTUAL_ENV')}/{toolchain_name}")

        with open(f'{os.getcwd()}/build/compile_commands.json', "w") as f:
            json.dump(jsf, f)

        print('Successfully patched compile_commands.json')

def run_command_docker(command, stream_output=False):
    """Run a command in the Docker container."""
    docker_command = ["docker", "compose", "run", "--rm", "ner-gcc-arm", "sh", "-c", command]
    print(f"[bold blue](ner-gcc-arm): Running command '{command}' in Docker container.")
    run_command(docker_command, stream_output=stream_output)

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

def fetch_openocd_command(openocd_loc: OpenOCDLocation) -> list[str]:
    '''
    Creates an OpenOCD command
    '''
    command: list[str] = []
    os_type = platform.system()

    if openocd_loc.value == 'sys':
        command.append('openocd')
    elif openocd_loc.value == 'cube':
        if os_type == "Darwin":
            # for cubeIDE
            command.append(os.path.normpath(glob.glob("/Applications/STM32CubeIDE.app/Contents/Eclipse/plugins/com.st.stm32cube.ide.mcu.externaltools.openocd.**/tools/bin/openocd")[0]))
        elif os_type == "Linux":
            # for cubeIDE
            command.append(os.path.normpath(glob.glob(os.path.expanduser("~/st/stm32cubeide_1.19.0/plugins/com.st.stm32cube.ide.mcu.externaltools.openocd.**/tools/bin/openocd"))[0]))
    else:
        if os.path.exists(f"{os.environ.get('VIRTUAL_ENV')}/openocd_ner"):
            pass
        else:
            # download the binary if it doesnt exist
            # TODO make builds on ner repo, for now this is good enough
            r: requests.Response
            if os_type == "Linux":
                r = requests.get('https://github.com/bjackson312006/ner-openocd/releases/download/tools-v1.0.4/openocd-linux-x64', allow_redirects=True)
            else:
                r = requests.get('https://github.com/bjackson312006/ner-openocd/releases/download/tools-v1.0.4/openocd-macos-arm64', allow_redirects=True)
            print(r)
            open(f"{os.environ.get('VIRTUAL_ENV')}/openocd_ner", 'wb').write(r.content)
            os.chmod(f"{os.environ.get('VIRTUAL_ENV')}/openocd_ner", 0o777)

        command.append(f"{os.environ.get('VIRTUAL_ENV')}/openocd_ner")

    command.append("-s")
    command.append("./Drivers/Embedded-Base/dev/OpenOCD/tcl")

    return command

# ==============================================================================
# Entry
# ==============================================================================

if __name__ == "__main__":
    app(prog_name="ner")
