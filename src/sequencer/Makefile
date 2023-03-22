PROJECT = sequencer
BUILD_DIR = bin

CFILES = sequencer.c

# Add project c files that need to be tested here
TEST_CFILES = tempo_and_duty.c utils.c

DEVICE=stm32f031k6t6

# CppUTest setup
TEST_DIR = test
MOCKS_DIR = test/mocks
TEST_BUILD_DIR = $(TEST_DIR)/$(BUILD_DIR)
TEST_SRC := $(wildcard $(TEST_DIR)/*.cpp)
TEST_OBJS := $(patsubst %.cpp,$(TEST_BUILD_DIR)/%.o,$(notdir $(TEST_SRC)))
TEST_CFILE_OBJS := $(patsubst %.c,$(TEST_BUILD_DIR)/%.o,$(TEST_CFILES))
CPPUTEST_HOME = test/cpputest
TEST_INCLUDES = -I$(CPPUTEST_HOME)/include -I.
TEST_CC = gcc
# TODO: may want to make a separate LDLIBS variable for tests
TEST_LDLIBS += -L$(CPPUTEST_HOME)/build/lib -lCppUTest -lCppUTestExt
TEST_TARGET = $(PROJECT)_tests
CPPUTEST_FLAGS = -c

# You shouldn't have to edit anything below here.
VPATH += $(SHARED_DIR)
INCLUDES += $(patsubst %,-I%, . $(SHARED_DIR))
OPENCM3_DIR=../libopencm3
OOCD_FILE ?= ../stm32f0_basic.cfg

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
	$(Q)$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(TEST_INCLUDES) -o $@ -c $<

# source files
$(TEST_BUILD_DIR)/%.o: %.c
	@printf "  CC\t$<\n"
	@mkdir -p $(dir $@)
	$(Q)$(TEST_CC) $(CFLAGS) $(CPPFLAGS) $(TEST_INCLUDES) -o $@ -c $<

# TODO: may need to add libs and ldscript here, not sure yet
TEST_DEPS = $(TEST_OBJS) $(MOCKS_OBJS) $(TEST_CFILE_OBJS)
$(TEST_TARGET):	$(TEST_DEPS)
	@printf "  Linking \t$<\n"
	$(Q)$(CXX) -o $@ $^ $(TEST_LDLIBS) $(LDFLAGS)

# TODO: make a static library for test files

# TODO: this may not actually be phony, TBD
.PHONY: test
test:		$(TEST_TARGET)
	./$(TEST_TARGET) $(CPPUTEST_FLAGS)

.PHONY: print_vars
print_vars:
	$(info CC: $(CC))
	$(info CXX: $(CXX))
	$(info COMPILE.cpp: $(COMPILE.cpp))
	$(info COMPILE.c: $(COMPILE.c))
	$(info OBJS: $(OBJS))
	$(info TEST_SRC: $(TEST_SRC))
	$(info TEST_OBJS: $(TEST_OBJS))
	$(info TEST_BUILD_DIR: $(TEST_BUILD_DIR))
	$(info TEST_DEPS: $(TEST_DEPS))
	$(info GENERATED_BINS: $(GENERATED_BINS))


