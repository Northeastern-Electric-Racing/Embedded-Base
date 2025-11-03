

# Compiler
CC := gcc

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
C_INCLUDES := -I$(UNITY_SRC) -I$(CMOCK_SRC) $(addprefix -I,$(INCLUDE_DIRS)) -IDrivers/Embedded-Base/Testing/manual_mocks -I$(MOCKS_DIR)/$(TEST_PACKAGE) -I$(TEST_DIR)/Inc 

CFLAGS = -w -DTESTING $(C_INCLUDES)

TEST_SOURCES = $(CMOCK_SRC)/cmock.c $(UNITY_SRC)/unity.c

# All source files
SOURCES := $(TEST_FILE) $(C_SOURCES)
TEST_SRCS := $(CMOCK_SRC)/cmock.c $(UNITY_SRC)/unity.c

# Object files
OBJS := $(patsubst %.c,$(TEST_BUILD)/%.o,$(SOURCES))
TEST_OBJS = $(CMOCK_SRC)/cmock.o $(UNITY_SRC)/unity.o

ALL_OBJS := $(OBJS) $(TEST_OBJS)

# Default target
all: $(TARGET)

# Link executable
$(TARGET): $(ALL_OBJS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $^ -o $@  -lm

# Compile object files
$(TEST_BUILD)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@ -lm

$(TEST_BUILD)/cmock.o: $(CMOCK_SRC)/cmock.c
	@mkdir -p $(dir $@)
	@echo "Compiling $< -> $@"
	$(CC) $(CFLAGS) -c $< -o $@

$(TEST_BUILD)/unity.o: $(UNITY_SRC)/unity.c
	@mkdir -p $(dir $@)
	@echo "Compiling $< -> $@"
	$(CC) $(CFLAGS) -c $< -o $@


# Clean
clean:
	rm -rf $(TEST_BUILD)
