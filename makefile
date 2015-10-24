.PHONY: all clean install

ifeq ($(PREFIX_DIR),)
PREFIX_DIR := $(HOME)/.local
endif

CFLAGS := -Ofast -fno-stack-protector -fomit-frame-pointer

all: bin_to_hex

install: $(PREFIX_DIR)/bin/bin_to_hex


$(PREFIX_DIR)/bin/bin_to_hex: bin_to_hex | $(PREFIX_DIR)
	install $< $@

bin_to_hex: bin_to_hex.c
	$(CC) -o$@ $(CFLAGS) $<

