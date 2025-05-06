#! /bin/bash

MOCKED_HEADERS=(
    "../../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/cmsis_os.h"
    "../platforms/stm32f405/include/can.h"
    "../../../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal.h"
)

MOCK_DIR="./mocks"
mkdir -p $MOCK_DIR

for header in "${MOCKED_HEADERS[@]}"; do
    ruby /cmock_portable/lib/cmock.rb "$header"
done

for mock in $MOCKS/Mock*.h; do
    base=$(basename $mock | sed 's/^Mock//')
    ln -sf $mock $MOCKS/$base
done