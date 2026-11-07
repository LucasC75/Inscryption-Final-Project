#ifndef

typedef struct card {
        int health;
        int attack;
        char sigil;
        int requirement;

}Card;

typedef struct deck {
        Card card;
        struct deck *next;

}Deck;



#endif
