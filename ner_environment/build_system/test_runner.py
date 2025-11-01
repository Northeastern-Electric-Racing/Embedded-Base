import tomli
import os
import subprocess

PROJECT_DIR_PATH = "/home/app/"
EMBEDDED_BASE_PATH = PROJECT_DIR_PATH + "Drivers/Embedded-Base/"
EMBEDDED_BASE_TESTING_DIR_PATH = EMBEDDED_BASE_PATH + "Testing/"
PROJECT_TEST_DIR_PATH = PROJECT_DIR_PATH + "Tests/"
TEST_CONF_PATH = PROJECT_TEST_DIR_PATH + "ner_test.conf"
CMOCK_RUBY_SCRIPT_PATH = "/cmock_portable/lib/cmock.rb"
CMOCK_CONFIG = EMBEDDED_BASE_TESTING_DIR_PATH + "cmock-config.yml"

with open(TEST_CONF_PATH, "rb") as f:
    data = tomli.load(f)

def create_mocks():
    test_packages = data.get("test-packages", {})
    processes = []

    for tp_name, tp_data in test_packages.items():
        dir_path = os.path.join(PROJECT_TEST_DIR_PATH, tp_name)
        os.makedirs(dir_path, exist_ok=True)
        print(f"Created directory for test package: {tp_name}")

        files = tp_data.get("files", [])
        for file_name in files:
            command = ["ruby", CMOCK_RUBY_SCRIPT_PATH, f"-o{CMOCK_CONFIG}", f"--mock_path={PROJECT_TEST_DIR_PATH + tp_name}", file_name]
            processes.append(subprocess.Popen(command, text=True))

    # wait for all mocks to be created
    for p in processes:
        p.wait()


    

    



    