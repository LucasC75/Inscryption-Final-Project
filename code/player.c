/* player.c - player setup, deck management, hand management, card selection */
#include "player.h"

/* Reads a whole line and keeps asking until it is a number from min to max. */
int readNumber(const char *prompt, int min, int max)
{
    char line[64];
    int value;

    while (1) {
        printf("%s", prompt);
        if (fgets(line, sizeof line, stdin) == NULL) {   /* input was closed */
            printf("\nGoodbye!\n");
            exit(0);
        }
        if (sscanf(line, "%d", &value) == 1 && value >= min && value <= max)
            return value;
        printf("Please enter a number from %d to %d.\n", min, max);
    }
}

/* ============================ PLAYER SETUP ============================ */

/* Empty hand, a random main deck and a full squirrel deck. */
void setupPlayer(Player *p)
{
    const MCard *squirrel = findCard("Squirrel");

    p->deck.top = NULL;
    p->deck.size = 0;
    p->squirrels.top = NULL;
    p->squirrels.size = 0;
    p->handSize = 0;

    for (int i = 0; i < DECK_SIZE; i++)
        push(&p->deck, randomCard(0));
    for (int i = 0; i < SQUIRREL_COUNT; i++)
        push(&p->squirrels, *squirrel);
}

void freePlayer(Player *p)
{
    freeStack(&p->deck);
    freeStack(&p->squirrels);
    p->handSize = 0;
}

/* ========================== DECK MANAGEMENT ========================== */

void showDeck(const Stack *deck)
{
    int i = 1;
    for (const Node *n = deck->top; n != NULL; n = n->next) {
        printf("%2d) ", i++);
        printCard(&n->card);
    }
}

/* Draw one card from the deck or the squirrel deck.
   Returns 0 if both decks are empty. */
int drawCard(Player *p)
{
    Stack *from;
    MCard card;

    if (isEmptyStack(&p->deck) && isEmptyStack(&p->squirrels))
        return 0;
    if (p->handSize >= HAND_MAX) {
        printf("Your hand is full, so you don't draw.\n");
        return 1;
    }

    if (isEmptyStack(&p->deck))
        from = &p->squirrels;
    else if (isEmptyStack(&p->squirrels))
        from = &p->deck;
    else {
        printf("\nDraw from: 1) Your deck (%d left)  2) Squirrel deck (%d left)\n",
               p->deck.size, p->squirrels.size);
        from = (readNumber("> ", 1, 2) == 1) ? &p->deck : &p->squirrels;
    }

    card = pop(from);
    addToHand(p, card);
    printf("You drew: ");
    printCard(&card);
    return 1;
}

/* ========================== HAND MANAGEMENT ========================== */

/* Returns 0 if the hand is full. */
int addToHand(Player *p, MCard card)
{
    if (p->handSize >= HAND_MAX)
        return 0;
    p->hand[p->handSize++] = card;
    return 1;
}

/* Takes a card out of the hand and slides the rest down. */
MCard removeFromHand(Player *p, int index)
{
    MCard card = p->hand[index];
    for (int i = index; i < p->handSize - 1; i++)
        p->hand[i] = p->hand[i + 1];
    p->handSize--;
    return card;
}

void showHand(const Player *p)
{
    printf("\nYour hand:\n");
    if (p->handSize == 0)
        printf("  (empty)\n");
    for (int i = 0; i < p->handSize; i++) {
        printf("%2d) ", i + 1);
        printCard(&p->hand[i]);
    }
}

/* =========================== CARD SELECTION =========================== */

/* Shows the hand and asks for a card. Returns its index, or -1 to go back. */
int selectCard(const Player *p)
{
    if (p->handSize == 0) {
        printf("Your hand is empty.\n");
        return -1;
    }
    showHand(p);
    return readNumber("Pick a card (0 = back): ", 0, p->handSize) - 1;
}
