CC=gcc -O3

INCLUDE     = -I include/
LIBS        = -lSDL2
EXEC        = life

SRC=src/
C_FILES     = $(wildcard $(SRC)*.c)
OBJ_FILES   = $(C_FILES:.c=.o) 

%.o: %.c
	$(CC) $(INCLUDE) -c $< -o $@

all: $(OBJ_FILES)
	$(CC) $(OBJ_FILES) -o $(EXEC) $(LIBS)

clean:
	rm $(EXEC) $(OBJ_FILES)
