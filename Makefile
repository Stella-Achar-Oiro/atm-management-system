# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra -I./src
LDFLAGS = -lsqlite3 -lcrypto

# Directory structure
SRC_DIR = src
BUILD_DIR = build

# Source files and objects
SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Output binary
TARGET = atm

# Default target
all: setup $(TARGET)

# Linking the program
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

# Compiling source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(SRC_DIR)/header.h
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create necessary directories and initialize database
setup:
	@mkdir -p data $(BUILD_DIR)
	@touch data/users.txt data/records.txt

# Install dependencies (Ubuntu/Debian)
install-deps:
	@echo "Installing required packages..."
	@sudo apt-get update
	@sudo apt-get install -y libsqlite3-dev libssl-dev

# Clean build files
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

# Clean all generated files including data
cleanall: clean
	rm -f data/users.txt data/records.txt data/atm.db
	rm -rf data

# Help target
help:
	@echo "Available targets:"
	@echo "  all         : Build the ATM program (default)"
	@echo "  setup       : Create necessary directories and files"
	@echo "  install-deps: Install required system libraries"
	@echo "  clean       : Remove object files and executable"
	@echo "  cleanall    : Remove all generated files including data"
	@echo "  help        : Show this help message"

.PHONY: all setup clean cleanall help install-deps