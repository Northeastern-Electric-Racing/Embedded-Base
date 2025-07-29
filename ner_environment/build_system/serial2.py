import subprocess
import platform
import serial
import sys
from rich import print
from rich.panel import Panel
import serial.tools.list_ports
from collections import defaultdict

# Returns true if the script is running in WSL. Returns false otherwise.
def is_wsl():
    with open('/proc/version', 'r') as f:
        if 'microsoft' in f.read().lower():
            return True
    return False

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
            if is_wsl():
                print("\n[blue]If you're using WSL, you may need to attach attach the USB device in a Windows terminal.")
                print("1. Open a Windows terminal [bold blue]with admin privalleges[/bold blue] and enter 'usbipd list'.")
                print("2. Find the device you want to attach (probably named something like 'CMSIS-DAP v2 Interface' or 'USB Serial Device')")
                print("3. Note the device's BUSID and run [bold green]'usbipd bind --busid=<BUSID>'[/bold green]. (If you've mounted the device before, skip this step.)")
                print("4. Then, run [bold green]'usbipd attach --wsl=ubuntu --busid <BUSID>'.")
                print("5. It should work now, so try running the command again.\n")
            sys.exit(1)

    except subprocess.CalledProcessError as e:
        print(f"[bold red]Failed to execute command to list USB devices: {e}[/bold red]", file=sys.stderr)
        sys.exit(1)
    except Exception as e:
        print(f"[bold red]An unexpected error occurred: {e}[/bold red]", file=sys.stderr)
        sys.exit(1)

    return devices

# MONITOR STUFF
ui_monitor = None
ui_monitor_labels = {}
def update_monitor(line):
    global ui_monitor, ui_monitor_labels
    """Update monitor panel with new data."""

    import tkinter as tk # Only import tkinter if monitor stuff is being used (dunno if this is python bad practice)

    parts = line.strip().split("/")
    if len(parts) != 4: return # If there aren't exactly 4 parts, the line is malformed. So, skip it.
    title = parts[1]
    data_label = parts[2]
    value = parts[3]

    if ui_monitor is None: # If the monitor window doesn't exist yet, create it
        ui_monitor = tk.Tk()
        ui_monitor.title(title)

    if data_label not in ui_monitor_labels: # If this data label hasn't been seen yet, create it
        ui_monitor_labels[data_label] = tk.Label(ui_monitor, text=f"{data_label}: {value}", font=("Helvetica", 12))
        ui_monitor_labels[data_label].pack()
    else: # (Otherwise, update the existing label)
        ui_monitor_labels[data_label].config(text=f"{data_label}: {value}")

    try:
        if ui_monitor.winfo_exists(): # Check if the monitor window still exists.
            ui_monitor.update() # If it does, update the monitor.
    except tk.TclError:
        ui_monitor = None # If it doesn't, someone is probably trying to exit the serial program. So, set the monitor to None.

# GRAPH STUFF
ui_graph = None
ui_graph_axes = None
ui_graph_lines = {}
ui_graph_series = defaultdict(lambda: ([], []))  # {DataLabel: ([ticks], [values])}

def update_graph(line, plt):
    global ui_graph, ui_graph_axes, ui_graph_lines, ui_graph_series

    # Parse the line
    parts = line.strip().split("/")

    # Check if the line is valid
    if len(parts) != 5: return # If there aren't exactly 5 parts, the line is malformed. So, skip it.
    try:
        tick = float(parts[3])
        value = float(parts[4])
    except ValueError: return # If the conversion to float fails, the line is malformed. So, skip it.

    title = parts[1]
    data_label = parts[2]
    tick = float(parts[3])  # Convert to float or int depending on your data
    value = float(parts[4])

    # Update series
    ui_graph_series[data_label][0].append(tick)
    ui_graph_series[data_label][1].append(value)

    # Limit to 1000 points
    if len(ui_graph_series[data_label][0]) > 1000:
        ui_graph_series[data_label][0].pop(0)
        ui_graph_series[data_label][1].pop(0)

    # Create graph if it doesn't exist
    if ui_graph is None:
        ui_graph, ui_graph_axes = plt.subplots()
        ui_graph_axes.set_title(title)
        ui_graph_axes.set_xlabel("Tick")
        ui_graph_axes.set_ylabel("Value")
        for label, (x, y) in ui_graph_series.items():
            ui_graph_lines[label], = ui_graph_axes.plot(x, y, label=label)
        ui_graph_axes.legend()
        plt.ion()  # Interactive mode on
        plt.show()
    else:
        # Update existing graph
        for label, (x, y) in ui_graph_series.items():
            if label not in ui_graph_lines:
                ui_graph_lines[label], = ui_graph_axes.plot(x, y, label=f"{label}: {y[-1]:.2f}")
                ui_graph_axes.legend()
            else:
                ui_graph_lines[label].set_data(x, y) # Update data
                ui_graph_lines[label].set_label(f"{label}: {y[-1]:.2f}") # Update label with latest value
        # Rescale axes if necessary
        ui_graph_axes.relim()
        ui_graph_axes.autoscale_view()
        ui_graph_axes.legend()
        ui_graph.canvas.draw()
        ui_graph.canvas.flush_events()
        


def main(ls=False, device="", monitor=None, graph=None, filter=None, showall=False):
    global ui_monitor, ui_graph
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

    if(graph): # Import matplotlib if graphing is used. If matplotlib isn't installed, tell the user to run "ner update".
        try:
            import matplotlib.pyplot as plt
        except ImportError:
            print("Error: You don't have matplotlib installed. Please run 'ner update' to install it.")
            sys.exit(1)

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
                if line.startswith("mtr/") or line.startswith("gph/"): # Message has a special tag, so handle accordingly.
                    if monitor and line.startswith(f"mtr/{monitor}/"): # Message contains monitor data, so update the monitor.
                        update_monitor(line)
                    elif graph and line.startswith(f"gph/{graph}/"):
                        update_graph(line, plt)
                elif not filter or (filter in line): # If message isn't being filtered out, print it.
                    print(line)
    except serial.SerialException as e:
        print(f"[bold red]Failed to open serial port {selected_device}: {e}[/bold red]", file=sys.stderr)
        sys.exit(1)
    except KeyboardInterrupt:
        if ui_monitor is not None:
            ui_monitor.destroy()
        if ui_graph is not None:
            plt.close(ui_graph)
        print("\n[bold blue]Exiting...[/bold blue]")

if __name__ == '__main__':
    main()