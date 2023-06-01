
EXEC = lix
	
CC = clang
RM = rm -rf

CFLAGS =
LIBS =
INCLUDES = "deps/picohttpparser"

all: build

build: main.c
	$(CC) $(CFLAGS) $(LIBS) $(INCLUDES) -o $(EXEC) main.c

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
	rm -rf $(EXEC)
