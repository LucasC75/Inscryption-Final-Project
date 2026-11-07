#ifndef

typedef struct card {
        int health;
        int attack;
        int sigil;
        int requirement;

}MCard;

typedef struct deck {
        MCard card;
        struct deck *next;

}Deck;

typedef struct hand {
        Card card;
        struct hand *next;
}Hand;



Card cardList[];

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
