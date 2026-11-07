#ifndef

typedef struct card {
        int health;
        int attack;
        char sigil;
        int requirement;

}MCard;

typedef struct deck {
        Card card;
        struct deck *next;

}Deck;

void addHand(int d);

Card drawMain();

Card drawS();

void makeCard(Card new);


#endif
