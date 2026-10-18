CC = gcc

CFLAGS = -Wall -Wextra -Wpedantic -g -Iinclude

LDFLAGS = -lws2_32 -lcomdlg32

SRC = src/main.c \
      src/host.c \
      src/client.c \
      src/transfer.c

TARGET = build/main.exe

all: $(TARGET)

$(TARGET): $(SRC)
	mkdir -p build
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)

.PHONY: all clean