
CC = gcc

PROJECT_DIR = /home/app

UNITY_SRC = /cmock_portable/vendor/unity/src
CMOCK_SRC = /cmock_portable/src

# MOCK_CONFIG IS A PARAMETER
# TEST_SOURCE IS A PARAMETER
TEST_FILENAME = $(basename $(notdir $(TEST_SOURCE)))
MOCK_CONFIG_FILENAME = $(basename $(notdir $(MOCK_CONFIG)))

TEST_DIR = $(PROJECT_DIR)/Tests
TEST_BUILD = build
MOCKS = mocks/$(MOCK_CONFIG_FILENAME)_mocks
MOCKS_STAMP := $(MOCKS)/.mocks_generated

TARGET := $(addprefix $(TEST_BUILD)/, $(TEST_FILENAME))

MOCKED_CORE_INCLUDES = $(foreach f, $(shell cat $(PROJECT_DIR)/$(MOCK_CONFIG)), \
                           $(addprefix $(PROJECT_DIR)/, $(f)))
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
	
C_INCLUDES = \
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


CFLAGS = -w -DTESTING $(C_INCLUDES)

MOCK_HEADERS := $(patsubst %,$(MOCKS)/mock_%.h,$(basename $(notdir $(MOCKED_INCLUDES))))
MOCK_C_SOURCES := $(patsubst %.h,%.c,$(MOCK_HEADERS))
C_SOURCES = \
	$(CMOCK_SRC)/cmock.c \
	$(UNITY_SRC)/unity.c \
	$(PROJECT_DIR)/Drivers/Embedded-Base/middleware/src/bitstream.c \
	$(MOCK_C_SOURCES) \
	$(CORE_SOURCES) 

.PHONY: all clean build

all: build
	./$(TARGET)

build: $(TARGET)

$(MOCKS_STAMP): $(MOCKED_INCLUDES_FILE) $(MOCKED_INCLUDES)
	@mkdir -p $(MOCKS)
	@bash $(PROJECT_DIR)/Drivers/Embedded-Base/dev/scripts/mock.sh $(MOCKS) $(MOCKED_INCLUDES)
	@touch $@

$(TARGET): $(MOCKS_STAMP)
	@mkdir -p $(TEST_BUILD)
	$(CC) $(CFLAGS) \
		$(PROJECT_DIR)/$(TEST_SOURCE) \
		$(CMOCK_SRC)/cmock.c \
		$(UNITY_SRC)/unity.c \
		$(PROJECT_DIR)/Drivers/Embedded-Base/middleware/src/bitstream.c \
		$(MOCK_C_SOURCES) \
		$(CORE_SOURCES) \
		-o $@ -lm

clean:
	rm -rf $(TEST_BUILD)
	rm -rf mocks


	