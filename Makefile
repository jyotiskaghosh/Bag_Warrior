# -*- Makefile -*-

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
LDFLAGS = -lraylib

# Emscripten settings
EMCC = emcc
EMCFLAGS = -I$(HOME)/raylib/src -Iinclude -DPLATFORM_WEB
EMLDFLAGS = -s USE_GLFW=3 -s ASYNCIFY -s FULL_ES3=1 -s ERROR_ON_UNDEFINED_SYMBOLS=0
RAYLIB = $(HOME)/raylib/src/libraylib.a

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
	rm -rf $(OBJ_DIR) game game.html game.js game.wasm

# WebAssembly object files
WASM_OBJ_DIR = build/wasm
WASM_OBJ = $(patsubst $(SRC_DIR)/%.c, $(WASM_OBJ_DIR)/%.o, $(SRC))

# Compile .c to .o for WebAssembly
$(WASM_OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(EMCC) $(EMCFLAGS) -c $< -o $@

# WebAssembly build target
wasm: $(WASM_OBJ)
	$(EMCC) $(WASM_OBJ) $(RAYLIB) $(EMLDFLAGS) -o game.html

.PHONY: all clean wasm
