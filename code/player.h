#ifndef PLAYER_H
#define PLAYER_H

#include "card.h"

#define LANES          4
#define MAX_HAND       10
#define DECK_SIZE      12
#define MAX_REROLLS    3
#define SQUIRREL_COUNT 10

typedef struct {
    Node *deck;               /* main deck (stack)     */
    Node *squirrels;          /* squirrel deck (stack) */
    MCard hand[MAX_HAND];
    int   handSize;
    MCard board[LANES];       /* empty lane = health 0 */
} Player;

int  readInt(const char *prompt, int min, int max);

MCard rollCard(void);
void  buildDeck(Player *p);
void  initPlayer(Player *p);
void freePlayer(Player *p);

void  addToHand(Player *p, MCard c);
MCard removeFromHand(Player *p, int index);
void  displayHand(const Player *p);

void drawPhase(Player *p);
int  selectCard(const Player *p);
void playCard(Player *p);

#endif
