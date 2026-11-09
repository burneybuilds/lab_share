
CC = gcc

CFLAGS = -Wall -Wextra -Wpedantic -Wshadow -Wformat=2 -Wconversion -Wsign-conversion -Wuninitialized -Wmaybe-uninitialized -Wnull-dereference -Wstrict-prototypes -Wmissing-prototypes -Wold-style-definition -Werror=implicit-function-declaration -g -O2 -Iinclude

LDFLAGS = -lws2_32 -lcomdlg32

# Directories
BUILD_DIR = build
SRC_DIR = src
TEST_DIR = tests

# ================= MAIN APPLICATION =================

APP_SRC = $(SRC_DIR)/main.c \
          $(SRC_DIR)/host.c \
          $(SRC_DIR)/client.c \
          $(SRC_DIR)/transfer.c \
          $(SRC_DIR)/utils.c \
          $(SRC_DIR)/validate.c

APP_TARGET = $(BUILD_DIR)/main

# ================= TEST EXECUTABLE =================

TEST_SRC = $(TEST_DIR)/test_host.c \
           $(TEST_DIR)/test_client.c \
           $(SRC_DIR)/host.c \
           $(SRC_DIR)/client.c \
           $(SRC_DIR)/transfer.c \
           $(SRC_DIR)/utils.c

TEST_TARGET = $(BUILD_DIR)/tests.exe

# ================= DEFAULT TARGET =================

all: app tests

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# ================= BUILD APPLICATION =================

app: $(APP_TARGET)

$(APP_TARGET): $(APP_SRC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(APP_SRC) -o $@ $(LDFLAGS)

# ================= BUILD TESTS =================

tests: $(TEST_TARGET)

$(TEST_TARGET): $(TEST_SRC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(TEST_SRC) -o $@ $(LDFLAGS)

# ================= RUN TESTS =================

test: tests
	./$(TEST_TARGET)

# ================= RUN APPLICATION =================

run: app
	./$(APP_TARGET)

# ================= CLEAN =================

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all app tests test run clean
