
from pathlib import Path
import subprocess
import re
import sys

def main(tests=None, test_packages=None):

    if test_packages == None:
        test_packages = get_test_packages()

    if tests == None:
        tests = test_packages

    # generate mocks for tests
    processes = []
    for test in tests:
        p = generate_mocks(test)
        if (p != None):
            processes.append(p)

    # wait for all mocks to finish generating
    for p in processes:
        retcode = p.wait()
        if retcode != 0:
            throw_error("failed to build tests", retcode)

    # run make on each test 
    processes = []
    for test in tests:
        if test in test_packages:
            processes.extend(handle_test_package(test))
        else:
            p = handle_test_file(test)
            if (p != None):
                processes.append(p)

    # wait for all builds to finish
    for p in processes:
        retcode = p.wait()
        if retcode != 0:
            throw_error("failed to build tests", retcode)

def get_mock_config(filename):
    mc = Path(f"Tests/mock_configs/{filename}.txt")
    if not mc.exists():
        mc = Path(f"Tests/mock_configs/{filename.split('.')[0]}.txt")
    if not mc.exists():
        return 
    
    return mc

def generate_mocks(test):    
    mc = get_mock_config(Path(test).stem)
    command = ["docker", "compose", "run", "--rm", "ner-gcc-arm", "sh", "-c", "cd Drivers/Embedded-Base/testing/ " 
                + f"&& make mocks MOCK_CONFIG={str(mc)}"]
    return subprocess.Popen(command, text=True)

def get_test_packages():
    mc_dir = Path("Tests/mock_configs")
    test_packages = []
    for file in mc_dir.iterdir():
        if file.is_file() and len(file.stem.split(".")) == 1:
            test_packages.append(file.stem)
    
    return test_packages

def get_tests_in_package(test_package):
    test_sources_dir = Path("Tests/Src")
    test_package_files = []
    for file in test_sources_dir.iterdir():
        if file.is_file() and re.search(f"^{test_package}.*.c$", file.name):
            test_package_files.append(file)
    
    return test_package_files

def handle_test_file(test_file):
    filepath = Path(test_file)
    if not filepath.exists():
        return

    mc = get_mock_config(filepath.stem)
    command = ["docker", "compose", "run", "--rm", "ner-gcc-arm", "sh", "-c", "cd Drivers/Embedded-Base/testing/ " 
                   + f"&& make TEST_SOURCE={test_file} MOCK_CONFIG={str(mc)}"]
    return subprocess.Popen(command, text=True)

def handle_test_package(test_package):
    test_files = get_tests_in_package(test_package)
    processes = []  
    for test_file in test_files:
        processes.append(handle_test_file(test_file))
    return processes

if __name__ == "__main__":
    main()

def throw_error(diag, retcode):
    print(diag)
    sys.exit(retcode)