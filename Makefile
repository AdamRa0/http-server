CC := gcc

CFLAGS := -Wall -Wextra -g

SRC_DIR := src

BIN_DIR := bin

OUTPUT := $(BIN_DIR)/server

SRCS = $(shell find $(SRC_DIR) -name '*.c')

OBJS = $(SRCS:.c=.o)

all: $(OUTPUT)

$(OUTPUT): $(OBJS)
	$(CC) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(OUTPUT)

.PHONY: all clean