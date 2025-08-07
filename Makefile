CC := gcc

CFLAGS := -Wall -Wextra -g

SRC_DIR := src
HTML_DIR := html
BIN_DIR := bin

OUTPUT := $(BIN_DIR)/cerver

SRCS = $(shell find $(SRC_DIR) -name '*.c')
OBJS = $(SRCS:.c=.o)

all: $(OUTPUT) copy-html

$(OUTPUT): $(OBJS) | $(BIN_DIR)
	$(CC) -o $@ $^

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

copy-html: $(OUTPUT)
	cp -r $(HTML_DIR) $(BIN_DIR)/
	@echo "HTML files copied to $(BIN_DIR)/html/"

dev: all
	@echo "Starting server from $(BIN_DIR)..."
	cd $(BIN_DIR) && ./cerver

install: all
	mkdir -p /usr/local/bin
	mkdir -p /usr/local/share/cerver
	cp $(OUTPUT) /usr/local/bin/
	cp -r $(HTML_DIR)/* /usr/local/share/cerver/
	@echo "Installed to /usr/local/"

dist: all
	mkdir -p dist/cerver
	cp $(OUTPUT) dist/cerver/
	cp -r $(HTML_DIR) dist/cerver/
	tar -czf cerver.tar.gz -C dist cerver
	@echo "Distribution package created: cerver.tar.gz"

clean:
	rm -f $(OBJS) $(OUTPUT)
	rm -rf $(BIN_DIR)/html
	rm -rf dist/ *.tar.gz

clean-html:
	rm -rf $(BIN_DIR)/html

rebuild-html: clean-html copy-html

.PHONY: all clean copy-html dev install dist clean-html rebuild-html