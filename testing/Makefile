
CC = gcc

PROJECT_DIR = /home/app

UNITY_SRC = /cmock_portable/vendor/unity/src
CMOCK_SRC = /cmock_portable/src

TEST_DIR = $(PROJECT_DIR)/Tests
TEST_BUILD = build
MOCKS = mocks
MOCKS_STAMP := $(MOCKS)/.mocks_generated

TEST_SOURCES := $(wildcard $(TEST_DIR)/Src/test_*.c)
TEST_NAMES := $(basename $(notdir $(TEST_SOURCES)))
ifeq ($(TEST_FILES),)
  TARGETS := $(addprefix $(TEST_BUILD)/, $(addsuffix .out, $(TEST_NAMES)))
else
  TARGETS := $(addprefix $(TEST_BUILD)/, $(addsuffix .out, $(TEST_FILES)))
endif


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
	@for t in $(TARGETS); do ./$$t; done

build: $(TARGETS)

$(MOCKS_STAMP): $(MOCKED_INCLUDES)
	@mkdir -p $(MOCKS)
	@bash $(PROJECT_DIR)/Drivers/Embedded-Base/dev/scripts/mock.sh $(MOCKED_INCLUDES)
	@touch $@

$(TEST_BUILD)/%.out: $(TEST_DIR)/Src/%.c $(MOCKS_STAMP)
	@mkdir -p $(TEST_BUILD)
	$(CC) $(CFLAGS) \
		$(TEST_DIR)/Src/$*.c \
		$(CMOCK_SRC)/cmock.c \
		$(UNITY_SRC)/unity.c \
		$(PROJECT_DIR)/Drivers/Embedded-Base/middleware/src/bitstream.c \
		$(MOCK_C_SOURCES) \
		$(CORE_SOURCES) \
		-o $@ -lm


$(MOCKS)/mock_%.c $(MOCKS)/mock_%.h:
	@bash $(PROJECT_DIR)/Drivers/Embedded-Base/dev/scripts/mock.sh $(MOCKED_INCLUDES)

clean:
	rm -rf $(TEST_BUILD)
	rm -rf $(MOCKS)

list:
	@for name in $(TEST_NAMES); do echo $$name; done