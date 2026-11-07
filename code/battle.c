#include "player_card"

int isEmptyDeck(Deck *top) {
    return top == NULL;
}

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
