#! /usr/bin/env python3

import tomllib
import os
import subprocess
import sys

EMBEDDED_BASE_PATH = "Drivers/Embedded-Base/"
EMBEDDED_BASE_TESTING_DIR_PATH = EMBEDDED_BASE_PATH + "Testing/"
BUILD_DIR = EMBEDDED_BASE_TESTING_DIR_PATH + "build"
CMAKE_TOOLCHAIN_FILEPATH = EMBEDDED_BASE_TESTING_DIR_PATH + "test-toolchain.cmake"
PROJECT_TEST_DIR_PATH =  "Tests/"
PROJECT_TEST_BUILD_BIN =  "Tests/build/bin"
TEST_MOCKS_DIR_PATH = PROJECT_TEST_DIR_PATH + "Mocks/"
TEST_CONF_PATH = PROJECT_TEST_DIR_PATH + "ner_test.conf"
CMOCK_RUBY_SCRIPT_PATH = "/cmock_portable/lib/cmock.rb"
EMBEDDED_BASE_CMOCK_CONFIG = EMBEDDED_BASE_TESTING_DIR_PATH + "cmock-config.yml"
PROJECT_CMOCK_CONFIG = PROJECT_TEST_DIR_PATH + "cmock-config.yml" 

data = {}
with open("Tests/ner_test.conf", "rb") as f:
        data = tomllib.load(f)
    
test_packages = data.get("test-packages", {})
tests = data.get("tests", {})
mocked_files = data.get("mocked-files", [])
core_sources = data.get("sources", [])
include_dirs = data.get("include-dirs", [])

def get_project_sources(test_package_name):

    source_files = []
    source_files += core_sources
    package_sources = test_packages[test_package_name].get("sources", [])
    source_files += [ps for ps in package_sources if ps not in source_files]

    mocked_source_dir = os.path.join(TEST_MOCKS_DIR_PATH, test_package_name) 

    for _, _, filenames in os.walk(mocked_source_dir):
        for fname in filenames:
            ext = fname.split(".")[1]
            if ext != "c":
                continue
            source_files.append(os.path.join(f"Tests/Mocks/{test_package_name}", fname))
    
    return source_files

def get_parent_mocks(test_package_name, visited):
    if test_package_name not in data["test-packages"]:
        raise KeyError(f"test-package {test_package_name} does not exist")
    
    if test_package_name in visited:
        return []

    files = data["test-packages"][test_package_name].get("mocked-files", [])
    if "parent-package" in data["test-packages"][test_package_name]:
        visited.append(test_package_name)
        files += get_parent_mocks(data["test-packages"][test_package_name]["parent-package"], visited)

    return files

def create_mocks(selected_test_packages):

    print("Creating Mocks...")

    for tp_name in selected_test_packages:
        tp_data = test_packages[tp_name]
       
        dir_path = os.path.join(TEST_MOCKS_DIR_PATH, tp_name)
        os.makedirs(dir_path, exist_ok=True)
        print(f"Created directory for test package: {tp_name}")

        files = tp_data.get("mocked-files", [])
        additional_mocks = mocked_files + get_parent_mocks(tp_name, [])

        for mock in additional_mocks:
            if mock not in files:
                files.append(mock)

        create_mocks_dir_p = subprocess.Popen(["mkdir", "-p", f"{TEST_MOCKS_DIR_PATH}"])
        create_mocks_dir_p.wait() 

        cmock_config_defined = False
        if os.path.isfile(PROJECT_CMOCK_CONFIG):
            cmock_config_defined = True

        for file_path in files:
            if cmock_config_defined:
                command = ["ruby", CMOCK_RUBY_SCRIPT_PATH, f"-o{PROJECT_CMOCK_CONFIG}", f"--mock_path={TEST_MOCKS_DIR_PATH + tp_name}", file_path]
            else:
                command = ["ruby", CMOCK_RUBY_SCRIPT_PATH, f"-o{EMBEDDED_BASE_CMOCK_CONFIG}", f"--mock_path={TEST_MOCKS_DIR_PATH + tp_name}", file_path]
            process = subprocess.Popen(command, text=True)
            retcode = process.wait()
            if (retcode != 0):
                return retcode
    
    return 0
    


def build_test(test_name, test_file, test_package, source_files, c_defines):

    print("BUILDING TEST: " + test_name)
    
    joined_sources =  " ".join(source_files) 
    joined_include_dirs = " ".join(include_dirs) 
    
    make_command = f"""
    make -f {os.path.join(EMBEDDED_BASE_TESTING_DIR_PATH, "Makefile")} \
    TEST_NAME={test_name} \
    TEST_FILE={test_file} \
    TEST_PACKAGE={test_package} \
    C_SOURCES="{joined_sources}" \
    C_DEFINES="{c_defines}" \
    INCLUDE_DIRS="{joined_include_dirs}"
    """

    command = ["sh", "-c", make_command]

    # Run Build
    return subprocess.Popen(command, text=True)

def get_selected_test_packages(selected_tests):
    selected_test_packages = []
    for test in selected_tests:
        if test not in data["tests"]:
            raise KeyError(f"Test '{test}' not found in [tests] section of ner_test.conf")
        elif "test-package" not in data["tests"][test]:
            raise KeyError(f"Test '{test}' missing required field 'test-package' in ner_test.conf")
        elif tests[test]["test-package"] not in test_packages:
            raise KeyError(f"Test package '{tests[test]['test-package']}' not found in [test-packages]")
        else:
            selected_test_packages.append(data["tests"][test]["test-package"])
    
    return selected_test_packages

def get_formatted_defines():
    c_defines = data.get("defines", [])
    formatted_defines = []
    for cd in c_defines:
        formatted_defines.append("-D" + cd)
    
    return " ".join(formatted_defines)


def build_tests(selected_tests):
    c_defines = get_formatted_defines()
    for t_name in selected_tests:
        t_data = tests[t_name]
        test_package = t_data["test-package"]
        test_file = t_data["test-file"]
        sources = get_project_sources(test_package)
        process = build_test(t_name, test_file, test_package, sources, c_defines)
        retcode = process.wait()
        if (retcode != 0):
            return retcode

    return 0

def run_test_bin(selected_tests):
    processes = []
    for t_name in selected_tests:
        if t_name in selected_tests:
            processes.append(subprocess.Popen(f"{PROJECT_TEST_BUILD_BIN}/{t_name}", shell=True, text=True))
    
    retcode = 0
    for p in processes:
        retcode = p.wait()
        if retcode != 0:
            return retcode
        
    return retcode

def main():
  
    if (len(sys.argv) > 1):
        selected_tests = sys.argv[1:]
    else:
        selected_tests = tests.keys()
    selected_test_packages = get_selected_test_packages(selected_tests)

    ret = 0

    # creates mocks for all the necessary test packages
    ret = create_mocks(selected_test_packages)
    if ret != 0:
        return ret

    # builds bin and obj files for running test
    ret = build_tests(selected_tests)
    if ret != 0:
        return ret

    # runs the test binary
    ret = run_test_bin(selected_tests)
    if ret != 0:
        return ret

if __name__ == "__main__":
    main()
        