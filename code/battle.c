/* battle.c - stack and queue, the board, battle resolution,
   the life point scale and winner determination */
#include "player.h"

#define LANES         4
#define MAX_SCORE     5    /* tip the scale this far to take a life */
#define OPP_DECK_SIZE 15   /* cards in the opponent's queue */

#define OPPONENT 0         /* top row of the board */
#define PLAYER   1         /* bottom row of the board */
#define NOBODY  -1

static const MCard NO_CARD = {"", 0, 0, 0, 0};
static const char *OWNER[2] = {"Opponent's", "Your"};

static MCard board[2][LANES];   /* board[OPPONENT][lane] and board[PLAYER][lane] */
static int score;               /* the scale: + is damage to the opponent, - is damage to you */
static int lives[2];            /* lives left for each side */
static Queue oppCards;          /* cards the opponent will play, in order */

/* ================================ STACK ================================ */

static Node *newNode(MCard card)
{
    Node *node = malloc(sizeof(Node));
    if (node == NULL) {
        printf("Out of memory!\n");
        exit(1);
    }
    node->card = card;
    node->next = NULL;
    return node;
}

void push(Stack *s, MCard card)
{
    Node *node = newNode(card);
    node->next = s->top;
    s->top = node;
    s->size++;
}

MCard pop(Stack *s)
{
    MCard card = NO_CARD;
    Node *old = s->top;

    if (isEmptyStack(s))
        return card;
    card = old->card;
    s->top = old->next;
    s->size--;
    free(old);
    return card;
}

int isEmptyStack(const Stack *s)
{
    return s->top == NULL;
}

void freeStack(Stack *s)
{
    while (!isEmptyStack(s))
        pop(s);
}

/* ================================ QUEUE ================================ */

void enqueue(Queue *q, MCard card)
{
    Node *node = newNode(card);
    if (isEmptyQueue(q))
        q->front = node;
    else
        q->rear->next = node;
    q->rear = node;
    q->size++;
}

MCard dequeue(Queue *q)
{
    MCard card = NO_CARD;
    Node *old = q->front;

    if (isEmptyQueue(q))
        return card;
    card = old->card;
    q->front = old->next;
    if (q->front == NULL)
        q->rear = NULL;
    q->size--;
    free(old);
    return card;
}

int isEmptyQueue(const Queue *q)
{
    return q->front == NULL;
}

void freeQueue(Queue *q)
{
    while (!isEmptyQueue(q))
        dequeue(q);
}

/* ========================= LIFE POINT SCALE ========================= */

/* e.g.  Scale: YOU LOSE [-----|--O--] YOU WIN  (+2) */
static void displayScale(void)
{
    printf("Scale: YOU LOSE [");
    for (int i = -MAX_SCORE; i <= MAX_SCORE; i++)
        putchar(i == score ? 'O' : (i == 0 ? '|' : '-'));
    printf("] YOU WIN  (%+d)\n", score);
}

/* When the scale tips all the way, that side loses a life and the scale resets.
   Returns the winner (PLAYER or OPPONENT), or NOBODY if the battle goes on. */
static int checkScale(void)
{
    int loser;

    if (score >= MAX_SCORE)
        loser = OPPONENT;
    else if (score <= -MAX_SCORE)
        loser = PLAYER;
    else
        return NOBODY;

    lives[loser]--;
    score = 0;
    printf("\n*** The scale tips! %s a life. ***\n",
           loser == PLAYER ? "You lose" : "The opponent loses");
    if (lives[loser] == 0)
        return (loser == PLAYER) ? OPPONENT : PLAYER;
    printf("The scale resets. Lives - You: %d  Opponent: %d\n", lives[PLAYER], lives[OPPONENT]);
    return NOBODY;
}

/* =============================== BOARD =============================== */

static void showGame(const Player *p)
{
    printf("\nOPPONENT  lives: %d   cards left: %d   next card: %s\n",
           lives[OPPONENT], oppCards.size,
           isEmptyQueue(&oppCards) ? "none" : oppCards.front->card.name);
    for (int lane = 0; lane < LANES; lane++)
        printf("     Lane %d        ", lane + 1);
    printf("\n");
    printCardRow(board[OPPONENT], LANES);
    printCardRow(board[PLAYER], LANES);
    printf("YOU       lives: %d   deck: %d   squirrels: %d\n",
           lives[PLAYER], p->deck.size, p->squirrels.size);
    displayScale();
}

/* Rabbit Hole, Bees Within and Unkillable give a card to its owner:
   you get it in your hand, the opponent gets it at the back of its queue. */
static void giveCard(Player *p, int side, const MCard *card)
{
    if (card == NULL)
        return;
    if (side == OPPONENT)
        enqueue(&oppCards, *card);
    else if (addToHand(p, *card))
        printf("  %s was added to your hand.\n", card->name);
}

static void killCard(Player *p, int side, int lane)
{
    MCard dead = board[side][lane];

    board[side][lane] = NO_CARD;
    printf("  %s %s dies.\n", OWNER[side], dead.name);
    if (dead.sigil == UNKILLABLE)
        giveCard(p, side, findCard(dead.name));
}

/* Fledgling cards grow at the start of their owner's turn. */
static void growFledglings(int side)
{
    for (int lane = 0; lane < LANES; lane++) {
        MCard *c = &board[side][lane];
        if (growUp(c))
            printf("%s %s grew up! (ATK %d  HP %d)\n", OWNER[side], c->name, c->attack, c->health);
    }
}

/* ========================= BATTLE RESOLUTION ========================= */

/* Every card on one side attacks the space across from it. */
static void attackPhase(Player *p, int side)
{
    int enemy = 1 - side;

    for (int lane = 0; lane < LANES; lane++) {
        MCard *attacker = &board[side][lane];
        MCard *defender = &board[enemy][lane];

        if (isEmptySlot(attacker) || attacker->attack == 0)
            continue;

        if (attacksDirectly(attacker, defender)) {
            score += (side == PLAYER) ? attacker->attack : -attacker->attack;
            printf("  %s %s hits %s for %d!\n", OWNER[side], attacker->name,
                   side == PLAYER ? "the opponent" : "you", attacker->attack);
        } else {
            int damage = damageDealt(attacker, defender);
            defender->health -= damage;
            printf("  %s %s attacks %s for %d damage.\n", OWNER[side], attacker->name,
                   defender->name, damage);
            if (defender->sigil == BEES_WITHIN)
                giveCard(p, enemy, findCard("Bee"));
            if (defender->health <= 0)
                killCard(p, enemy, lane);
        }
    }
}

/* Pick a card, pay its blood by sacrificing your cards, then place it.
   Returns 1 if a card was played. */
static int playCard(Player *p)
{
    int pick = selectCard(p);
    int chosen[LANES] = {0};
    int available = 0;
    int blood = 0;
    int lane;
    MCard card;

    if (pick < 0)
        return 0;
    card = p->hand[pick];

    /* Card validation: is there enough blood on the board? */
    for (lane = 0; lane < LANES; lane++)
        if (!isEmptySlot(&board[PLAYER][lane]))
            available += bloodValue(&board[PLAYER][lane]);
    if (available < card.requirement) {
        printf("%s needs %d blood, but your cards can only give %d.\n",
               card.name, card.requirement, available);
        return 0;
    }

    printf("\nYour side of the board:\n");
    printCardRow(board[PLAYER], LANES);

    /* Choose sacrifices (nothing happens until enough blood is chosen) */
    while (blood < card.requirement) {
        printf("%s needs %d more blood.\n", card.name, card.requirement - blood);
        lane = readNumber("Sacrifice the card in lane (1-4, 0 = cancel): ", 0, LANES) - 1;
        if (lane < 0)
            return 0;
        if (isEmptySlot(&board[PLAYER][lane]) || chosen[lane]) {
            printf("Pick one of your cards that isn't already chosen.\n");
            continue;
        }
        chosen[lane] = 1;
        blood += bloodValue(&board[PLAYER][lane]);
    }
    for (lane = 0; lane < LANES; lane++) {
        if (!chosen[lane])
            continue;
        if (board[PLAYER][lane].sigil == MANY_LIVES) {
            printf("%s is sacrificed... but survives! (Many Lives)\n", board[PLAYER][lane].name);
        } else {
            printf("%s is sacrificed.\n", board[PLAYER][lane].name);
            board[PLAYER][lane] = NO_CARD;
        }
    }

    /* Place the card in an empty lane */
    for (lane = 0; lane < LANES; lane++)
        if (isEmptySlot(&board[PLAYER][lane]))
            break;
    if (lane == LANES) {
        printf("There is no empty space for %s.\n", card.name);
        return 0;
    }
    while (1) {
        lane = readNumber("Place it in lane (1-4): ", 1, LANES) - 1;
        if (isEmptySlot(&board[PLAYER][lane]))
            break;
        printf("That lane is taken.\n");
    }
    board[PLAYER][lane] = removeFromHand(p, pick);
    printf("You play %s in lane %d.\n", card.name, lane + 1);
    if (card.sigil == RABBIT_HOLE)
        giveCard(p, PLAYER, findCard("Squirrel"));
    return 1;
}

/* Your turn: play as many cards as you like, then ring the bell.
   Returns 0 if you give up. */
static int playerTurn(Player *p)
{
    while (1) {
        int choice = readNumber("\n1) Play a card  2) Ring the bell  3) Show board  4) Give up\n> ", 1, 4);
        if (choice == 1) {
            if (playCard(p))
                showGame(p);
        } else if (choice == 2) {
            return 1;
        } else if (choice == 3) {
            showGame(p);
        } else {
            return 0;
        }
    }
}

/* The opponent plays the next card in its queue into a random empty lane.
   It doesn't pay blood, but it may skip its turn. */
static void opponentPlays(Player *p, int skipChance)
{
    int open[LANES];
    int count = 0;
    int lane;

    for (lane = 0; lane < LANES; lane++)
        if (isEmptySlot(&board[OPPONENT][lane]))
            open[count++] = lane;
    if (count == 0 || isEmptyQueue(&oppCards))
        return;
    if (rand() % 100 < skipChance) {
        printf("The opponent waits...\n");
        return;
    }

    lane = open[rand() % count];
    board[OPPONENT][lane] = dequeue(&oppCards);
    printf("The opponent plays %s in lane %d.\n", board[OPPONENT][lane].name, lane + 1);
    if (board[OPPONENT][lane].sigil == RABBIT_HOLE)
        giveCard(p, OPPONENT, findCard("Squirrel"));
}

/* ============================ THE BATTLE ============================ */

/* Difficulty 1-3 changes the opponent's cards, how often it skips, and its lives.
   Returns 1 if you win. */
int battle(Player *p, int difficulty)
{
    int rarityBonus = (difficulty - 2) * 20;   /* easy -20, medium 0, hard +20 */
    int skipChance = (3 - difficulty) * 20;    /* easy 40%, medium 20%, hard 0% */
    int winner = NOBODY;
    int turn = 1;

    /* New board, scale and lives */
    for (int side = 0; side < 2; side++)
        for (int lane = 0; lane < LANES; lane++)
            board[side][lane] = NO_CARD;
    score = 0;
    lives[PLAYER] = 2;
    lives[OPPONENT] = (difficulty == 3) ? 2 : 1;   /* hard = boss with 2 lives */

    oppCards.front = oppCards.rear = NULL;
    oppCards.size = 0;
    for (int i = 0; i < OPP_DECK_SIZE; i++)
        enqueue(&oppCards, randomCard(rarityBonus));

    /* Opening hand: 3 cards from your deck and 1 squirrel */
    for (int i = 0; i < 3; i++)
        addToHand(p, pop(&p->deck));
    addToHand(p, pop(&p->squirrels));

    printf("\nThe battle begins!\n");
    opponentPlays(p, 0);

    while (winner == NOBODY) {
        printf("\n=================== TURN %d ===================\n", turn++);

        /* Your turn */
        growFledglings(PLAYER);
        showGame(p);
        if (!drawCard(p)) {
            printf("Both of your decks are empty! You take 1 damage.\n");
            score--;
        }
        if (!playerTurn(p)) {
            printf("You give up.\n");
            winner = OPPONENT;
            break;
        }
        printf("\nYou ring the bell!\n");
        attackPhase(p, PLAYER);
        winner = checkScale();
        if (winner != NOBODY)
            break;

        /* Opponent's turn */
        printf("\n--- Opponent's turn ---\n");
        growFledglings(OPPONENT);
        opponentPlays(p, skipChance);
        attackPhase(p, OPPONENT);
        winner = checkScale();
    }

    freeQueue(&oppCards);

    /* Winner determination */
    if (winner == PLAYER)
        printf("\n*** YOU WIN! The opponent is defeated. ***\n");
    else
        printf("\n*** YOU LOSE... Better luck next time. ***\n");
    return winner == PLAYER;
}#include "player_card.h"

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

void enqueue(Queue *q, Mcard card)
{
    Deck *n = malloc(sizeof(Deck));
    n->card = card;
    n->next = NULL;

    if (isEmpty(q))
        q->front = n;
    else
        q->rear->next = n;
    q->rear = n;
}

Mcard dequeue(Queue *q)
{
    Mcard card = {0};
    if (isEmpty(q))
        return card;

    Deck *n = q->front;
    card = n->card;

    q->front = n->next;
    if (q->front == NULL)
        q->rear = NULL;

    free(n);
    return card;
}




