import subprocess
import platform
import serial
import sys
from rich import print
import serial.tools.list_ports

def list_usb_devices():
    """List available USB serial devices based on the operating system."""
    os_name = platform.system()
    devices = []

    try:
        if os_name == 'Linux':
            result = subprocess.run(['ls /dev/tty*'], shell=True, capture_output=True, text=True)
            devices = [device for device in result.stdout.splitlines() if 'ttyUSB' in device or 'ttyACM' in device]
            devices.sort(key=lambda x: ('ttyACM' in x, x))  # Prioritize ttyUSB over ttyACM (Aka prioritize FTDI)
        else:
            print(f"[bold red]Unsupported operating system: {os_name}[/bold red]", file=sys.stderr)
            sys.exit(1)

        if not devices:
            print(f"[bold red]No USB devices found on {os_name}.[/bold red]", file=sys.stderr)
            sys.exit(1)

    except subprocess.CalledProcessError as e:
        print(f"[bold red]Failed to execute command to list USB devices: {e}[/bold red]", file=sys.stderr)
        sys.exit(1)
    except Exception as e:
        print(f"[bold red]An unexpected error occurred: {e}[/bold red]", file=sys.stderr)
        sys.exit(1)

    return devices

def main(ls=False, device="", filter=None):
    """Main function for serial communication."""
    if device == "":
        devices = list_usb_devices()
        if ls:
            print("[bold blue]Devices present:[/bold blue]")
            for dev in devices:
                print(f"[green]  - {dev}[/green]")
            print("[bold blue]The first device will be selected if --device is not specified.[/bold blue]")
            sys.exit(0)

        # Default to the first device
        selected_device = devices[0]
    else:
        selected_device = device

    print(f"[bold blue]Selected USB device:[/bold blue] [green]{selected_device}[/green]")

    try:
        with serial.Serial(selected_device, 115200, timeout=1) as ser:
            print(f"[bold blue]Connected to {selected_device} at 115200 baud.[/bold blue]")
            if filter:
                print(f"[bold blue]Filtering messages starting with:[/bold blue] [green]{filter}[/green]")
            else:
                print("[bold blue]Displaying all messages.[/bold blue]")
            while True:
                line = ser.readline().decode('utf-8', errors='ignore').strip()
                if not filter or line.startswith(filter):
                    print(line)
    except serial.SerialException as e:
        print(f"[bold red]Failed to open serial port {selected_device}: {e}[/bold red]", file=sys.stderr)
        sys.exit(1)
    except KeyboardInterrupt:
        print("\n[bold blue]Exiting...[/bold blue]")