#include "player.h"
#include "battle.h"

/* Reads a whole number between min and max. Quits cleanly on end of input. */
int readInt(const char *prompt, int min, int max)
{
    int value, c, ok;

    while (1) {
        printf("%s", prompt);
        ok = scanf("%d", &value);
        while ((c = getchar()) != '\n' && c != EOF)
            ;                                   /* discard rest of line */
        if (ok == EOF || (ok != 1 && c == EOF)) {
            printf("\nGoodbye!\n");
            exit(0);
        }
        if (ok == 1 && value >= min && value <= max)
            return value;
        printf("Please enter a number from %d to %d.\n", min, max);
    }
}

/*========================== PLAYER INITIALIZATION ======================*/

/* Rolls one card by rarity (rarity = blood cost):
   50% common (0), 30% uncommon (1), 15% rare (2), 5% epic (3-4). */
MCard rollCard(void)
{
    int rare = rand() % 100;

    if (rare < 50) return randomCard(0, 0);
    if (rare < 80) return randomCard(1, 1);
    if (rare < 95) return randomCard(2, 2);
    return randomCard(3, 4);
}

static const char *rarityName(MCard c)
{
    const char *names[] = {"Common", "Uncommon", "Rare", "Epic", "Epic"};
    return names[c.cost];
}

static void showDeck(const MCard deck[])
{
    printf("\n--- YOUR DECK ---\n");
    for (int i = 0; i < DECK_SIZE; i++) {
        printf("%2d. %-9s ", i + 1, rarityName(deck[i]));
        printCard(deck[i]);
    }
}

/* Rolls a random deck, lets the player reroll up to MAX_REROLLS cards,
   then pushes the deck onto the player's deck stack. */
void buildDeck(Player *p)
{
    MCard deck[DECK_SIZE];
    int rerolls = MAX_REROLLS;

    for (int i = 0; i < DECK_SIZE; i++)
        deck[i] = rollCard();

    while (rerolls > 0) {
        int slot;

        showDeck(deck);
        printf("Rerolls left: %d\n", rerolls);
        slot = readInt("Reroll which card? (1-12, 0 to keep this deck): ", 0, DECK_SIZE);
        if (slot == 0)
            break;

        printf("%s is rerolled into... ", deck[slot - 1].name);
        deck[slot - 1] = rollCard();
        printf("%s (%s)!\n", deck[slot - 1].name, rarityName(deck[slot - 1]));
        rerolls--;
    }
    showDeck(deck);

    for (int i = 0; i < DECK_SIZE; i++)
        push(&p->deck, deck[i]);
}

void initPlayer(Player *p)
{
    MCard squirrel = findCard("Squirrel");

    p->deck = NULL;
    p->squirrels = NULL;
    p->handSize = 0;
    for (int i = 0; i < LANES; i++)
        p->board[i] = (MCard){"", 0, 0, 0, 0};

    buildDeck(p);
    for (int i = 0; i < SQUIRREL_COUNT; i++)
        push(&p->squirrels, squirrel);

    /* Starting hand: 3 deck cards + 1 squirrel */
    for (int i = 0; i < 3; i++)
        addToHand(p, pop(&p->deck));
    addToHand(p, pop(&p->squirrels));
}

void freePlayer(Player *p)
{
    freeStack(&p->deck);
    freeStack(&p->squirrels);
    p->handSize = 0;
}

/*============================ HAND MANAGEMENT ==========================*/

void addToHand(Player *p, MCard c)
{
    if (p->handSize >= MAX_HAND) {
        printf("Your hand is full, %s is discarded.\n", c.name);
        return;
    }
    p->hand[p->handSize++] = c;
}

MCard removeFromHand(Player *p, int index)
{
    MCard c = p->hand[index];
    for (int i = index; i < p->handSize - 1; i++)
        p->hand[i] = p->hand[i + 1];
    p->handSize--;
    return c;
}

void displayHand(const Player *p)
{
    printf("--- YOUR HAND ---\n");
    if (p->handSize == 0)
        printf("(empty)\n");
    for (int i = 0; i < p->handSize; i++) {
        printf("%2d. ", i + 1);
        printCard(p->hand[i]);
    }
}

/*============================ DECK MANAGEMENT ==========================*/

void drawPhase(Player *p)
{
    int deckEmpty = isEmptyStack(p->deck);
    int squirrelEmpty = isEmptyStack(p->squirrels);
    int choice;
    MCard c;

    if (deckEmpty && squirrelEmpty) {
        printf("Both decks are empty, nothing to draw.\n");
        return;
    }
    if (deckEmpty)
        choice = 2;
    else if (squirrelEmpty)
        choice = 1;
    else
        choice = readInt("Draw from: 1.Main deck  2.Squirrel deck\n> ", 1, 2);

    c = (choice == 1) ? pop(&p->deck) : pop(&p->squirrels);
    printf("You drew: ");
    printCard(c);
    addToHand(p, c);
}

/*============================ CARD SELECTION ===========================*/

/* Returns the hand index chosen, or -1 to cancel. */
int selectCard(const Player *p)
{
    displayHand(p);
    if (p->handSize == 0)
        return -1;
    return readInt("Choose a card (0 to cancel): ", 0, p->handSize) - 1;
}

static int hasEmptyLane(const Player *p)
{
    for (int i = 0; i < LANES; i++)
        if (isEmptySlot(p->board[i]))
            return 1;
    return 0;
}

/* Choose a card, pay its blood cost by sacrificing, then place it. */
void playCard(Player *p)
{
    int idx = selectCard(p);
    int marked[LANES] = {0};
    int blood = 0, got = 0, lane;
    MCard c;

    if (idx < 0)
        return;
    c = p->hand[idx];

    for (int i = 0; i < LANES; i++)
        if (!isEmptySlot(p->board[i]))
            blood += bloodValue(p->board[i]);

    if (blood < c.cost) {
        printf("Not enough blood: %s needs %d, your board has %d.\n",
               c.name, c.cost, blood);
        return;
    }
    if (c.cost == 0 && !hasEmptyLane(p)) {
        printf("Your board is full.\n");
        return;
    }

    /* Pick sacrifices (nothing is removed until the cost is paid) */
    while (got < c.cost) {
        printf("Blood %d/%d. ", got, c.cost);
        lane = readInt("Sacrifice the card in lane (1-4, 0 to cancel): ", 0, LANES) - 1;
        if (lane < 0)
            return;
        if (isEmptySlot(p->board[lane]) || marked[lane]) {
            printf("You can't sacrifice that lane.\n");
            continue;
        }
        marked[lane] = 1;
        got += bloodValue(p->board[lane]);
    }

    for (int i = 0; i < LANES; i++) {
        if (!marked[i])
            continue;
        if (p->board[i].sigil == SIG_MANY_LIVES) {
            printf("%s is sacrificed but survives (Many Lives).\n", p->board[i].name);
        } else {
            printf("%s is sacrificed.\n", p->board[i].name);
            p->board[i] = (MCard){"", 0, 0, 0, 0};
        }
    }

    if (!hasEmptyLane(p)) {
        printf("No empty lane to place %s.\n", c.name);
        return;
    }

    do {
        lane = readInt("Place it in lane (1-4): ", 1, LANES) - 1;
        if (!isEmptySlot(p->board[lane]))
            printf("That lane is taken.\n");
    } while (!isEmptySlot(p->board[lane]));

    p->board[lane] = removeFromHand(p, idx);
    printf("You played %s in lane %d.\n", c.name, lane + 1);

    if (c.sigil == SIG_RABBIT_HOLE) {
        printf("Rabbit Hole: a Squirrel is added to your hand.\n");
        addToHand(p, findCard("Squirrel"));
    }
}
