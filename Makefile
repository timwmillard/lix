
EXEC = lix
	
CC = clang
RM = rm -rf

CFLAGS =
LIBS =
INCLUDES =

all: build

build: main.c
	$(CC) $(CFLAGS) $(LIBS) $(INCLUDES) -o $(EXEC) main.c

run:
	./$(EXEC)

serve: build run

install: build
	cp $(EXEC) $(HOME)/bin/

clean:
	rm -rf $(EXEC)
