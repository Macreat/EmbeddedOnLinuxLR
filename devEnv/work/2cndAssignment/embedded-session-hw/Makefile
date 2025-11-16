# ================================
#  Makefile for Embedded Session HW
#  Closed-loop Light Controller
# ================================

CC      := gcc
CFLAGS  := -Wall -Wextra -std=c11 -O2
LDFLAGS := -lpigpio -lrt -lpthread

# Directory definitions
SRC_DIRS := controller sensor actuators
OBJ_DIR  := build
BIN_DIR  := compiled

# Target binary
TARGET := $(BIN_DIR)/ctl

# Source and object mapping
SRC := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c))
OBJ := $(patsubst %.c,$(OBJ_DIR)/%.o,$(SRC))

.PHONY: all clean run dirs

# Default target: build and run
all: dirs $(TARGET)
	@echo " Running $(TARGET)..."
	@sudo ./$(TARGET)

# Create directories if missing
dirs:
	@mkdir -p $(OBJ_DIR) $(BIN_DIR)

# Link final binary
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo " Build completed: $@"

# Compile each source file into build/
$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@
	@echo " Compiled: $<"

# Run target after build
run: all

# Run and save timestamped log
runlog: all
	@timestamp=$$(date +%Y-%m-%d_%H-%M-%S); \
	mkdir -p logs; \
	echo " Logging to logs/run_$$timestamp.log"; \
	sudo ./$(TARGET) | tee logs/run_$$timestamp.log

