APP_ENVIRONMENT ?= DEVELOPMENT

CFLAGS_COMMON := -std=c23 -Wall -Wextra -Isrc -lcrypto
CFLAGS_DEVELOPMENT := -Og -g3
CFLAGS_PRODUCTION := -O2

BIN := bin/t3c
SRC := $(shell find src -name '*.c')

all:
	mkdir -p bin
	gcc $(CFLAGS_COMMON) $(CFLAGS_$(APP_ENVIRONMENT)) -o $(BIN) $(SRC)

run: all
	./$(BIN)

clean:
	rm -rf bin

.PHONY: all run clean
