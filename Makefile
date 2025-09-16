PROGRAM_NAME := cerver
CONF_DIR := /etc
PREFIX := /usr/local
WEBROOT := /var/www

CC := gcc

CFLAGS := -Wall -Wextra -g

SRC_DIR := src
HTML_DIR := html
BIN_DIR := bin

OUTPUT := $(BIN_DIR)/$(PROGRAM_NAME)

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
	cd $(BIN_DIR) && ./$(PROGRAM_NAME)

install: all
	install -d $(DESTDIR)$(PREFIX)/$(BIN_DIR)
	install -d $(DESTDIR)$(CONF_DIR)/$(PROGRAM_NAME)
	install -d $(DESTDIR)$(WEBROOT)/$(PROGRAM_NAME)

	install -m 755 $(OUTPUT) $(DESTDIR)$(PREFIX)/$(BIN_DIR)

	install -m 644 conf/* $(DESTDIR)$(CONF_DIR)/$(PROGRAM_NAME)

	cp -r $(HTML_DIR) $(DESTDIR)$(WEBROOT)/$(PROGRAM_NAME)

	@echo "Setting up permissions (may require sudo ...)"
	-chown -R www-data:www-data $(DESTDIR)$(WEBROOT)/$(PROGRAM_NAME) 2>/dev/null || \
		chown -R $(USER):$(USER) $(DESTDIR)$(WEBROOT)/$(PROGRAM_NAME)
	chmod -R 755 $(DESTDIR)$(WEBROOT)/$(PROGRAM_NAME)

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/$(BIN_DIR)/$(PROGRAM_NAME)
	rm -rf $(DESTDIR)$(CONF_DIR)/$(PROGRAM_NAME)
	rm -rf $(DESTDIR)$(WEBROOT)/$(PROGRAM_NAME)

clean:
	rm -f $(OBJS) $(OUTPUT)
	rm -rf $(BIN_DIR)/html

clean-html:
	rm -rf $(BIN_DIR)/html

rebuild-html: clean-html copy-html

.PHONY: all clean copy-html dev install uninstall clean-html rebuild-html