import subprocess
import sys
import os
import platform

def install_platformio(venv_path):
    """Install PlatformIO package and set up aliases."""
    try:
        # Install the platformio package
        subprocess.check_call([sys.executable, '-m', 'pip', 'install', 'platformio'])
        
        # Define aliases
        aliases = [
            'lpbuild="platformio run"',
            'lpflash="platformio run --target upload"',
            'lpinit="platformio project init"',

        ]

        os_type = platform.system()
        if os_type == 'Windows':
            activate_path = os.path.join(venv_path, 'Scripts', 'activate')  # Bash script for Git Bash on Windows
        else:
            activate_path = os.path.join(venv_path, 'bin', 'activate')  # bash script for Unix-like system
            

        with open(activate_path, 'a') as activate_file:
            activate_file.write('\n# Aliases\n')
            for alias in aliases:
                alias_name, alias_command = alias.strip().split('=', 1)
                alias_command = alias_command.strip('"')
                activate_file.write(f'alias {alias_name}="{alias_command}"\n')

        print("Aliases added to the activation script successfully.")

    except subprocess.CalledProcessError as e:
        print(f"Failed to install PlatformIO: {e}", file=sys.stderr)
        sys.exit(1)

def uninstall_platformio(venv_path):
    """Uninstall PlatformIO package and remove aliases."""
    try:
        # Uninstall the platformio package
        subprocess.check_call([sys.executable, '-m', 'pip', 'uninstall', '-y', 'platformio'])

        # Determine OS and adjust alias handling
        os_type = platform.system()
        if os_type == 'Windows':
            activate_path = os.path.join(venv_path, 'Scripts', 'activate')  # Bash script for Git Bash on Windows
        else:
            activate_path = os.path.join(venv_path, 'bin', 'activate')  # bash script for Unix-like system

        remove_aliases(activate_path)

        print("PlatformIO uninstalled and aliases removed. Please restart your terminal or source your profile script.")

    except subprocess.CalledProcessError as e:
        print(f"Failed to uninstall PlatformIO: {e}", file=sys.stderr)
        sys.exit(1)

def remove_aliases(activate_path):
    """Remove aliases from the virtual environment's activation script."""
    try:
        if os.path.exists(activate_path):
            with open(activate_path, 'r') as f:
                lines = f.readlines()
            with open(activate_path, 'w') as f:
                for line in lines:
                    if not line.startswith('alias '):
                        f.write(line)
            print("Aliases removed from the activation script successfully.")
        else:
            print("Activation script not found.", file=sys.stderr)
            sys.exit(1)
    except Exception as e:
        print(f"Failed to remove aliases: {e}", file=sys.stderr)
        sys.exit(1)

def main():
    if len(sys.argv) != 2:
        print("Usage: launchpad.py [install|uninstall]", file=sys.stderr)
        sys.exit(1)

    os_type = platform.system()
    current_directory = os.path.dirname(os.path.abspath(__file__))
    parent_directory = os.path.dirname(current_directory)
    venv_path = os.path.join(parent_directory, 'ner-venv') 

    action = sys.argv[1].lower()
    if action == 'install':
        install_platformio(venv_path)
    elif action == 'uninstall':
        uninstall_platformio(venv_path)
    else:
        print("Invalid action. Use 'install' or 'uninstall'.", file=sys.stderr)
        sys.exit(1)

if __name__ == '__main__':
    main()