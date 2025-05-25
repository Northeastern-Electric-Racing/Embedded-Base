
import os
from pathlib import Path

PROJECT_DIR = Path("/home/app")  
TEST_DIR = PROJECT_DIR/"Tests"

test_name = "" # argument
out_type = "" #argument

mock_config_file = f"{TEST_DIR}/{test_name}.txt"
with open(mock_config_file) as f:
    headers_to_mock = Path([PROJECT_DIR/line.strip() for line in f if line.strip()])

for path in headers_to_mock:
    mocked_includes = f"${test_name}_mocks/{path.name}"

if out_type == "inc":
    print(' '.join(mocked_includes))

CORE_SOURCE_DIR = PROJECT_DIR/"Core"/"Src"
directory = Path(CORE_SOURCE_DIR)  
core_files = [f for f in directory.iterdir() if f.is_file()]

header_names = [header.name[:len(header.name) - 2] for header in headers_to_mock]
for file in core_files:
    if (file.name[:len(file.name) - 2] in header_names)





