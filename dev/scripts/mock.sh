#! /bin/bash


MOCK_DIR="./mocks"
mkdir -p $MOCK_DIR

for header in "$@"; do
    ruby /cmock_portable/lib/cmock.rb -ocmock_config.yml "$header"
done
    

