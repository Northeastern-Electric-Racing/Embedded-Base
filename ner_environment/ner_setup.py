import os
import platform
import subprocess
import sys
import venv

try:
    import distro  # for Linux distribution detection
except ImportError:
    distro = None

def run_command(command, check=True, shell=False):
    try:
        result = subprocess.run(command, check=check, shell=shell, text=True, capture_output=True)
        print(result.stdout)
        if result.stderr:
            print(result.stderr, file=sys.stderr)
    except subprocess.CalledProcessError as e:
        print(f"Command failed: {e.cmd}\nReturn code: {e.returncode}\nOutput: {e.output}\nError: {e.stderr}", file=sys.stderr)
        sys.exit(1)

def check_docker_and_rust():
    print("This script requires Docker and Rust to be installed.")
    answer = input("Do you have Docker and Rust installed? (yes/no): ").strip().lower()
    if 'y' not in answer:
        sys.exit(1)

    os_type = platform.system()
    if os_type == 'Windows':
        print("Windows OS detected. If on windows, you must be using bash (included with git), instead of cmd or powershell") 
        answer = input("Are you using bash? (yes/no): ").strip().lower()
        if 'y' not in answer:
	        sys.exit(1)  

def docker_pull(image_url):
    os_type = platform.system()
    if os_type=='Windows' or os_type == 'Darwin':
        print("Please open the Docker Desktop application before proceeding")
        answer =input("Is the Docker Desktop app running? (yes/no)")
    print("Pulling Docker image...")
    run_command(["docker", "pull", image_url])
    print("Docker image pulled successfully.")

def create_venv(venv_path):
    try:
        venv.EnvBuilder(with_pip=True).create(venv_path)
        print(f"Virtual environment created at {venv_path}")
    except Exception as e:
        print(f"ERROR: Failed to create virtual environment: {e}", file=sys.stderr)
        sys.exit(1)

def run_setup(venv_python):
    print("Running setup.py...")
    try:
        # Run the pip install -e . command
        setup_dir = os.path.join(os.getcwd(), "ner_environment")
        run_command([venv_python, '-m', 'pip', 'install', '-e', setup_dir])
    except subprocess.CalledProcessError as e:
        print(f"Failed to run pip install -e .: {e}", file=sys.stderr)
        sys.exit(1)


def install_precommit(venv_python):
    print("Installing pre-commit hook...")
    try:
        run_command([venv_python, '-m', 'pre_commit', 'install'])
    except Exception as e:
        print(f"Failed to install pre-commit: {e}", file=sys.stderr)
        sys.exit(1)

def install_probe_rs():
    os_type = platform.system()
    print("Installing probe-rs...")
    try:
        if os_type == "Windows":
        # Using bash to run PowerShell for downloading and executing the script
            command = [
                      "powershell", "-Command", "Invoke-RestMethod https://github.com/probe-rs/probe-rs/releases/latest/download/probe-rs-tools-installer.ps1 | Invoke-Expression"]
            run_command(command, shell=False)
        else:
            # For Unix-like systems (Linux, macOS)
            command = ["bash", "-c", "curl --proto '=https' --tlsv1.2 -LsSf https://github.com/probe-rs/probe-rs/releases/latest/download/probe-rs-tools-installer.sh | sh"]
            run_command(command, shell=False)
    except Exception as e:
        print(f"Failed to install probe-rs: {e}", file=sys.stderr)
        sys.exit(1)

def install_usbip():
    if distro:
        distro_name = distro.id()
        if distro_name in ["ubuntu", "debian"]:
            run_command(["sudo", "apt-get", "install", "-y", "linux-tools-generic"])
        elif distro_name == "fedora":
            run_command(["sudo", "dnf", "install", "-y", "linux-tools-generic"])
        elif distro_name == "arch":
            run_command(["sudo", "pacman", "-S", "--noconfirm", "linux-tools-generic"])
        else:
            print("We haven't added USBIP install support for your distro, but if you're actually on linux, you can install it manually!")
    else:
        print("You should only see this if im stupid! Let someone know if you see this message -> ID=STUPID1")

def main():
    print("Welcome to NER Embedded Software! If this script shits the bed, let your system head or lead know!")
    print("-----------------------------------------------------------------------------------------------")
    print("Every step below will have an option to skip. While most users will *not* want to skip anything")
    print("Everyone's system is different, and if you already have something installed, or know exactly")
    print("what you are doing, feel free to manually go about this as needed.")

    # Step 0: Check for Docker and Rust
    check_docker_and_rust()
	
    # Step 1: pull image
    answer = input("Would you like to pull the docker image? (yes/no)")
    if 'y' in answer:    
        image_url = "ghcr.io/northeastern-electric-racing/embedded-base:main"
        docker_pull(image_url)

    os_type = platform.system()
    current_directory = os.path.dirname(os.path.abspath(__file__)) # ner_environment
    parent_directory = os.path.dirname(current_directory) # embedded-base
    venv_path = os.path.dirname(parent_directory) # NER
    venv_path = os.path.join(venv_path, 'ner-venv') # NER/ner-venv

    
    # Step 2: Create the Python virtual environment
    answer = input("Would you like to setup the virtual environment (yes/no)")
    if 'y' in answer:
        create_venv(venv_path)

        answer = input("Would you like to install all python packages in the venv? (yes/no)")
        if 'y' in answer:
        # Use the venv's Python
            venv_python = os.path.join(venv_path, 'Scripts', 'python') if os_type == "Windows" else os.path.join(venv_path, 'bin', 'python')

            # Step 4: run setup.py (installs requirements and entry points)
            run_setup(venv_python)

            # Step 5: Run pre-commit install
            install_precommit(venv_python)

    answer = input("Would you like to install probe-rs? (do this manually if on a weird linux distro!) (yes/no)")
    if 'y' in answer:
        # Step 5: Install probe-rs
        install_probe_rs()

    # Step 6: Install usbip if on Linux
    if os_type == "Linux":
        answer = input("Would you like to install usbip? (yes/no)")
        if 'y' in answer:
            install_usbip()

    print("Setup completed successfully!")

if __name__ == "__main__":
    main()
