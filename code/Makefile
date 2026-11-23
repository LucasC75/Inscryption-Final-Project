CC = gcc
CFLAGS = -Wall -Wextra -std=c99
SRC = main.c card.c player.c battle.c

inscryption: $(SRC) card.h player.h battle.h
	$(CC) $(CFLAGS) -o inscryption $(SRC)

clean:
	rm -f inscryption
