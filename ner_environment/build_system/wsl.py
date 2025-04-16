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

    # Attach the device
    print(f"[bold blue]Running Windows command: usbipd attach --wsl=ubuntu --busid {busid}[/bold blue]")
    output = powershell_command(f"usbipd attach --wsl=ubuntu --busid {busid}")
    print(output)

    if "usbipd bind" in output:
        yes_no(f"[bold blue]Do you want to run usbipd bind --busid {busid}? (y/n)[/bold blue]", exit_msg="Exiting. (You need to bind for this command to work.)")
        powershell_command(f"usbipd bind --busid {busid}", admin=True)
        print("[bold blue]Bounded device. Please try running 'ner wsl --attach' (or 'ner wsl -a') again to attach it.[/bold blue]")

    if "error" not in output:
        print("[bold green]Device attached successfully![/bold green]")

    return

def main(attach):
    # If the attach flag is set, run the wsl_attach command
    if attach:
        wsl_attach()

if __name__ == '__main__':
    main()