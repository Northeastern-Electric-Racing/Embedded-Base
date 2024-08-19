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
        print("ERROR: Please install Docker and Rust before running this script.")
        sys.exit(1)

def create_venv(venv_path):
    try:
        venv.EnvBuilder(with_pip=True).create(venv_path)
        print(f"Virtual environment created at {venv_path}")
    except Exception as e:
        print(f"ERROR: Failed to create virtual environment: {e}", file=sys.stderr)
        sys.exit(1)

def modify_activation_scripts(venv_path):
    os_type = platform.system()
    activate_path = os.path.join(venv_path, 'Scripts', 'activate') if os_type == 'Windows' else os.path.join(venv_path, 'bin', 'activate')
    deactivate_path = os.path.join(venv_path, 'Scripts', 'deactivate') if os_type == 'Windows' else os.path.join(venv_path, 'bin', 'deactivate')

    # Modify the activate script to start Docker container
    try:
        with open(activate_path, 'a') as activate_file:

            # aliases
          
         print("Activation script modified to add aliases")
    except Exception as e:
        print(f"Failed to modify activation script: {e}", file=sys.stderr)
        sys.exit(1)

    # Modify the deactivate script to stop Docker container
    try:
        with open(deactivate_path, 'a') as deactivate_file:
            deactivate_file.write("\n# Stop Docker container when venv is deactivated\n")
            deactivate_file.write("docker-compose down ner-gcc-arm\n")
        print("Deactivation script modified to stop Docker container.")
    except Exception as e:
        print(f"Failed to modify deactivation script: {e}", file=sys.stderr)
        sys.exit(1)

def install_requirements(venv_python):
    print("Installing requirements.txt...")
    try:
        run_command([venv_python, '-m', 'pip', 'install', '-r', 'requirements.txt'])
    except Exception as e:
        print(f"Failed to install requirements: {e}", file=sys.stderr)
        sys.exit(1)

def install_pyyaml_no_build_isolation(venv_python):
    print("Installing PyYAML without build isolation...")
    try:
        run_command([venv_python, '-m', 'pip', 'install', 'pyyaml==5.4.1', '--no-build-isolation'])
    except Exception as e:
        print(f"Failed to install PyYAML: {e}", file=sys.stderr)
        sys.exit(1)

def install_cython_and_wheel(venv_python):
    print("Installing Cython and Wheel...")
    try:
        run_command([venv_python, '-m', 'pip', 'install', 'cython<3.0.0', 'wheel'])
    except Exception as e:
        print(f"Failed to install Cython and Wheel: {e}", file=sys.stderr)
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
            # For Windows, using PowerShell to execute the script
            command = ["powershell", "-Command", "irm https://github.com/probe-rs/probe-rs/releases/latest/download/probe-rs-tools-installer.ps1 | iex"]
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
        print("You should only see this if im stupid! Let someone know if you see this message")

def main():
    print("Welcome to NER Embedded Software! If this script shits the bed, let a system head or lead know!")
    print("-----------------------------------------------------------------------------------------------")

    # Step 0: Check for Docker and Rust
    check_docker_and_rust()

    os_type = platform.system()
    current_directory = os.path.dirname(os.path.abspath(__file__))
    parent_directory = os.path.dirname(current_directory)
    venv_path = os.path.join(parent_directory, 'ner-venv')
    # Step 1: Create the Python virtual environment
    create_venv(venv_path)

    # Step 2: Modify activation and deactivation scripts
    modify_activation_scripts(venv_path)

    # Use the venv's Python
    venv_python = os.path.join(venv_path, 'Scripts', 'python') if os_type == "Windows" else os.path.join(venv_path, 'bin', 'python')

    # Step 3: Install all Python packages from requirements.txt

    install_cython_and_wheel(venv_python)
    install_pyyaml_no_build_isolation(venv_python)
    install_requirements(venv_python)

    # Step 4: Run pre-commit install
    install_precommit(venv_python)

    # Step 5: Install probe-rs
    install_probe_rs()

    # Step 6: Install usbip if on Linux
    if os_type == "Linux":
        install_usbip()

    print("Setup completed successfully!")

if __name__ == "__main__":
    main()
