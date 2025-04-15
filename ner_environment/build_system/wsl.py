import os
import sys
from rich import print
import subprocess

# Asks the user a yes/no question. If the answer is 'y', it continues. Otherwise, it exits the script.
def yes_no(prompt, ideal_response="y", exit_msg="Exiting."):
    print(prompt)
    response = input().strip().lower()
    if response != ideal_response:
        print(exit_msg)
        sys.exit(1)

# Executes a PowerShell command from. If admin is True, it runs the command as an administrator.
# Returns the output of the command (If admin is False). Unfortunately, it can't do this if admin is True.
def powershell_command(command, admin=False):
    if admin:
        # Mention that Windows is about to ask for admin permissions
        yes_no("[bold blue]Windows needs admin permissions for this; please say yes. Continue? (y/n):[/bold blue]")
        print("[bold blue]Running command as admin...[/bold blue]")
        try:
            # Format the command
            command = f"Start-Process powershell -ArgumentList '-NoProfile -Command \"{command}\"' -Verb RunAs -WindowStyle Hidden"
            subprocess.run(['powershell.exe', '-Command', command], text=True, check=True)
            print("\n")
            return "Successfully executed command as admin."
        except subprocess.CalledProcessError as e:
            return f"Command failed with error:\n{e.stderr}"
    else:
        try:
            result = subprocess.run(['powershell.exe', command], capture_output=True, text=True, check=True)
            return result.stdout
        except subprocess.CalledProcessError as e:
            return f"Command failed with error:\n{e.stderr}"


# Called when 'ner wsl --attach' is run. Prompts the user to choose a device, and attaches it.
def wsl_attach():
    # Check if usbipd is installed
    output = powershell_command("usbipd")
    if "CommandNotFoundException" in output:
        yes_no("[bold blue]usbipd doesn't seem to be installed. Would you like to install it? It may take a second. (y/n):[/bold blue]", "Exiting. (You need usbipd to attach devices.)")
        powershell_command("winget install usbipd", admin=True)
        print("[bold blue]usbipd should be installing on Windows (a pop up should appear). Please wait for it to download.[/bold blue]")
        yes_no("[bold blue]Once it's downloaded, please type 'continue':[/bold blue]", ideal_response="continue", exit_msg="Exiting. If it didn't work, try running 'winget install usbipd' in a Windows terminal, and then running this command again.")

        # Check if it installed correctly
        output = powershell_command("usbipd")
        if "CommandNotFoundException" in output:
            print("[bold red]usbipd didn't install correctly. Not good! You may have to install it manually, and then try running this script again.[/bold red]")
            sys.exit(1)
        else:
            print("[bold green]usbipd installed successfully. Continuing...[/bold green]")
            return
        
    # List the devices
    print(powershell_command("usbipd list"))
    print("[bold green]Please select a BUSID (ex. 1-1, 1-3, etc.) to attach.[/bold green]")
    busid = input("BUSID: ")

    # Attempt to attach the device
    print(f"[bold blue]Attempting to attach BUSID {busid}...[/bold blue]")
    output = powershell_command(f"usbipd attach --wsl=ubuntu --busid {busid}")
    if "Device is not shared" in output:
        print("[bold red]Failed to attach device. If this is your first time trying to attach the device, you may need to bind it.[/bold red]")
        yes_no("[bold blue]Would you like to bind the device? (y/n):[/bold blue]", ideal_response="y", exit_msg="Exiting. (You need to bind the device to attach it.)")
        powershell_command(f"usbipd bind --busid={busid}", admin=True)
        output = powershell_command(f"usbipd attach --wsl=ubuntu --busid {busid}")
        if "Device is not shared" in output:
            # Error: Device needs to be attached
            print("[bold red]Failed to bind device. You may need to try doing this manually.[/bold red]")
            print("[bold green](The command is: usbipd bind --busid=<busid>. You would need to run it in a Windows terminal as an administrator.)[/bold green]")
            print("Exiting...")
            sys.exit(1)
        else:
            print("[bold green]Device bound and attached successfully![/bold green]")
            print("[bold green]You can access the device in WSL now.[/bold green]")
    elif "Device with busid" in output:
        # Error: Device is already attached
        print("[bold red]That device is already attached. Please try again with a different device.[/bold red]")
        sys.exit(1)
    elif "the device will be available" in output:
        # Success
        print("[bold green]Device attached successfully![/bold green]")
        print("[bold green]You should be able to access the device in WSL now.[/bold green]")
    else:
        # Unknown error, so print the output message for debugging
        print("[bold red]An unknown error occurred while trying to attach the device.[/bold red]")
        print("(This probably means the usbipd output messages changed.)")
        print("[bold green]Please try running the command again, or check the output for more information:[/bold green]")
        print(output)
        sys.exit(1)

    return

def main(attach):
    # If the attach flag is set, run the wsl_attach command
    if attach:
        wsl_attach()

if __name__ == '__main__':
    main()