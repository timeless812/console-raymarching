CC = gcc
CFLAGS = -Wall -O2 -Iinclude
LDFLAGS = -lncurses -lOpenCL -lm
BUILD = build
TARGET = $(BUILD)/raymarching

SOURCES := $(wildcard src/*.c)

compile: $(TARGET)

$(TARGET): $(SOURCES) | $(BUILD)
	$(CC) main.c $^ -o $@ $(LDFLAGS)

$(BUILD):
	mkdir -p $@

clean:
	rm -rf $(BUILD)

all: clean compile

.PHONY: all clean compile