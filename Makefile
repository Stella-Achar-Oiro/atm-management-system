# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra -I./src
LDFLAGS = -lsqlite3 -lcrypt

# Directory structure
SRC_DIR = src
BUILD_DIR = build
DATA_DIR = data

# Source files and objects
SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Output binary
TARGET = atm
MIGRATION = migration

# Default target
all: setup $(TARGET) $(MIGRATION)

# Linking the main program
$(TARGET): $(filter-out $(BUILD_DIR)/migration.o, $(OBJECTS))
	$(CC) $^ -o $@ $(LDFLAGS)

# Linking the migration utility
$(MIGRATION): $(BUILD_DIR)/migration.o $(BUILD_DIR)/database.o $(BUILD_DIR)/encryption.o
	$(CC) $^ -o $@ $(LDFLAGS)


# Compiling source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(SRC_DIR)/header.h
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create necessary directories
setup:
	@mkdir -p $(DATA_DIR) $(BUILD_DIR)

# Install dependencies (Ubuntu/Debian)
install-deps:
	@echo "Installing required packages..."
	@sudo apt-get update
	@sudo apt-get install -y libsqlite3-dev libcrypt-dev

# Clean build files
clean:
	rm -rf $(BUILD_DIR) $(TARGET) $(MIGRATION)

# Clean all generated files including database
cleanall: clean
	rm -f $(DATA_DIR)/atm.db
	rm -rf $(DATA_DIR)

# Help target
help:
	@echo "ATM Management System - Build Targets"
	@echo "make              - Build the ATM system"
	@echo "make install-deps - Install required dependencies"
	@echo "make clean        - Remove build files"
	@echo "make cleanall     - Remove all generated files"
	@echo "make help         - Display this help message"

.PHONY: all setup clean cleanall help install-deps