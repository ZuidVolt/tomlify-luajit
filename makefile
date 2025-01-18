# Makefile for macos development builds
CC=cc
LUAJIT_DIR=/opt/homebrew
CFLAGS=-I$(LUAJIT_DIR)/include/luajit-2.1 -Wall -Wextra -Werror -Wunused-result -O3
LDFLAGS=-L$(LUAJIT_DIR)/lib -lluajit-5.1
LUAJIT=$(LUAJIT_DIR)/bin/luajit

# Main target
all: tomlify

# Generate bytecode header from Lua source
tomlify_bc.h: tomlify.lua
	$(LUAJIT) -bg tomlify.lua tomlify_bc.h

# Build the final executable
tomlify: wrapper.c tomlify_bc.h
	$(CC) $(CFLAGS) wrapper.c $(LDFLAGS) -o tomlify

# Clean built files
clean:
	rm -f tomlify tomlify_bc.h
	rm -f *.o

.PHONY: all clean
