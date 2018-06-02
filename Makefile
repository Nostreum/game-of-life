CC          = gcc
CFLAGS      = -O2 -Wall -msse2 -mssse3 -fopenmp

INCLUDE     = -I include/
LIBS        = -lSDL2 -lSDL2_ttf -fopenmp
EXEC        = life
DEFINE      = 

SRC         = src/
OBJ         = obj/
C_FILES     = $(wildcard $(SRC)*.c)
OBJ_FILES   = $(addprefix $(OBJ), $(notdir $(C_FILES:.c=.o))) 

$(OBJ)%.o: $(SRC)%.c
	@echo CC $@
	@ $(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

$(EXEC): $(OBJ_FILES)	
	@echo $(EXEC) Linking..
	@ $(CC) -o $(EXEC) $(OBJ_FILES) $(LIBS)
	@echo OK

all: $(EXEC) 

clean:
	@echo Cleaning...
	@ rm $(EXEC) $(OBJ_FILES) *.txt
	@echo OK
