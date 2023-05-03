PROJECT = sequencer
BUILD_DIR = bin

# add project c files that don't need to be tested here
NO_TEST_CFILES = sequencer.c system_clocks.c step_leds.c
NO_TEST_CFILES += platform.c platform_uart.c platform_adc.c platform_pwm.c

# Add project c files that do need to be tested here
TEST_CFILES = utils.c tempo_and_duty.c uart.c switch.c led.c pwm.c platform_utils.c

CFILES = $(NO_TEST_CFILES) $(TEST_CFILES)

DEVICE=stm32f031k6t6

# CppUTest setup
TEST_DIR = test
MOCKS_DIR = test/mocks
TEST_BUILD_DIR = $(TEST_DIR)/$(BUILD_DIR)
TEST_SRC := $(wildcard $(TEST_DIR)/*.cpp)
MOCKS_SRC := $(wildcard $(MOCKS_DIR)/*.c) $(wildcard $(MOCKS_DIR)/*.cpp)
TEST_OBJS := $(patsubst %.cpp,$(TEST_BUILD_DIR)/%.o,$(notdir $(TEST_SRC)))
MOCKS_OBJS := $(patsubst %.cpp,$(TEST_BUILD_DIR)/%.o,$(patsubst %.c,$(TEST_BUILD_DIR)/%.o,$(notdir $(MOCKS_SRC))))
TEST_CFILE_OBJS := $(patsubst %.c,$(TEST_BUILD_DIR)/%.o,$(TEST_CFILES))
CPPUTEST_HOME = test/cpputest
TEST_INCLUDES = -I$(CPPUTEST_HOME)/include -I$(TEST_DIR) -I$(MOCKS_DIR) -I.
TEST_CC = gcc
# TODO: may want to make a separate LDLIBS variable for tests
TEST_LDLIBS += -L$(CPPUTEST_HOME)/build/lib -lCppUTest -lCppUTestExt
TEST_TARGET = $(PROJECT)_tests
TEST_CPPFLAGS += -g -O0
TEST_CPPFLAGS += -fno-common
TEST_CPPFLAGS += -Wextra -Wshadow -Wconversion
TEST_CFLAGS   += -Wno-unused-variable -Wimplicit-function-declaration
TEST_CFLAGS   += -Wstrict-prototypes -Wmissing-prototypes -Wredundant-decls

CSTD = -std=c17
CXXSTD = -std=c++20

# command line args for test binary
CPPUTEST_FLAGS = -c

# You shouldn't have to edit anything below here.
VPATH += $(SHARED_DIR)
INCLUDES += $(patsubst %,-I%, . $(SHARED_DIR))
OPENCM3_DIR=../libopencm3
OOCD_FILE ?= ../stm32f0_basic.cfg

.PHONY: all
all:	$(PROJECT).elf $(TEST_TARGET)

include $(OPENCM3_DIR)/mk/genlink-config.mk
include ../rules.mk
include $(OPENCM3_DIR)/mk/genlink-rules.mk

# make sure clean can clean up after tests
GENERATED_BINS += $(TEST_BUILD_DIR)

.PHONY:	debug
debug:		$(PROJECT).debug
%.debug:	%.elf
	$(PREFIX)gdb $(PROJECT).elf -x gdbinit.gdb

# test files
$(TEST_BUILD_DIR)/%.o: $(TEST_DIR)/%.cpp
	@printf "  CXX\t$<\n"
	@mkdir -p $(dir $@)
	$(Q)$(CXX) $(TEST_CXXFLAGS) $(TEST_CPPFLAGS) $(TEST_INCLUDES) -o $@ -c $<

# test fake/mock C files
$(TEST_BUILD_DIR)/%.o: $(MOCKS_DIR)/%.c
	@printf "  CC\t$<\n"
	@mkdir -p $(dir $@)
	$(Q)$(TEST_CC) $(TEST_CFLAGS) $(TEST_CPPFLAGS) $(TEST_INCLUDES) -o $@ -c $<

# test fake/mock C++ files
$(TEST_BUILD_DIR)/%.o: $(MOCKS_DIR)/%.cpp
	@printf "  CXX\t$<\n"
	@mkdir -p $(dir $@)
	$(Q)$(CXX) $(TEST_CXXFLAGS) $(TEST_CPPFLAGS) $(TEST_INCLUDES) -o $@ -c $<

# application source files for test
$(TEST_BUILD_DIR)/%.o: %.c
	@printf "  CC\t$<\n"
	@mkdir -p $(dir $@)
	$(Q)$(TEST_CC) $(TEST_CFLAGS) $(TEST_CPPFLAGS) $(TEST_INCLUDES) -o $@ -c $<

# TODO: may need to add libs and ldscript here, not sure yet
TEST_DEPS = $(TEST_OBJS) $(TEST_CFILE_OBJS) $(MOCKS_OBJS)
$(TEST_TARGET):	$(TEST_DEPS)
	@printf "  TEST_DEPS: $(TEST_DEPS)\n"
	@printf "  TEST_OBJS: $(TEST_OBJS)\n"
	@printf "  MOCKS_OBJS: $(MOCKS_OBJS)\n"
	@printf "  TEST_CFILE_OBJS: $(TEST_CFILE_OBJS)\n"
	@printf "  TEST_LDLIBS: $(TEST_LDLIBS)\n"
	@printf "  Linking \t$<\n"
	$(Q)$(CXX) -o $@ $^ $(TEST_LDLIBS) $(LDFLAGS)

# TODO: make a static library for test files

.PHONY: test
test: $(TEST_TARGET)
	./$(TEST_TARGET) $(CPPUTEST_FLAGS)

.PHONY: print_vars
print_vars:
	$(info CC: $(CC))
	$(info CXX: $(CXX))
	$(info COMPILE.cpp: $(COMPILE.cpp))
	$(info COMPILE.c: $(COMPILE.c))
	$(info TEST_CFLAGS: $(TEST_CFLAGS))
	$(info TEST_CPPFLAGS: $(TEST_CPPFLAGS))
	$(info TEST_CXXFLAGS: $(TEST_CXXFLAGS))
	$(info OBJS: $(OBJS))
	$(info TEST_SRC: $(TEST_SRC))
	$(info MOCKS_SRC: $(MOCKS_SRC))
	$(info TEST_OBJS: $(TEST_OBJS))
	$(info MOCKS_OBJS: $(MOCKS_OBJS))
	$(info TEST_BUILD_DIR: $(TEST_BUILD_DIR))
	$(info TEST_DEPS: $(TEST_DEPS))
	$(info GENERATED_BINS: $(GENERATED_BINS))


