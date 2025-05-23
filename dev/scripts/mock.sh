#! /bin/bash

for header in "$@"; do
    ruby /cmock_portable/lib/cmock.rb -ocmock_config.yml "$header"
done
    

