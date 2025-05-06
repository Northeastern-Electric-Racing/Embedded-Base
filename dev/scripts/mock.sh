#! /bin/bash

INCLUDE_PATHS=()

for arg in "$@"; do
    if [[ "$arg" == -I* ]]; then
        INCLUDE_PATHS+=("${arg:2}")
    fi
done

MOCK_DIR="./mocks"
mkdir -p $MOCK_DIR

for include_path in "${INCLUDE_PATHS[@]}"; do
    while IFS= read -r -d '' header; do
        base=$(basename "$header")
        [[ "$base" == *stm* ]] && continue
        [[ "$base" == *cmsis_os* ]] && continue
        ruby /cmock_portable/lib/cmock.rb "$header"
    done < <(find "$include_path" -type f -name '*.h' -print0)
done


