#! /usr/bin/env python3

import tomllib
import os
import subprocess

PROJECT_DIR_PATH = "/home/app/"
EMBEDDED_BASE_PATH = PROJECT_DIR_PATH + "Drivers/Embedded-Base/"
EMBEDDED_BASE_TESTING_DIR_PATH = EMBEDDED_BASE_PATH + "Testing/"
MOCK_STUBS_FILEPATH = EMBEDDED_BASE_TESTING_DIR_PATH + "manual_mocks/mock_stubs.h" 
BUILD_DIR = EMBEDDED_BASE_TESTING_DIR_PATH + "build"
CMAKE_TOOLCHAIN_FILEPATH = EMBEDDED_BASE_TESTING_DIR_PATH + "test-toolchain.cmake"
PROJECT_TEST_DIR_PATH = PROJECT_DIR_PATH + "Tests/"
TEST_MOCKS_DIR_PATH = PROJECT_TEST_DIR_PATH + "Mocks/"
TEST_CONF_PATH = PROJECT_TEST_DIR_PATH + "ner_test.conf"
CMOCK_RUBY_SCRIPT_PATH = "/cmock_portable/lib/cmock.rb"
CMOCK_CONFIG = EMBEDDED_BASE_TESTING_DIR_PATH + "cmock-config.yml"

with open("Tests/ner_test.conf", "rb") as f:
        data = tomllib.load(f)
    
test_packages = data.get("test-packages", {})
tests = data.get("test", {})
mocked_files = data.get("mocked-files", [])
core_sources = data.get("sources", [])
include_dirs = data.get("include-dirs", [])

def get_project_sources(test_package_name):

    source_files = core_sources
    mocked_source_dir = os.path.join(TEST_MOCKS_DIR_PATH, test_package_name) 
    # mocked_source_dir = "Tests/Mocks"

    for _, _, filenames in os.walk(mocked_source_dir):
        for fname in filenames:
            ext = fname.split(".")[1]
            if ext != "c":
                continue
            source_files.append(os.path.join(f"Tests/Mocks/{test_package_name}", fname))

    return source_files

def create_mocks():

    print("Creating Mocks...")

    processes = []
    for tp_name, tp_data in test_packages.items():
        dir_path = os.path.join(TEST_MOCKS_DIR_PATH, tp_name)
        os.makedirs(dir_path, exist_ok=True)
        print(f"Created directory for test package: {tp_name}")

        files = tp_data.get("mocked-files", [])
        files += mocked_files

        for file_path in files:
            command = ["ruby", CMOCK_RUBY_SCRIPT_PATH, f"-o{CMOCK_CONFIG}", f"--mock_path={TEST_MOCKS_DIR_PATH + tp_name}", file_path]
            processes.append(subprocess.Popen(command, text=True))

    # wait for all mocks to be created
    for p in processes:
        p.wait()

def build_test(test_name, test_file, test_package, source_files):

    print("BUILDING TEST: " + test_name)
    
    joined_sources =  " ".join(source_files) 
    joined_include_dirs = " ".join(include_dirs) 
    
    make_command = f"""
    make -f {os.path.join(EMBEDDED_BASE_TESTING_DIR_PATH, "Makefile")} \
    TEST_NAME={test_name} \
    TEST_FILE={test_file} \
    TEST_PACKAGE={test_package} \
    C_SOURCES="{joined_sources}" \
    INCLUDE_DIRS="{joined_include_dirs}"
    """

    command = ["sh", "-c", make_command]

    # Run Build
    return subprocess.Popen(command, text=True)
  

def main():
    create_mocks()

    processes = []
    for t_name, t_data in tests.items():

        test_package = t_data["test-package"]
        test_file = t_data["test-file"]
        sources = get_project_sources(test_package)

        processes.append(build_test(t_name, test_file, test_package, sources))

    for p in processes:
        p.wait()
    
    processes = []
    for t_name in tests.keys():
        processes.append(subprocess.Popen(f"Tests/build/{t_name}", shell=True, text=True))
    
    for p in processes:
        p.wait()
    

if __name__ == "__main__":
    main()
    

    



    