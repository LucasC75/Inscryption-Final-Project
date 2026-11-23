/* player.h - the Player structure, deck structures, and the
   functions in player.c and battle.c */
#ifndef PLAYER_H
#define PLAYER_H

#include "card.h"

#define HAND_MAX       10   /* most cards you can hold */
#define DECK_SIZE      12   /* cards in your main deck */
#define SQUIRREL_COUNT 10   /* cards in your squirrel deck */

/* One link in a linked list of cards (used by the stack and the queue) */
typedef struct Node {
    MCard card;
    struct Node *next;
} Node;

/* Decks are stacks: you draw from the top */
typedef struct {
    Node *top;
    int size;
} Stack;

/* The opponent's cards are a queue: played in the order they were added */
typedef struct {
    Node *front;
    Node *rear;
    int size;
} Queue;

typedef struct {
    Stack deck;        /* main deck */
    Stack squirrels;   /* squirrel deck */
    MCard hand[HAND_MAX];
    int handSize;
} Player;

/* ---- player.c ---- */
int   readNumber(const char *prompt, int min, int max);
void  setupPlayer(Player *p);
void  freePlayer(Player *p);
void  showDeck(const Stack *deck);
int   addToHand(Player *p, MCard card);
MCard removeFromHand(Player *p, int index);
void  showHand(const Player *p);
int   drawCard(Player *p);
int   selectCard(const Player *p);

/* ---- battle.c ---- */
void  push(Stack *s, MCard card);
MCard pop(Stack *s);
int   isEmptyStack(const Stack *s);
void  freeStack(Stack *s);
void  enqueue(Queue *q, MCard card);
MCard dequeue(Queue *q);
int   isEmptyQueue(const Queue *q);
void  freeQueue(Queue *q);
int   battle(Player *p, int difficulty);

#endif
