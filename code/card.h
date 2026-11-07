#ifndef

typedef struct card {
        int health;
        int attack;
        int requirement

}Card;

typedef struct deck {
        Card card;
        struct deck *next;

}Deck;



#endif
