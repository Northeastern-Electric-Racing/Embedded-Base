#! /usr/bin/env python3

import tomli
import os
import subprocess

PROJECT_DIR_PATH = "/home/app/"
EMBEDDED_BASE_PATH = PROJECT_DIR_PATH + "Drivers/Embedded-Base/"
EMBEDDED_BASE_TESTING_DIR_PATH = EMBEDDED_BASE_PATH + "Testing/"
PROJECT_TEST_DIR_PATH = PROJECT_DIR_PATH + "Tests/"
TEST_MOCKS_DIR_PATH = PROJECT_TEST_DIR_PATH + "Mocks/"
TEST_CONF_PATH = PROJECT_TEST_DIR_PATH + "ner_test.conf"
CMOCK_RUBY_SCRIPT_PATH = "/cmock_portable/lib/cmock.rb"
CMOCK_CONFIG = EMBEDDED_BASE_TESTING_DIR_PATH + "cmock-config.yml"

with open("Tests/ner_test.conf", "rb") as f:
        data = tomli.load(f)
    
test_packages = data.get("test-packages", {})

def get_project_sources(test_package_name):
    # project_sources = os.path.join(PROJECT_DIR_PATH, "Core", "Src")
    project_source_dir = "Core/Src"
    excluded_substrings = ("tx", "stm", "sys", "trace", "main")

    tp_files = test_packages[test_package_name]["files"]
    tp_files_no_ext = [os.path.splitext(os.path.basename(f))[0] for f in tp_files]
    print(tp_files_no_ext)

    files = []
    for root, _, filenames in os.walk(project_source_dir):
        for fname in filenames:
            
            if any(sub in fname for sub in excluded_substrings):
                continue

            file_no_ext = fname.split(".")[0]
            if file_no_ext not in tp_files_no_ext:
                files.append(os.path.join(root, fname))

    # mocked_source_dir = os.path.join(TEST_MOCKS_DIR_PATH, test_package_name) 
    mocked_source_dir = "Tests/Mocks"

    for root, _, filenames in os.walk(mocked_source_dir):
        for fname in filenames:
            ext = fname.split(".")[1]
            if ext != "c":
                continue
            files.append(os.path.join(root, fname))

    return files


def create_mocks():

    processes = []
    for tp_name, tp_data in test_packages.items():
        dir_path = os.path.join(TEST_MOCKS_DIR_PATH, tp_name)
        os.makedirs(dir_path, exist_ok=True)
        print(f"Created directory for test package: {tp_name}")

        files = tp_data.get("files", [])
        for file_path in files:
            command = ["ruby", CMOCK_RUBY_SCRIPT_PATH, f"-o{CMOCK_CONFIG}", f"--mock_path={TEST_MOCKS_DIR_PATH + tp_name}", file_path]
            processes.append(subprocess.Popen(command, text=True))

    # wait for all mocks to be created
    for p in processes:
        p.wait()

def main():
    create_mocks()

if __name__ == "__main__":
    main()
    

    



    