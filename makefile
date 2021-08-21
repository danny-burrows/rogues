CC=gcc
IDIR =./include
SDIR = ./src

ifeq ($(config), debug)
    CFLAGS = -I $(IDIR) -g -D DCONFIGSET
else
	CFLAGS = -I $(IDIR) -O3
endif

ODIR=obj
LDIR =./lib

LIBS=-lm -lpthread

_DEPS = game.h map.h saved_games.h terminal.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o map.o saved_games.o 
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

game: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o game
