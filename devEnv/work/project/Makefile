SHELL := /bin/sh

CC     := cc
CFLAGS := -O2 -Wall -Wextra -pedantic -std=c11
LDLIBS := -lpigpio -lrt -lpthread

SRC_DIR   := software
BUILD_DIR := build

# Importante: cambiar nombre de binario para no chocar con build/sensor/
BINARY := sensor_app

.PHONY: all clean install uninstall

all: $(BUILD_DIR)/$(BINARY)

# Objetos (hechos a mano)
OBJ := \
	$(BUILD_DIR)/buzzer_actuator.o \
	$(BUILD_DIR)/led_actuator.o \
	$(BUILD_DIR)/ctl.o \
	$(BUILD_DIR)/sensor.o

# Link final
$(BUILD_DIR)/$(BINARY): $(OBJ)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(OBJ) -o $@ $(LDLIBS)

# -------------------------------------------
# --- Compilación manual por carpeta ---
# -------------------------------------------

$(BUILD_DIR)/buzzer_actuator.o: $(SRC_DIR)/actuators/buzzer_actuator.c $(SRC_DIR)/actuators/actuator.h
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/led_actuator.o: $(SRC_DIR)/actuators/led_actuator.c $(SRC_DIR)/actuators/actuator.h
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/ctl.o: $(SRC_DIR)/controller/ctl.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/sensor.o: $(SRC_DIR)/sensor/sensor.c $(SRC_DIR)/sensor/sensor.h
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# -------------------------------------------

clean:
	rm -rf $(BUILD_DIR)

install: all
	install -d $(DESTDIR)/usr/local/bin
	install -m 0755 $(BUILD_DIR)/$(BINARY) $(DESTDIR)/usr/local/bin/

uninstall:
	rm -f $(DESTDIR)/usr/local/bin/$(BINARY)
