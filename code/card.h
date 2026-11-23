/* card.h - the MCard structure and the card functions in card.c */
#ifndef CARD_H
#define CARD_H

#define _CRT_SECURE_NO_WARNINGS   /* lets Visual Studio use fopen/sscanf */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NAME_LEN  32   /* longest card name + 1 */
#define MAX_CARDS 50   /* most cards card_definitions.txt can hold */

/* Sigil numbers used in card_definitions.txt */
enum {
    NO_SIGIL,         /* 0 */
    FLEDGLING,        /* 1 */
    TOUCH_OF_DEATH,   /* 2 */
    RABBIT_HOLE,      /* 3 */
    BEES_WITHIN,      /* 4 */
    UNKILLABLE,       /* 5 */
    AIRBORNE,         /* 6 */
    MANY_LIVES,       /* 7 */
    WORTHY_SACRIFICE, /* 8 */
    MIGHTY_LEAP,      /* 9 */
    WATERBORNE,       /* 10 */
    MYSTERY,          /* 11 */
    SIGIL_COUNT
};

typedef struct {
    char name[NAME_LEN];
    int health;
    int attack;
    int sigil;
    int requirement;   /* blood needed to play the card */
} MCard;

/* Every card loaded from card_definitions.txt */
extern MCard cardList[MAX_CARDS];
extern int cardCount;

/* Creation and validation */
int loadCards(const char *filename);
int validateCard(const MCard *c);
const MCard *findCard(const char *name);
MCard randomCard(int rarityBonus);
int isEmptySlot(const MCard *c);

/* Display */
const char *sigilName(int sigil);
void printCard(const MCard *c);
void printCardRow(const MCard row[], int count);
void printAllCards(void);

/* Type (sigil) bonuses */
int bloodValue(const MCard *c);
int attacksDirectly(const MCard *attacker, const MCard *defender);
int damageDealt(const MCard *attacker, const MCard *defender);
int growUp(MCard *c);

#endif
