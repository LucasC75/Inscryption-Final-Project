#include "player_card.h"

int isEmptyDeck(Deck *top) {
    return top == NULL;
}

//STACK FUNCTIONS
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
