# ===================================================
#  Makefile for Embedded Linux System Monitor Project
# ===================================================

CC      := gcc
CFLAGS  := -Wall -Wextra -std=c11 -O2
LDFLAGS := -lncurses

SRC_DIR := src
OBJ_DIR := build
BIN_DIR := results

TARGET  := $(BIN_DIR)/sysmon
SRC     := $(wildcard $(SRC_DIR)/*.c)
OBJ     := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC))

.PHONY: all clean run runlog dirs

# Build + run automatically
all: dirs $(TARGET)
	@echo " Running $(TARGET)..."
	@./$(TARGET)

dirs:
	mkdir -p $(OBJ_DIR) $(BIN_DIR)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo " Build complete: $@"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

run: all

runlog: all
	@timestamp=$$(date +%Y-%m-%d_%H-%M-%S); \
	echo " Saving output to results/output_$$timestamp.log"; \
	./$(TARGET) | tee results/output_$$timestamp.log

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
	@echo " Cleaned build directories"
