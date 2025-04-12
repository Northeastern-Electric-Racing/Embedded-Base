import subprocess
import platform
import serial
import sys
from rich import print
from rich.panel import Panel
import serial.tools.list_ports
import tkinter as tk

# USB DEVICE STUFF
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

# MONITOR STUFF
monitor = None
monitor_labels = {}
def update_monitor(line):
    global monitor, monitor_labels
    """Update monitor panel with new data."""
    parts = line.split("/")
    title = parts[1]
    data_label = parts[2]
    value = parts[3]

    if monitor is None: # If the monitor window doesn't exist yet, create it
        monitor = tk.Tk()
        monitor.title(title)

    if data_label not in monitor_labels: # If this data label hasn't been seen yet, create it
        monitor_labels[data_label] = tk.Label(monitor, text=f"{data_label}: {value}", font=("Helvetica", 12))
        monitor_labels[data_label].pack()
    else: # (Otherwise, update the existing label)
        monitor_labels[data_label].config(text=f"{data_label}: {value}")

    monitor.update()


def main(ls=False, device="", monitor=None, filter=None, showall=False):
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
            print(f"[bold blue]Displaying messages. Use CTRL+C to exit.[/bold blue]")
            if filter:
                print(f"[bold blue]Filtering messages containing:[/bold blue] [green]{filter}[/green]")
            while True:
                line = ser.readline().decode('utf-8', errors='ignore').strip()
                if not line: # Skip empty lines
                    continue
                if line.startswith("m/"): # Message has a special tag, so handle accordingly.
                    if monitor and line.startswith(f"m/{monitor}/"): # Message contains monitor data, so update the monitor.
                        update_monitor(line)
                elif not filter or (filter in line): # If message isn't being filtered out, print it.
                    print(line)
    except serial.SerialException as e:
        print(f"[bold red]Failed to open serial port {selected_device}: {e}[/bold red]", file=sys.stderr)
        sys.exit(1)
    except KeyboardInterrupt:
        print("\n[bold blue]Exiting...[/bold blue]")

if __name__ == '__main__':
    main()