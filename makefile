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

_DEPS = game.h map.h saved_games.h terminal.h ui.h player.h camera.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ  = main.o map.o saved_games.o terminal.o ui.o player.o camera.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

rogues: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: tools clean

tools:
	$(CC) -o ./tools/makemap ./tools/makemap.c -g

clean:
	rm -f $(ODIR)/*.o ./tools/makemap rogues 
