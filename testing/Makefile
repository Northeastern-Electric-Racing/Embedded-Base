UNITY_SRC = /cmock_portable/vendor/unity
PROJECT_DIR = ../../..
EMBEDDED_BASE = ..

PROJECT_CORE = $(PROJECT_DIR)/Core
TEST_DIR = $(PROJECT_DIR)/Tests
PROJECT_SRC = $(PROJECT_CORE)/Src
TEST_BUILD = build
TEST_BIN = $(TEST_BUILD)/test_runner.out
# root test files must be called 	main_test.c
TEST_SRC = $(TEST_DIR)/Src/test_main.c
MOCKS = ./mocks

CC = gcc

C_INCLUDES = \
	-I$(TEST_DIR)/Inc \
	-I$(PROJECT_CORE)/Inc \
	-I$(UNITY_SRC) \
	-I$(EMBEDDED_BASE)/middleware/include \
	-I$(EMBEDDED_BASE)/general/include \
	-I$(EMBEDDED_BASE)/os/inc \
	-I$(MOCKS)

C_SOURCES := $(shell find $(PROJECT_SRC) $(EMBEDDED_BASE)/middleware/src  \
$(EMBEDDED_BASE)/os/src $(EMBEDDED_BASE)/general/src $(TEST_DIR)/Src -type f)

OBJECTS := $(patsubst %.c,$(TEST_BUILD)/%.o,$(notdir $(C_SOURCES)))
CFLAGS = -Wall -Wextra -Wno-unused-variable $(C_INCLUDES)

.PHONY: mock
mock:
	@echo "Running the mocking script..."
	@bash ../dev/scripts/mock.sh 

all: $(TEST_BIN)
	@echo "Running unit tests..."
	@echo $(TEST_BIN)

$(TEST_BIN): $(OBJECTS) $(TEST_SRC) $(UNITY_SRC)/unity.c | $(TEST_BUILD)
	$(CC) $(CFLAGS) $^ -o $@

$(TEST_BUILD)/%.o:
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $(firstword $(filter %/$(basename $(notdir $@)).c,$(C_SOURCES))) -o $@

clean:
	rm -rf $(TEST_BUILD)
	rm -rf $(MOCKS)/*
