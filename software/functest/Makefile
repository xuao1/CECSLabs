C_FILES := $(wildcard src/*.c)
NAMES ?= $(basename $(notdir $(C_FILES)))
BASE_PORT = ../base-port

.PHONY: all clean clean-all

all: $(NAMES)

%: src/%.c
	@NAMES=$* SRCS=src/$* BASE_PORT=$(BASE_PORT) make -s -f $(BASE_PORT)/Makefile

clean:
	@rm -rf build

clean-all:
	@rm -rf build
	@rm -rf $(BASE_PORT)/*/build
