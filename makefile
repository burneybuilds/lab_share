CC = gcc

CFLAGS = -Wall -Wextra -Wpedantic -Wshadow -Wformat=2 -Wconversion -Wsign-conversion -Wuninitialized -Wmaybe-uninitialized -Wnull-dereference -Wstrict-prototypes -Wmissing-prototypes -Wold-style-definition -Werror=implicit-function-declaration -g -O2 -Iinclude

LDFLAGS = -lws2_32 -lcomdlg32

SRC = src/main.c src/host.c src/client.c src/transfer.c

TARGET = build/main.exe

all: $(TARGET)

$(TARGET): $(SRC)
	mkdir -p build
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)

.PHONY: all clean