
CC = gcc

PROJECT_DIR = /home/app

UNITY_SRC = /cmock_portable/vendor/unity/src
CMOCK_SRC = /cmock_portable/src

TEST_DIR = $(PROJECT_DIR)/Tests
TEST_BUILD = build
TEST_BIN = $(TEST_BUILD)/test_runner.out
MOCKS = mocks


MOCKED_CORE_INCLUDES = $(shell sed "s|^|$(PROJECT_DIR)/|" $(TEST_DIR)/mocked_includes.txt)
EXCLUDED_C_SOURCES := $(patsubst $(PROJECT_DIR)/Core/Inc/%.h,$(PROJECT_DIR)/Core/Src/%.c, $(MOCKED_CORE_INCLUDES))
CORE_ALL_SOURCES := $(wildcard $(PROJECT_DIR)/Core/Src/*.c)
CORE_SOURCES := $(filter-out $(EXCLUDED_C_SOURCES), \
    $(foreach f,$(CORE_ALL_SOURCES), \
        $(if $(or $(findstring stm,$(notdir $(f))), \
                  $(findstring sys,$(notdir $(f))), \
				  $(findstring main,$(notdir $(f))), \
                  $(findstring rtos,$(notdir $(f)))),, $(f))))

MOCKED_INCLUDES :=  \
	manual_mocks/stub_functions.h \
	$(PROJECT_DIR)/Drivers/Embedded-Base/middleware/include/debounce.h \
	$(PROJECT_DIR)/Drivers/Embedded-Base/middleware/include/c_utils.h \
	$(PROJECT_DIR)/Drivers/Embedded-Base/middleware/include/timer.h \
	$(PROJECT_DIR)/Drivers/Embedded-Base/platforms/stm32f405/include/can.h \
	$(MOCKED_CORE_INCLUDES)
	
UNMOCKED_INCLUDES = \
	-Imanual_mocks \
	-I$(PROJECT_DIR)/Core/Inc \
	-I$(MOCKS) \
	-I$(UNITY_SRC) \
	-I$(CMOCK_SRC) \
	-I$(TEST_DIR)/Inc \
	-I$(PROJECT_DIR)/Drivers/Embedded-Base/platforms/stm32f405/include \
	-I$(PROJECT_DIR)/Drivers/Embedded-Base/middleware/include \
	-I$(PROJECT_DIR)/Drivers/Embedded-Base/general/include \
	-I$(PROJECT_DIR)/Drivers/Embedded-Base/os/inc \
	-I$(PROJECT_DIR)/Drivers/adbms/lib/inc \
	-I$(PROJECT_DIR)/Drivers/adbms/program/inc


C_INCLUDES = $(MOCKED_INCLUDES) $(UNMOCKED_INCLUDES) 
CFLAGS = -Wall -Wextra -DTESTING $(C_INCLUDES)

MOCK_HEADERS := $(patsubst %,$(MOCKS)/mock_%.h,$(basename $(notdir $(MOCKED_INCLUDES))))
MOCK_C_SOURCES := $(patsubst %.h,%.c,$(MOCK_HEADERS))

C_SOURCES = \
	$(PROJECT_DIR)/Tests/Src/test_source.c \
	$(CMOCK_SRC)/cmock.c \
	$(UNITY_SRC)/unity.c \
	$(PROJECT_DIR)/Drivers/Embedded-Base/middleware/src/bitstream.c \
	$(MOCK_C_SOURCES) \
	$(CORE_SOURCES)


.PHONY: all clean build

all: build
	./$(TEST_BUILD)/test_runner.out

build: $(MOCK_C_SOURCES)
	@echo $(MOCKED_INCLUDES)
	@mkdir -p $(TEST_BUILD)
	$(CC) $(CFLAGS) $(C_SOURCES) -o $(TEST_BIN) -lm

$(MOCKS)/mock_%.c $(MOCKS)/mock_%.h:
	@bash $(PROJECT_DIR)/Drivers/Embedded-Base/dev/scripts/mock.sh $(MOCKED_INCLUDES)
	@touch $@  

clean:
	rm -rf $(TEST_BUILD)
	rm -rf $(MOCKS)

debug-print:
	@echo $(MOCKED_INCLUDES)