PROJECT = sequencer
BUILD_DIR = bin

CFILES = sequencer.c

DEVICE=stm32f031k6t6

# You shouldn't have to edit anything below here.
VPATH += $(SHARED_DIR)
INCLUDES += $(patsubst %,-I%, . $(SHARED_DIR))
OPENCM3_DIR=../libopencm3
OOCD_FILE ?= ../stm32f0_basic.cfg

include $(OPENCM3_DIR)/mk/genlink-config.mk
include ../rules.mk
include $(OPENCM3_DIR)/mk/genlink-rules.mk

debug:		$(PROJECT).debug
%.debug:	%.elf
	$(PREFIX)gdb $(PROJECT).elf -x gdbinit.gdb
.PHONY:	debug
