CC          =gcc
CFLAGS      = -O3 -Wall -msse2

INCLUDE     = -I include/
LIBS        = -lSDL2 -lSDL2_ttf
EXEC        = life

SRC         = src/
OBJ         = obj/
C_FILES     = $(wildcard $(SRC)*.c)
OBJ_FILES   = $(addprefix $(OBJ), $(notdir $(C_FILES:.c=.o))) 

$(OBJ)%.o: $(SRC)%.c
	@echo CC $@
	@ $(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

$(EXEC): $(OBJ_FILES)	
	@echo Linking..
	@ $(CC) -o $(EXEC) $(OBJ_FILES) $(LIBS)
	@echo OK

all: $(EXEC) 

clean:
	@echo Cleaning...
	@ rm $(EXEC) $(OBJ_FILES)
	@echo OK
