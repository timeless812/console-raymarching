CC = gcc
CFLAGS = -Wall -O2 -Iinclude
LDFLAGS = -lncurses -lm
BUILD = build
TARGET = $(BUILD)/raymarching

SOURCES := $(filter-out src/opencl.c, $(wildcard src/*.c))

OPENCL ?= 0
ifeq ($(OPENCL), 1)
    CFLAGS += -DOPENCL
    LDFLAGS += -lOpenCL
    SOURCES += src/opencl.c
endif

compile: $(TARGET)

$(TARGET): $(SOURCES) | $(BUILD)
	$(CC) main.c $^ -o $@ $(CFLAGS) $(LDFLAGS)

$(BUILD):
	mkdir -p $@

clean:
	rm -rf $(BUILD)

all: clean compile

.PHONY: all clean compile