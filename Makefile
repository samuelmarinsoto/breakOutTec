CC = clang
CFLAGS = -Wall -Wextra -O2
LDFLAGS = -lraylib -lm -ldl -lpthread -lGL -lX11

TARGET = game
SRC = main.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)
