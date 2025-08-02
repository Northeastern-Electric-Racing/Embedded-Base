#! /bin/bash

MOCK_OUTPUT_DIR="$1"

shift

for header in "$@"; do
    ruby /cmock_portable/lib/cmock.rb -ocmock_config.yml --mock_path="$MOCK_OUTPUT_DIR" "$header"
done
    

