
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


clean:
	rm -rf $(EXEC)
