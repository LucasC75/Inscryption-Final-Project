#ifndef CARD_H
#define CARD_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CARDS 40
#define NAME_LEN  32

/* Sigil numbers used in card_definitions.txt */
enum {
    SIG_NONE,             /* 0  no sigil                                  */
    SIG_FLEDGLING,        /* 1  grows +1/+1 at the start of next turn     */
    SIG_TOUCH_OF_DEATH,   /* 2  kills any card it damages                 */
    SIG_RABBIT_HOLE,      /* 3  when played, adds a Squirrel to your hand */
    SIG_BEES_WITHIN,      /* 4  when hit, adds a Bee to your hand         */
    SIG_UNKILLABLE,       /* 5  returns to your hand when it dies         */
    SIG_AIRBORNE,         /* 6  attacks the scale directly                */
    SIG_MANY_LIVES,       /* 7  survives being sacrificed                 */
    SIG_WORTHY_SACRIFICE, /* 8  counts as 3 blood                         */
    SIG_MIGHTY_LEAP,      /* 9  blocks Airborne attackers                 */
    SIG_WATERBORNE,       /* 10 attacks against it go to the scale        */
    SIG_MYSTERY,          /* 11 no effect in this version                 */
    SIG_COUNT
};

typedef struct {
    char name[NAME_LEN];
    int  health;
    int  attack;
    int  sigil;
    int  cost;            /* blood needed to play the card */
} MCard;

/* Linked-list node shared by the deck stack and the card queue (battle.c) */
typedef struct Node {
    MCard card;
    struct Node *next;
} Node;

typedef struct {
    Node *front;
    Node *rear;
} Queue;

/* All cards read from card_definitions.txt */
extern MCard cardList[MAX_CARDS];
extern int   cardCount;

/* Creation */
int   loadCards(const char *filename);
MCard findCard(const char *name);
MCard randomCard(int maxCost);

/* Validation */
int isValidCard(MCard c);
int isEmptySlot(MCard c);

/* Display */
const char *sigilName(int sigil);
void printCard(MCard c);
void printCardList(void);

/* Sigil (type-bonus) calculations */
int bloodValue(MCard c);
int growCard(MCard *c);
int attacksDirectly(MCard attacker, MCard defender);
int damageCard(MCard attacker, MCard *defender);

#endif
