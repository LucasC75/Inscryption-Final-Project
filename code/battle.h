#ifndef BATTLE_H
#define BATTLE_H

#include "card.h"

#define MAX_SCORE 5

/* Stack (decks) */
void  push(Node **top, MCard card);
MCard pop(Node **top);
int   isEmptyStack(const Node *top);
void  freeStack(Node **top);

/* Queue (opponent's incoming cards) */
void  initQueue(Queue *q);
void  enqueue(Queue *q, MCard card);
MCard dequeue(Queue *q);
int   isEmptyQueue(const Queue *q);
void  freeQueue(Queue *q);

/* Battle */
void displayScale(int score);
int  playBattle(int difficulty);   /* returns 1 if the player wins */

#endif
