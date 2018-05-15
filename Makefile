CC=gcc

SRC=src/

EXEC=life
FILES=$(SRC)main.c
INCLUDE= -I include/
LIBS=-lSDL2

all:
	$(CC) -o $(EXEC) $(FILES) $(INCLUDE) $(LIBS)

clean:
	rm $(EXEC)
