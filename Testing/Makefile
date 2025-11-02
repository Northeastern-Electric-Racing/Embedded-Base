# Makefile for NER Testing Framework
# Usage:
#   make TEST_NAME=charging TEST_FILE=Tests/Src/test_analyzer.c TEST_PACKAGE=dcl_calcs \
#        C_SOURCES="Core/Src/foo.c Core/Src/bar.c" \
#        INCLUDE_DIRS="/home/app/Core/Inc /home/app/Tests/Inc" all

# Compiler
CC := gcc
CFLAGS := -std=c11 -I$(UNITY_SRC) -I$(CMOCK_SRC)

# Project directories
PROJECT_DIR := /home/app
TEST_DIR := $(PROJECT_DIR)/Tests
MOCKS_DIR := $(TEST_DIR)/Mocks
TEST_BUILD := $(TEST_DIR)/build
UNITY_SRC := /cmock_portable/vendor/unity/src
CMOCK_SRC := /cmock_portable/src

# Command line variables (must be provided)
TEST_NAME ?=
TEST_FILE ?=
TEST_PACKAGE ?=
C_SOURCES ?=
INCLUDE_DIRS ?=

# Sanity check
ifeq ($(TEST_NAME),)
$(error You must define TEST_NAME)
endif
ifeq ($(TEST_FILE),)
$(error You must define TEST_FILE)
endif
ifeq ($(TEST_PACKAGE),)
$(error You must define TEST_PACKAGE)
endif
ifeq ($(C_SOURCES),)
$(error You must define C_SOURCES)
endif

# Output executable
TARGET := $(TEST_BUILD)/$(TEST_NAME)

# Include dirs
INCLUDES := $(addprefix -I,$(INCLUDE_DIRS)) -I$(MOCKS_DIR)/$(TEST_PACKAGE) -I$(TEST_DIR)/Inc

# All source files
SOURCES := $(CMOCK_SRC)/cmock.c $(UNITY_SRC)/unity.c $(PROJECT_DIR)/$(TEST_FILE) $(C_SOURCES)

# Object files
OBJS := $(patsubst %.c,$(TEST_BUILD)/%.o,$(SOURCES))

# Default target
all: $(TARGET)

# Link executable
$(TARGET): $(OBJS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@

# Compile object files
$(TEST_BUILD)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Clean
clean:
	rm -rf $(TEST_BUILD)
