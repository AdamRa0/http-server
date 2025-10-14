PROGRAM_NAME := cerver
CONF_DIR := /etc
PREFIX := /usr/local
WEBROOT := /var/www
LOGROOT := /var/log

CC := gcc

CFLAGS := -Wall -Wextra -g

SRC_DIR := src
HTML_DIR := html
SERVER_CONF_DIR := conf
BIN_DIR := bin

OUTPUT := $(BIN_DIR)/$(PROGRAM_NAME)

SRCS = $(shell find $(SRC_DIR) -name '*.c')
OBJS = $(SRCS:.c=.o)

all: $(OUTPUT)

$(OUTPUT): $(OBJS) | $(BIN_DIR)
	$(CC) -o $@ $^ -lmagic

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

dev: all
	install -d $(DESTDIR)$(CONF_DIR)/$(PROGRAM_NAME)/conf
	install -d $(DESTDIR)$(WEBROOT)/$(PROGRAM_NAME)
	install -d $(DESTDIR)$(LOGROOT)/$(PROGRAM_NAME)

	install -m 644 conf/* $(DESTDIR)$(CONF_DIR)/$(PROGRAM_NAME)/conf
	install -m 644 logs/* $(DESTDIR)$(LOGROOT)/$(PROGRAM_NAME)

	cp -r $(HTML_DIR) $(DESTDIR)$(WEBROOT)/$(PROGRAM_NAME)

	@echo "Starting server from $(BIN_DIR)..."
	cd $(BIN_DIR) && ./$(PROGRAM_NAME)

install: all
	install -d $(DESTDIR)$(PREFIX)/$(BIN_DIR)
	install -d $(DESTDIR)$(CONF_DIR)/$(PROGRAM_NAME)/conf
	install -d $(DESTDIR)$(WEBROOT)/$(PROGRAM_NAME)
	install -d $(DESTDIR)$(LOGROOT)/$(PROGRAM_NAME)

	install -m 755 $(OUTPUT) $(DESTDIR)$(PREFIX)/$(BIN_DIR)

	install -m 644 conf/* $(DESTDIR)$(CONF_DIR)/$(PROGRAM_NAME)/conf
	install -m 644 logs/* $(DESTDIR)$(LOGROOT)/$(PROGRAM_NAME)

	cp -r $(HTML_DIR) $(DESTDIR)$(WEBROOT)/$(PROGRAM_NAME)

	@echo "Setting up permissions (may require sudo ...)"
	-chown -R www-data:www-data $(DESTDIR)$(WEBROOT)/$(PROGRAM_NAME) 2>/dev/null || \
		chown -R $(USER):$(USER) $(DESTDIR)$(WEBROOT)/$(PROGRAM_NAME)
	chmod -R 755 $(DESTDIR)$(WEBROOT)/$(PROGRAM_NAME)

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/$(BIN_DIR)/$(PROGRAM_NAME)
	rm -rf $(DESTDIR)$(CONF_DIR)/$(PROGRAM_NAME)
	rm -rf $(DESTDIR)$(WEBROOT)/$(PROGRAM_NAME)
	rm -rf $(DESTDIR)$(LOGROOT)/$(PROGRAM_NAME)

clean:
	rm -f $(OBJS) $(OUTPUT)

	rm -rf $(DESTDIR)$(CONF_DIR)/$(PROGRAM_NAME)
	rm -rf $(DESTDIR)$(WEBROOT)/$(PROGRAM_NAME)
	rm -rf $(DESTDIR)$(LOGROOT)/$(PROGRAM_NAME)


.PHONY: all clean dev install uninstall