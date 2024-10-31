# C Compiler settings
CC = clang
CFLAGS = -Wall -Wextra -O0 -g
LDFLAGS = -lraylib -lm -ldl -lpthread -lGL -lX11

# Java settings
CSRC_DIR := cliente
JAVASRC_DIR := servidor
BUILD_DIR := build
PACKAGE := com.server
JAVA_SOURCES := $(wildcard $(JAVASRC_DIR)/$(PACKAGE)/*.java)
CLASS_FILES := $(patsubst $(JAVASRC_DIR)/$(PACKAGE)/%.java,$(BUILD_DIR)/$(PACKAGE)/%.class,$(JAVA_SOURCES))
JAR := $(BUILD_DIR)/servidor.jar

# Targets
TARGET = $(BUILD_DIR)/juego
C_SRC = $(CSRC_DIR)/main.c

# Default target
all: $(TARGET) $(JAR)

$(BUILD_DIR):
	mkdir -p $@

$(BUILD_DIR)/$(PACKAGE): $(BUILD_DIR)
	mkdir -p $@
	
# Compile C program
$(TARGET): $(C_SRC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(C_SRC) -o $(TARGET) $(LDFLAGS)

# Compile Java files
$(BUILD_DIR)/%.class: $(SRC_DIR)/$(PACKAGE)/%.java | $(BUILD_DIR)/$(PACKAGE)
	javac -d $(BUILD_DIR) $<

# Create JAR file
$(JAR_FILE): $(CLASS_FILES)
	jar cf $@ -C $(BUILD_DIR) .

# Clean build directory
clean:
	rm -f $(BUILD_DIR)/*

.PHONY: all clean
