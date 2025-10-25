# Makefile
CC = gcc
CFLAGS = -Wall -O2 `sdl2-config --cflags` -I/usr/include/lua5.4
LDFLAGS = `sdl2-config --libs` -llua -lm
SOURCES = game.c lua_utils.c logger.c
OBJECTS = $(SOURCES:.c=.o)
TARGET = game

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: all clean
