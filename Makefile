# -*- Makefile -*-

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
LDFLAGS = -lraylib

# Directories
SRC_DIR = src
OBJ_DIR = build
TARGET = game

# Find all .c files recursively
SRC = $(shell find $(SRC_DIR) -name '*.c')

# Generate corresponding .o file paths in build/
OBJ = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))

# Default target
all: $(TARGET)

# Link object files into final binary
$(TARGET): $(OBJ)
	@mkdir -p $(dir $@)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

# Compile .c to .o while preserving directory structure
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean everything
clean:
	rm -rf $(OBJ_DIR) game

.PHONY: all clean
