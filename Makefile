CC=gcc-12
SOURCES=$(wildcard sources/*.c)
HEADERS=headers
OBJECTS=$(SOURCES:.c=.o)

EXECUTABLE=./test

LIB_NAME=RegExpLib.h

all: build
	$(EXECUTABLE)

build: $(OBJECTS)
	$(CC) $(CFLAGS) -o $(EXECUTABLE) $^

lib: headers/RegExp.h sources/RegExp.c sources/print.c
	cat headers/RegExp.h > $(LIB_NAME)
	echo "#if defined(REGEXP_LIB_IMPLEMENTATION)" >> $(LIB_NAME)
	tail -n +2 sources/RegExp.c >> $(LIB_NAME)
	tail -n +3 sources/print.c >> $(LIB_NAME)
	echo "#endif // REGEXP_LIB_IMPLEMENTATION" >> $(LIB_NAME)

test: lib
	gcc -o test test.c
	./test
	rm -f RegExpLib.h test

.c.o:
	$(CC) -c -Wall -Wextra -std=c99 -pedantic -I$(HEADERS) $(CFLAGS) -o $@ $<

clean:
	rm -f $(OBJECTS) $(EXECUTABLE) $(LIB_NAME)