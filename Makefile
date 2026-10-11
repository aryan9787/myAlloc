CC = gcc

CFLAGS = -Wall -Wextra -std=c11 -g -D_DEFAULT_SOURCE \
         -Iinclude

SRC = src/allocator.c \
      src/heap.c \
      src/block.c \
      src/sync.c

TEST_SRC = tests/test_allocator.c \
           tests/test_runner.c

OBJ = $(SRC:.c=.o)
TEST_OBJ = $(TEST_SRC:.c=.o)

TARGET = malloc_tests

all: $(TARGET)

$(TARGET): $(OBJ) $(TEST_OBJ)
	$(CC) $(CFLAGS) $(OBJ) $(TEST_OBJ) -o $(TARGET)

clean:
	rm -f $(OBJ) $(TEST_OBJ) $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run