
UNITY_SRC = /cmock_portable/vendor/unity/src
CMOCK_SRC = /cmock_portable/src
PROJECT_DIR = /home/app

TEST_DIR = $(PROJECT_DIR)/Tests

TEST_BUILD = build
TEST_BIN = $(TEST_BUILD)/test_runner.out

MOCKS = mocks

CC = gcc

UNMOCKED_INCLUDES = \
	-I$(MOCKS) \
	-I$(UNITY_SRC) \
	-I$(CMOCK_SRC) \
	-Imanual_mocks \
	-I$(PROJECT_DIR)/Core/Inc \
	-I$(TEST_DIR)/Inc \
	-I$(PROJECT_DIR)/Drivers/Embedded-Base/platforms/stm32f405/include \
	-I$(PROJECT_DIR)/Drivers/Embedded-Base/middleware/include \
	-I$(PROJECT_DIR)/Drivers/Embedded-Base/general/include \
	-I$(PROJECT_DIR)/Drivers/Embedded-Base/os/inc \

C_INCLUDES = $(UNMOCKED_INCLUDES) 
CFLAGS = -Wall -Wextra -Wno-unused-variable -DTESTING $(C_INCLUDES)

C_SOURCES = \
	$(PROJECT_DIR)/Tests/Src/test_main.c \
	$(PROJECT_DIR)/Core/Src/pedals.c

.PHONY: all mock clean build

all: mock build
	./$(TEST_BUILD)/test_runner.out

mock:
	@echo "Generating mocks..."
	@bash $(PROJECT_DIR)/Drivers/Embedded-Base/dev/scripts/mock.sh  

build:
	@mkdir -p $(TEST_BUILD)
	$(CC) $(CFLAGS) $(C_SOURCES) $(CMOCK_SRC)/cmock.c $(UNITY_SRC)/unity.c -o $(TEST_BIN)


clean:
	rm -rf $(TEST_BUILD)
	rm -rf $(MOCKS)

print-debug:
	@echo $(C_INCLUDES)
