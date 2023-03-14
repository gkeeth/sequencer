MAKEFILE_DIR = $(dir $(realpath $firstword $(MAKEFILE_LIST)))
PROJECT_DIR = $(realpath $(MAKEFILE_DIR)..)
TEST_DIR = $(PROJECT_DIR)/test

CPPUTEST_HOME = $(TEST_DIR)/cpputest
SRC_DIRS = $(PROJECT_DIR)
INCLUDE_DIRS = $(PROJECT_DIR)
INCLUDE_DIRS += $(CPPUTEST_HOME)/include
TEST_SRC_DIRS = $(TEST_DIR)
TEST_TARGET = test_runner

include $(CPPUTEST_HOME)/build/MakefileWorker.mk

# .PHONY: default
# default:
# 	$(info MAKEFILE_DIR: $(MAKEFILE_DIR))
# 	$(info PROJECT_DIR: $(PROJECT_DIR))
# 	$(info TEST_DIR: $(TEST_DIR))
