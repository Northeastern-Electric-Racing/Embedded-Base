
UNITY_SRC = /cmock_portable/vendor/unity/src
CMOCK_SRC = /cmock_portable/src
PROJECT_DIR = /home/app

TEST_DIR = $(PROJECT_DIR)/Tests

TEST_BUILD = build
TEST_BIN = $(TEST_BUILD)/test_runner.out

MOCKS = mocks

CC = gcc

MOCKED_INCLUDES = \
	-I$(PROJECT_DIR)/Core/Inc \
	-I$(PROJECT_DIR)/Drivers/Embedded-Base/platforms/stm32f405/include \
	-I$(PROJECT_DIR)/Drivers/Embedded-Base/middleware/include \
	-I$(PROJECT_DIR)/Drivers/Embedded-Base/general/include \
	-I$(PROJECT_DIR)/Drivers/Embedded-Base/os/inc \
	-Imanual_mocks \

UNMOCKED_INCLUDES = \
	-I$(MOCKS) \
	-I$(UNITY_SRC) \
	-I$(CMOCK_SRC) \
	-I$(TEST_DIR)/Inc \

C_INCLUDES = $(MOCKED_INCLUDES) $(UNMOCKED_INCLUDES) 
CFLAGS = -Wall -Wextra -Wno-unused-variable -DTESTING $(C_INCLUDES)

MOCK_C_SOURCES := $(wildcard $(MOCKS)/mock_*.c)
C_SOURCES = \
	$(PROJECT_DIR)/Tests/Src/test_source.c \
	$(CMOCK_SRC)/cmock.c \
	$(UNITY_SRC)/unity.c \
	$(MOCK_C_SOURCES) \
	$(foreach f,$(wildcard $(PROJECT_DIR)/Core/Src/*.c), \
		$(if $(or $(findstring stm,$(notdir $(f))),$(findstring sys,$(notdir $(f))),$(findstring freertos,$(notdir $(f))),$(findstring main,$(notdir $(f)))),,$(f)))

.PHONY: all mock clean build

all: build
	./$(TEST_BUILD)/test_runner.out

mock:
	rm -rf $(MOCKS)
	@echo "Generating mocks..."
	@bash $(PROJECT_DIR)/Drivers/Embedded-Base/dev/scripts/mock.sh $(MOCKED_INCLUDES)

build: mock
	@mkdir -p $(TEST_BUILD)
	$(CC) $(CFLAGS) $(C_SOURCES) -o $(TEST_BIN)


clean:
	rm -rf $(TEST_BUILD)
	rm -rf $(MOCKS)

