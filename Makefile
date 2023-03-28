CC=gcc-12
SOURCES=$(wildcard sources/*.c)
HEADERS=headers
OBJECTS=$(SOURCES:.c=.o)

EXECUTABLE=./test

all: build
	$(EXECUTABLE)

build: $(OBJECTS)
	$(CC) -o $(EXECUTABLE) $^

.c.o:
	$(CC) -c -Wall -Wextra -std=c99 -pedantic -I$(HEADERS) $(CFLAGS) -o $@ $<

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)