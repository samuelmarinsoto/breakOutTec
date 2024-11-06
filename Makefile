# C Compiler settings
CC = clang
CFLAGS = -Wall -Wextra -O0 -g
LDFLAGS = -lraylib -lm -ldl -lpthread -lGL -lX11

# Java settings
CSRC_DIR := cliente
JAVASRC_DIR := servidor
JAVALIB_DIR := servidor/lib
BUILD_DIR := build

JAVA_SOURCES := $(wildcard $(JAVASRC_DIR)/*.java)
CLASS_FILES := $(patsubst $(JAVASRC_DIR)/%.java,$(BUILD_DIR)/%.class,$(JAVA_SOURCES))
JAR := $(BUILD_DIR)/servidor.jar

# Targets
TARGET = $(BUILD_DIR)/juego
C_SRC = $(CSRC_DIR)/main.c

# Default target
all: $(TARGET) $(JAR)

$(BUILD_DIR):
	mkdir -p $@
	
# Compile C program
$(TARGET): $(C_SRC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(C_SRC) -o $(TARGET) $(LDFLAGS)

testclient: $(BUILD_DIR)
	$(CC) $(CFLAGS) client.c -o build/client $(LDFLAGS)

startgame: $(TARGET)
	./$(TARGET)

# Download external libraries
java_lib:
	mkdir -p $(JAVALIB_DIR)
	curl -L -o $(JAVALIB_DIR)/json-20240303.jar "https://repo1.maven.org/maven2/org/json/json/20240303/json-20240303.jar"

# Compile Java files
$(CLASS_FILES): $(JAVA_SOURCES) java_lib | $(BUILD_DIR)
	javac -d $(BUILD_DIR) $< -cp $(JAVALIB_DIR)

# Create JAR file
$(JAR): $(CLASS_FILES)
	jar cmf $(JAVASRC_DIR)/manifest.txt $@ -C $(BUILD_DIR) .

startserver: $(JAR)
	java -jar $(JAR)
	
# Clean build directory
clean:
	rm -f $(BUILD_DIR)/*

.PHONY: all clean startserver startgame
