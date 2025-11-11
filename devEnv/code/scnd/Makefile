CC ?= gcc
CFLAGS ?= -Wall -Wextra -std=c11 -pedantic
SRCS := sensor/sensor.c \
        actuators/led_actuator.c \
        actuators/buzzer_actuator.c \
        controller/ctl.c

BUILD_DIR := build
TARGET64 := $(BUILD_DIR)/ctl64
TARGET32 := $(BUILD_DIR)/ctl32

.PHONY: all ctl64 ctl32 clean

all: ctl64

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

ctl64: $(TARGET64)

$(TARGET64): $(SRCS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -m64 $(SRCS) -o $@

ctl32: $(TARGET32)

$(TARGET32): $(SRCS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -m32 $(SRCS) -o $@

clean:
	rm -rf $(BUILD_DIR)
