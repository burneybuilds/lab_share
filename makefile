
CC = gcc

CFLAGS = -Wall -Wextra -Wpedantic -Wshadow -Wformat=2 -Wconversion -Wsign-conversion -Wuninitialized -Wmaybe-uninitialized -Wnull-dereference -Wstrict-prototypes -Wmissing-prototypes -Wold-style-definition -Werror=implicit-function-declaration -g -O2 -Iinclude

LDFLAGS = -lws2_32 -lcomdlg32

# Directories
BUILD_DIR = build
SRC_DIR = src
TEST_DIR = tests

# ================= MAIN APPLICATION =================

APP_SRC = $(SRC_DIR)/main.c $(SRC_DIR)/host.c $(SRC_DIR)/client.c $(SRC_DIR)/transfer.c
APP_TARGET = $(BUILD_DIR)/main.exe

# ================= TEST EXECUTABLES =================

CLIENT_TEST = $(BUILD_DIR)/test_client.exe
HOST_TEST = $(BUILD_DIR)/test_host.exe
TRANSFER_TEST = $(BUILD_DIR)/test_transfer.exe

TEST_TARGETS = $(CLIENT_TEST) $(HOST_TEST) $(TRANSFER_TEST)

# ================= DEFAULT TARGET =================

all: app tests

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# ================= BUILD MAIN APPLICATION =================

app: $(APP_TARGET)

$(APP_TARGET): $(APP_SRC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(APP_SRC) -o $@ $(LDFLAGS)

# ================= BUILD TESTS =================

$(CLIENT_TEST): $(TEST_DIR)/test_client.c $(SRC_DIR)/client.c $(SRC_DIR)/transfer.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(HOST_TEST): $(TEST_DIR)/test_host.c $(SRC_DIR)/host.c $(SRC_DIR)/transfer.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(TRANSFER_TEST): $(TEST_DIR)/test_transfer.c $(SRC_DIR)/transfer.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

tests: $(TEST_TARGETS)

# ================= RUN TESTS =================

test: tests
	./$(CLIENT_TEST)
	./$(HOST_TEST)
	./$(TRANSFER_TEST)

# ================= RUN APPLICATION =================

run: app
	./$(APP_TARGET)

# ================= CLEAN =================

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all app tests test run clean
