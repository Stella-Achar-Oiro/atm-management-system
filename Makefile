# Compiler and flags
CC := gcc
CFLAGS := -Wall -Wextra -O2 -march=native
CFLAGS_DEBUG := $(CFLAGS) -g3 -DDEBUG
LDFLAGS := -lsqlite3 -lcrypt

# Directories
SRC_DIR := src
OBJ_DIR := obj
BUILD_DIR := build

# Target executable
TARGET := atm

# Source files
SRC_FILES := $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES := $(SRC_FILES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
DEP_FILES := $(OBJ_FILES:.o=.d)

# Colors for terminal output
CYAN := \033[36m
RESET := \033[0m

# Default target
all: release

# Release build
release: CFLAGS += -O2
release: directories $(TARGET)

# Debug build
debug: CFLAGS := $(CFLAGS_DEBUG)
debug: directories $(TARGET)

# Create necessary directories
directories:
	@mkdir -p $(OBJ_DIR)
	@mkdir -p data

# Compile source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "$(CYAN)Compiling $<$(RESET)"
	@$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

# Link the executable
$(TARGET): $(OBJ_FILES)
	@echo "$(CYAN)Linking $@$(RESET)"
	@$(CC) $^ $(LDFLAGS) -o $@
	@echo "$(CYAN)Build complete!$(RESET)"

# Run the program
run: release
	@echo "$(CYAN)Running ATM System...$(RESET)"
	@./$(TARGET)

# Clean build files
clean:
	@echo "$(CYAN)Cleaning build files...$(RESET)"
	@rm -rf $(OBJ_DIR) $(BUILD_DIR) $(TARGET) bin
	@echo "$(CYAN)Clean complete!$(RESET)"

# Include dependency files
-include $(DEP_FILES)

# Development help commands
help:
	@echo "$(CYAN)Available commands:$(RESET)"
	@echo " make - Build release version"
	@echo " make debug - Build debug version"
	@echo " make run - Build and run the program"
	@echo " make clean - Remove build files"
	@echo " make help - Show this help message"

.PHONY: all release debug clean run help directories