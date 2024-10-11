import subprocess
import platform
import sys

def list_usb_devices():
    """List available USB serial devices based on the operating system."""
    os_name = platform.system()

    if os_name == 'Windows':
        # List COM ports on Windows
        try:
            result = subprocess.run(['wmic', 'path', 'Win32_SerialPort'], capture_output=True, text=True)
            devices = []
            for line in result.stdout.splitlines():
                if 'DeviceID' in line:
                    devices.append(line.split()[-1])
            return devices
        except Exception as e:
            print(f"Failed to list USB devices on Windows: {e}", file=sys.stderr)
            sys.exit(1)

    elif os_name == 'Linux' or os_name == 'Darwin':  # Darwin is macOS
        # List USB devices on Unix-like systems
        try:
            result = subprocess.run(['ls /dev/tty*'], shell=True, capture_output=True, text=True)

            devices = [device for device in result.stdout.splitlines() if 'ttyUSB' in device or 'ttyACM' in device]
            return devices
        except Exception as e:
            print(f"Failed to list USB devices on {os_name}: {e}", file=sys.stderr)
            sys.exit(1)

    else:
        print(f"Unsupported operating system: {os_name}", file=sys.stderr)
        sys.exit(1)

def run_miniterm(device, baudrate=115200):
    """Run pyserial-miniterm with the specified device and baudrate."""
    try:
        subprocess.run(['pyserial-miniterm', device, str(baudrate)], check=True)
    except subprocess.CalledProcessError as e:
        print(f"Failed to run pyserial-miniterm: {e}", file=sys.stderr)
        sys.exit(1)

def main(args):
    # Detect the operating system and find USB devices
    if not args.device:
        # reverse so FTDI is the default if present, can always be overridden
        devices = list(reversed(list_usb_devices()))
        
        if not devices:
            print("No USB devices found.", file=sys.stderr)
            sys.exit(1)

        if args.list:
            print("Devices present:")
            for i, device in enumerate(devices):
                print(device)
            
            print("The first device will be selected if --device not specified.")
            sys.exit(0)
        
        # Default to the first device if available
        selected_device = devices[0]
    else:
        selected_device = args.device
    print(f"Selected USB device: {selected_device}")
    
    # Run pyserial-miniterm with the selected device
    run_miniterm(selected_device)

if __name__ == '__main__':
    main()