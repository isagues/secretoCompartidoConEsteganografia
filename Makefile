
SOURCES=$(wildcard *.c)
OBJECTS=$(SOURCES:.c=.o)
BIN=anashe.exe

CFLAGS=-pedantic -std=c11 -Wall -Wextra -ggdb

all: $(OBJECTS)
	$(CC) $^ -o $(BIN)

clean:
	$(RM) $(OBJECTS) $(BIN)


.PHONY: all clean
