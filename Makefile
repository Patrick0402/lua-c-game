CC = gcc
CFLAGS = -Wall -I/usr/include/SDL2 -I/usr/include/lua5.4 -O2
LIBS = -lSDL2 -lSDL2_ttf -llua -lm
OBJS = src/game.o src/lua_utils.o src/logger.o

all: game

game: $(OBJS)
	$(CC) $(OBJS) -o game $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) game
