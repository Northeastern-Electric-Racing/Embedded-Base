import subprocess
import os

def run_cmake_in_docker():
    # Ensure the Docker container is running
    subprocess.run("docker-compose up -d", shell=True, check=True)

    # Run CMake inside the Docker container
    try:
        subprocess.run(
            'docker-compose exec -T build_service bash -c "cmake -S . -B build && cmake --build build"',
            shell=True,
            check=True
        )
        print("Build completed successfully.")
    except subprocess.CalledProcessError as e:
        print(f"Error during build process: {e}")

    # Bring down the Docker container
    subprocess.run("docker-compose down", shell=True, check=True)

def main():
    run_cmake_in_docker()

if __name__ == "__main__":
    main()
