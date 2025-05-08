#! /bin/bash

HEADERS=(
    "/home/app/Core/Inc/dti.h"
    "/home/app/Drivers/Embedded-Base/platforms/stm32f405/include/can.h"
)

#for arg in "$@"; do
#    if [[ "$arg" == -I* ]]; then
#        INCLUDE_PATHS+=("${arg:2}")
#    fi
#done

MOCK_DIR="./mocks"
mkdir -p $MOCK_DIR

for header in "${HEADERS[@]}"; do
ruby /cmock_portable/lib/cmock.rb -ocmock_config.yml ${header}
done

#for include_path in "${INCLUDE_PATHS[@]}"; do
#    while IFS= read -r -d '' header; do
#        base=$(basename "$header")
#        [[ "$base" == *stm* ]] && continue
#        [[ "$base" == *cmsis_os* ]] && continue
#        ruby /cmock_portable/lib/cmock.rb "$header"
#    done < <(find "$include_path" -type f -name '*.h' -print0)
#done
    

