
EXEC = lix
	
CC = clang

RM = rm -rf

CFLAGS =

LIBS =

INCLUDES = -I ./deps/picohttpparser

SRC = main.c

OBJS = $(PICOHTTPPARSER_DIR)/picohttpparser.o

PICOHTTPPARSER_DIR = deps/picohttpparser

all: build

picohttpparser:
	$(CC) -c -o $(PICOHTTPPARSER_DIR)/picohttpparser.o \
				$(PICOHTTPPARSER_DIR)/picohttpparser.c

build: main.c picohttpparser
	$(CC) $(CFLAGS) $(LIBS) $(INCLUDES) -o $(EXEC) $(OBJS) $(SRC)

run:
	./$(EXEC)

serve: build run

install: build
	cp $(EXEC) $(HOME)/bin/

str_test: str.h str_test.c
	$(CC) -o str_test str_test.c

test: str_test
	./str_test

clean:
	rm $(EXEC)
	rm str_test
	rm $(PICOHTTPPARSER_DIR)/picohttpparser.o

