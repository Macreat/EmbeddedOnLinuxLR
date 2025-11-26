SHELL := /bin/sh

CC ?= cc
CFLAGS ?= -O2 -Wall -Wextra -pedantic -std=c11

SRC_DIR := src/rtl/
BUILD_DIR := build
BINARY := assignment-sensor

# All .c sources
SRC := $(wildcard $(SRC_DIR)/*.c)

# Object files generated inside build/
OBJ := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRC))

.PHONY: all clean install uninstall build

all: build

# Build final binary
build: $(BUILD_DIR)/$(BINARY)

$(BUILD_DIR)/$(BINARY): $(OBJ)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $^

# Compile individual source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(SRC_DIR)/assignment_sensor.h
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

install: build
	install -d $(DESTDIR)/usr/local/bin
	install -m 0755 $(BUILD_DIR)/$(BINARY) $(DESTDIR)/usr/local/bin/

uninstall:
	rm -f $(DESTDIR)/usr/local/bin/$(BINARY)
