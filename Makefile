CC:= gcc
CFLAGS:= -Wall -Wextra -Wpedantic -ggdb -O3 -std=c11
LIBS= -lraylib -lm

all: Snake_Game
	./Snake_Game

Snake_Game: Snake_Game.c
	$(CC) $(CFLAGS) -o Snake_Game Snake_Game.c $(LIBS)

clean:
	rm -fv Snake_Game

