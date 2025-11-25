SHELL := /bin/sh
CC ?= cc
CFLAGS ?= -O2 -Wall -Wextra -pedantic
BINARY := assignment-sensor
SRC := src/rtl/assignment_sensor.c
BUILD_DIR := build

.PHONY: all build clean install uninstall

all: build

$(BUILD_DIR)/$(BINARY): $(SRC)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $<

build: $(BUILD_DIR)/$(BINARY)

clean:
	rm -rf $(BUILD_DIR)

install: build
	install -d $(DESTDIR)/usr/local/bin
	install -m 0755 $(BUILD_DIR)/$(BINARY) $(DESTDIR)/usr/local/bin/$(BINARY)

uninstall:
	rm -f $(DESTDIR)/usr/local/bin/$(BINARY)
