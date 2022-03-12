# Really Q&D makefile...

SRC := $(wildcard src/*.c)
CFLAGS := -ldl
CC := gcc

all:
	$(CC) $(SRC) $(CFLAGS) -o durian

.PHONY: clean all
clean:
	rm -f durian
