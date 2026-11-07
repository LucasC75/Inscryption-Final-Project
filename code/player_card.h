#ifndef
#include <stdio.h>
#include <stdlib.h>

#define MAX_CARDS 31


typedef struct card {
        char name[50];
        int health;
        int attack;
        int sigil;
        int requirement;

}Mcard;

typedef struct deck {
        Mcard card;
        struct deck *next;

}Deck;

typedef struct hand {
        Mcard card;
        struct hand *next;
}Hand;



Card cardList[MAX_CARDS];

void addHand(int d);

Card drawMain();

Card drawS();

void makeCard(Card new);

void displayHand();
void displayBoard();
void displayScale();
void displayAttributes();



//player stuff

typedef struct player {
        struct Deck;
        struct Hand;
}Player;

#endif
