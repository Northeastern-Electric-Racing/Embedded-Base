#! /usr/bin/env python3

import tomllib
import os
import subprocess

PROJECT_DIR_PATH = "/home/app/"
EMBEDDED_BASE_PATH = PROJECT_DIR_PATH + "Drivers/Embedded-Base/"
EMBEDDED_BASE_TESTING_DIR_PATH = EMBEDDED_BASE_PATH + "Testing/"
BUILD_DIR = EMBEDDED_BASE_TESTING_DIR_PATH + "build"
CMAKE_TOOLCHAIN_FILEPATH = EMBEDDED_BASE_TESTING_DIR_PATH + "test-toolchain.cmake"
PROJECT_TEST_DIR_PATH = PROJECT_DIR_PATH + "Tests/"
TEST_MOCKS_DIR_PATH = PROJECT_TEST_DIR_PATH + "Mocks/"
TEST_CONF_PATH = PROJECT_TEST_DIR_PATH + "ner_test.conf"
CMOCK_RUBY_SCRIPT_PATH = "/cmock_portable/lib/cmock.rsb"
CMOCK_CONFIG = EMBEDDED_BASE_TESTING_DIR_PATH + "cmock-config.yml"

with open("Tests/ner_test.conf", "rb") as f:
        data = tomllib.load(f)
    
test_packages = data.get("test-packages", {})
tests = data.get("test", {})
mocked_files = data.get("mocked-files", [])
additional_sources = data.get("additional-sources", [])
include_dirs = data.get("include-dirs", [])

def _get_dir_sources(test_package_name, dir_name):
    excluded_substrings = ["tx", "stm", "sys", "trace", "main"]

    tp_files = test_packages[test_package_name]["mocked-files"]
    tp_files += mocked_files
    tp_files_no_ext = [os.path.splitext(os.path.basename(f))[0] for f in tp_files]

    files = []
    for root, _, filenames in os.walk(dir_name):
        for fname in filenames:
            
            if any(sub in fname for sub in excluded_substrings):
                continue

            file_no_ext = fname.split(".")[0]
            if file_no_ext not in tp_files_no_ext:
                files.append(os.path.join(root, fname))
    
    return files

def get_project_sources(test_package_name):

    project_sources = os.path.join(PROJECT_DIR_PATH, "Core", "Src")
    # project_sources = "Core/Src"

    source_files  = _get_dir_sources(test_package_name, project_sources)


    mocked_source_dir = os.path.join(TEST_MOCKS_DIR_PATH, test_package_name) 
    # mocked_source_dir = "Tests/Mocks"

    for root, _, filenames in os.walk(mocked_source_dir):
        for fname in filenames:
            ext = fname.split(".")[1]
            if ext != "c":
                continue
            source_files.append(os.path.join(root, fname))

    formatted_additional_sources = [os.path.join(PROJECT_DIR_PATH, s) for s in additional_sources]

    source_files += formatted_additional_sources
    return source_files

def create_mocks():

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
    
    joined_sources = "\"" + " ".join(source_files) + "\""
    formatted_include_dirs = [os.path.join(PROJECT_DIR_PATH, d) for d in include_dirs]
    joined_include_dirs = "\"" + " ".join(formatted_include_dirs) + "\""
    
    make_command = f"""
    mkdir -p {BUILD_DIR} && cd {BUILD_DIR} && \
    make -f {EMBEDDED_BASE_TESTING_DIR_PATH}/Makefile \
    TEST_NAME={test_name} \
    TEST_FILE={test_file} \
    TEST_PACKAGE={test_package} \
    C_SOURCES="{joined_sources}" \
    INCLUDE_DIRS="{joined_include_dirs}"
    """

    command = ["sh", "-c", make_command]

    print("RUNNING COMMAND: " + make_command)

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

if __name__ == "__main__":
    main()
    

    



    