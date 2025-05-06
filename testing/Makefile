UNITY_SRC = /cmock_portable/vendor/unity/src
CMOCK_SRC = /cmock_portable/src
PROJECT_DIR = /home/app

PROJECT_CORE = $(PROJECT_DIR)/Core
TEST_DIR = $(PROJECT_DIR)/Tests
PROJECT_SRC = $(PROJECT_CORE)/Src
TEST_BUILD = build
TEST_BIN = $(TEST_BUILD)/test_runner.out
TEST_SRC = $(TEST_DIR)/Src/test_main.c
MOCKS = ./mocks

CC = gcc

CMOCK_INCLUDES = \
	-I$(TEST_DIR)/Inc \
	-I$(UNITY_SRC) \
	-I$(CMOCK_SRC) \
	-I$(MOCKS)

FOREIGN_INCLUDES = \
	-I$(PROJECT_CORE)/Inc \
	-I$(PROJECT_DIR)/Drivers/Embedded-Base/middleware/include \
	-I$(PROJECT_DIR)/Drivers/Embedded-Base/general/include \
	-I$(PROJECT_DIR)/Drivers/Embedded-Base/os/inc \
	-I$(PROJECT_DIR)/Drivers/STM32F4xx_HAL_Driver/Inc \
	-I$(PROJECT_DIR)/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy \
	-I$(PROJECT_DIR)/Drivers/CMSIS/Device/ST/STM32F4xx/Include \
	-I$(PROJECT_DIR)/Drivers/Embedded-Base/platforms/stm32f405/include \
	-I$(PROJECT_DIR)/Middlewares/Third_Party/FreeRTOS/Source/include \
	-I$(PROJECT_DIR)/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 \
	-I$(PROJECT_DIR)/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F

C_INCLUDES = $(FOREIGN_INCLUDES) $(CMOCK_INCLUDES)
CFLAGS = -Wall -Wextra -Wno-unused-variable $(C_INCLUDES)

.PHONY: all mock clean build

all: mock build

mock:
	@echo "Generating mocks..."
	@bash $(PROJECT_DIR)/Drivers/Embedded-Base/dev/scripts/mock.sh $(FOREIGN_INCLUDES)

build:
	@echo "Collecting sources after mock generation..."
	$(eval C_SOURCES := $(shell find  \
		$(TEST_DIR)/Src \
		$(PROJECT_SRC) \
		$(PROJECT_DIR)/Drivers/Embedded-Base/middleware/src  \
		$(PROJECT_DIR)/Drivers/Embedded-Base/os/src \
		$(PROJECT_DIR)/Drivers/Embedded-Base/general/src \
		$(PROJECT_DIR)/Drivers/STM32F4xx_HAL_Driver/Src \
		$(PROJECT_DIR)/Middlewares/Third_Party/FreeRTOS/Source \
		$(MOCKS) -name '*.c'))
	$(eval OBJECTS := $(patsubst %.c,$(TEST_BUILD)/%.o,$(notdir $(C_SOURCES))))
	@mkdir -p $(TEST_BUILD)
	$(CC) $(CFLAGS) $(C_SOURCES) $(CMOCK_SRC)/cmock.c $(UNITY_SRC)/unity.c -o $(TEST_BIN)
	@echo "Built $(TEST_BIN)"

clean:
	rm -rf $(TEST_BUILD)
	rm -rf $(MOCKS)/*
