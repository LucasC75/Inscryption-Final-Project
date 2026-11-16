#include "player_card.h"

int score = 0;
int maxScore = 5;

//NEEDS THE 2D ARRAY FOR BOARD and the display function for it
int isEmptyDeck(Deck *top) {
    return top == NULL;
}

//================================STACK FUNCTIONS================================
void push(Deck **top, MCard card) {
    Deck *node = malloc(sizeof(Deck));
    if (node == NULL) {
        fprintf(stderr, "push: out of memory\n");
        exit(EXIT_FAILURE);
    }
    node->card = card;
    node->next = *top;
    *top = node;
}

MCard pop(Deck **top) {
    MCard card = {0, 0, 0, 0};
 
    if (isEmptyDeck(*top)) {
        fprintf(stderr, "pop: deck is empty\n");
        return card;
    }
 
    Deck *old = *top;    /* remember the node being removed     */
    card = old->card;    /* copy the card out before freeing    */
    *top = old->next;    /* move the top down to the next node  */
    free(old);
 
    return card;
}

//========================================QUEUE FUNCTIONS========================================

void enqueue(Deck *q, int value)
{
    Node *n = malloc(sizeof(Node));
    n->data = value;
    n->next = NULL;

    if (isEmpty(q))
        q->front = n;
    else
        q->rear->next = n;
    q->rear = n;
}

int dequeue(Queue *q)
{
    if (isEmpty(q))
        return -1;

    Node *n = q->front;
    int value = n->data;

    q->front = n->next;
    if (q->front == NULL)
        q->rear = NULL;

    free(n);
    return value;
}





