# C Compiler settings
CC = clang
CFLAGS = -Wall -Wextra -O0 -g
LDFLAGS = -ljson-c -lraylib -lm -ldl -lpthread -lGL -lX11 -I./cliente/

# Java settings
CSRC_DIR := cliente
JAVASRC_DIR := servidor
JAVALIB_DIR := servidor/lib
BUILD_DIR := build

JAVASRC := $(wildcard $(JAVASRC_DIR)/*.java)
MANIFEST := $(JAVASRC_DIR)/manifest.txt
JAVA_OBJ := $(patsubst $(JAVASRC_DIR)/%.java,$(BUILD_DIR)/%.class,$(JAVASRC))
JSON_JAR := $(JAVALIB_DIR)/json-20240303.jar
FATJAR_DIR := $(BUILD_DIR)/fatjar
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

juego: $(TARGET)

espectador:
	$(CC) $(CFLAGS) $(CSRC_DIR)/espectador.c -o $(BUILD_DIR)/espectador $(LDFLAGS)

testclients: $(BUILD_DIR)
	$(CC) $(CFLAGS) testgameclient.c -o build/testgameclient $(LDFLAGS)
	$(CC) $(CFLAGS) testspectatorclient.c -o build/testspectatorclient $(LDFLAGS)

startgame: $(TARGET)
	./$(TARGET)

# Download external libraries
$(JSON_JAR):
	mkdir -p $(JAVALIB_DIR)
	curl -L -o $@ "https://repo1.maven.org/maven2/org/json/json/20240303/json-20240303.jar"

# Compile Java files
$(JAVA_OBJ): $(JAVASRC) $(JSON_JAR) | $(BUILD_DIR)
	javac -d $(BUILD_DIR) $(JAVASRC) -cp $(JSON_JAR)

$(FATJAR_DIR): $(JSON_JAR) | $(BUILD_DIR)
	mkdir -p $@
	cd $@ && jar xf ../../$(JSON_JAR)

# Create JAR file
$(JAR): $(JAVA_OBJ) $(MANIFEST) $(FATJAR_DIR)
	jar cmf $(MANIFEST) $@ -C $(BUILD_DIR) . -C $(FATJAR_DIR) .

server: $(JAR)

startserver: $(JAR)
	java -jar $(JAR)
	
# Clean build directory
clean:
	rm -rf $(BUILD_DIR)/*

.PHONY: all clean server juego startserver startgame
