#include "battle.h"
#include "player.h"

#define CELL 16   /* width of a card box on the board */

/*=============================== STACK ================================*/

void push(Node **top, MCard card)
{
    Node *n = malloc(sizeof(Node));
    if (n == NULL) {
        fprintf(stderr, "Out of memory\n");
        exit(EXIT_FAILURE);
    }
    n->card = card;
    n->next = *top;
    *top = n;
}

MCard pop(Node **top)
{
    MCard card = {"", 0, 0, 0, 0};
    Node *old = *top;

    if (old == NULL)
        return card;
    card = old->card;
    *top = old->next;
    free(old);
    return card;
}

int isEmptyStack(const Node *top)
{
    return top == NULL;
}

void freeStack(Node **top)
{
    while (*top != NULL)
        pop(top);
}

/*=============================== QUEUE ================================*/

void initQueue(Queue *q)
{
    q->front = q->rear = NULL;
}

void enqueue(Queue *q, MCard card)
{
    Node *n = malloc(sizeof(Node));
    if (n == NULL) {
        fprintf(stderr, "Out of memory\n");
        exit(EXIT_FAILURE);
    }
    n->card = card;
    n->next = NULL;

    if (isEmptyQueue(q))
        q->front = n;
    else
        q->rear->next = n;
    q->rear = n;
}

MCard dequeue(Queue *q)
{
    MCard card = {"", 0, 0, 0, 0};
    Node *n = q->front;

    if (n == NULL)
        return card;
    card = n->card;
    q->front = n->next;
    if (q->front == NULL)
        q->rear = NULL;
    free(n);
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

/*============================== DISPLAY ===============================*/

/* Scale: 'O' moves right when you deal damage, left when you take it. */
void displayScale(int score)
{
    if (score > MAX_SCORE)  score = MAX_SCORE;
    if (score < -MAX_SCORE) score = -MAX_SCORE;
    printf("SCALE: YOU LOSE [");
    for (int i = -MAX_SCORE; i <= MAX_SCORE; i++)
        putchar(i == score ? 'O' : i == 0 ? '|' : '-');
    printf("] YOU WIN   (%+d)\n", score);
}

static void printRow(const MCard row[])
{
    for (int line = 0; line < 5; line++) {
        for (int i = 0; i < LANES; i++) {
            char s[40] = "";
            if (line == 0 || line == 4) {
                printf("+%.*s+ ", CELL, "--------------------");
                continue;
            }
            if (isEmptySlot(row[i])) {
                if (line == 2)
                    snprintf(s, sizeof s, "    empty");
            } else if (line == 1) {
                snprintf(s, sizeof s, "%s", row[i].name);
            } else if (line == 2) {
                snprintf(s, sizeof s, "ATK %d  HP %d", row[i].attack, row[i].health);
            } else if (row[i].sigil != SIG_NONE) {
                snprintf(s, sizeof s, "%s", sigilName(row[i].sigil));
            }
            printf("|%-*.*s| ", CELL, CELL, s);
        }
        putchar('\n');
    }
}

static void printBoard(const MCard opp[], const MCard you[], const Queue *q)
{
    const Node *n = q->front;

    printf("\nIncoming:");
    for (int i = 0; i < 3 && n != NULL; i++, n = n->next)
        printf(" [%s]", n->card.name);
    printf("\n--- OPPONENT ---\n");
    printRow(opp);
    printf("--- YOU ---\n");
    printRow(you);
    printf("      Lane 1             Lane 2             Lane 3             Lane 4\n");
}

/*========================== BATTLE RESOLUTION =========================*/

static void growRow(MCard row[])
{
    for (int i = 0; i < LANES; i++)
        if (growCard(&row[i]))
            printf("%s grew up! (Fledgling)\n", row[i].name);
}

/* Every card in atk[] strikes the lane in front of it.
   owner is the defending Player (for Bees Within / Unkillable) or NULL.
   Returns the damage that went to the scale. */
static int attackRow(MCard atk[], MCard def[], Player *owner, const char *who)
{
    int direct = 0;

    for (int i = 0; i < LANES; i++) {
        MCard *a = &atk[i];
        MCard *d = &def[i];

        if (isEmptySlot(*a) || a->attack <= 0)
            continue;

        if (attacksDirectly(*a, *d)) {
            printf("%s %s hits the scale for %d.\n", who, a->name, a->attack);
            direct += a->attack;
            continue;
        }

        printf("%s %s attacks %s for %d.\n", who, a->name, d->name, a->attack);
        if (owner && d->sigil == SIG_BEES_WITHIN) {
            printf("Bees Within: a Bee is added to your hand.\n");
            addToHand(owner, findCard("Bee"));
        }
        if (damageCard(*a, d)) {
            printf("%s dies.\n", d->name);
            if (owner && d->sigil == SIG_UNKILLABLE) {
                printf("Unkillable: %s returns to your hand.\n", d->name);
                addToHand(owner, findCard(d->name));
            }
            *d = (MCard){"", 0, 0, 0, 0};
        }
    }
    return direct;
}

static void fillQueue(Queue *q, int maxCost)
{
    for (int i = 0; i < 10; i++)
        enqueue(q, randomCard(maxCost));
}

/* Opponent moves up to 'count' cards from its queue into empty lanes. */
static void opponentPlace(MCard opp[], Queue *q, int count, int maxCost)
{
    for (int k = 0; k < count; k++) {
        int lanes[LANES], n = 0, lane;

        for (int i = 0; i < LANES; i++)
            if (isEmptySlot(opp[i]))
                lanes[n++] = i;
        if (n == 0)
            return;
        if (isEmptyQueue(q))
            fillQueue(q, maxCost);

        lane = lanes[rand() % n];
        opp[lane] = dequeue(q);
        printf("Opponent plays %s in lane %d.\n", opp[lane].name, lane + 1);
    }
}

/*========================== WINNER DETERMINATION ======================*/

static int winner(int score)
{
    if (score >= MAX_SCORE) return 1;    /* player wins   */
    if (score <= -MAX_SCORE) return -1;  /* opponent wins */
    return 0;
}

/* difficulty: 1 easy, 2 medium, 3 hard */
int playBattle(int difficulty)
{
    Player p;
    MCard opp[LANES];
    Queue q;
    int maxCost = difficulty == 1 ? 1 : difficulty == 2 ? 2 : 4;
    int perTurn = difficulty == 3 ? 2 : 1;
    int score = 0, turn = 1, result = 0;

    for (int i = 0; i < LANES; i++)
        opp[i] = (MCard){"", 0, 0, 0, 0};
    initPlayer(&p);
    initQueue(&q);
    fillQueue(&q, maxCost);
    opponentPlace(opp, &q, perTurn, maxCost);

    while (result == 0) {
        int choice = 0;

        printf("\n=================== TURN %d ===================\n", turn);
        growRow(p.board);
        if (turn > 1)
            drawPhase(&p);

        /* Player's turn */
        while (choice != 3) {
            printBoard(opp, p.board, &q);
            displayScale(score);
            choice = readInt("1.Play a card  2.View hand  3.Ring the bell (end turn)\n> ", 1, 3);
            if (choice == 1)
                playCard(&p);
            else if (choice == 2)
                displayHand(&p);
        }

        printf("\n--- You ring the bell ---\n");
        score += attackRow(p.board, opp, NULL, "Your");
        result = winner(score);
        if (result != 0)
            break;

        /* Opponent's turn */
        printf("\n--- Opponent's turn ---\n");
        growRow(opp);
        opponentPlace(opp, &q, perTurn, maxCost);
        score -= attackRow(opp, p.board, &p, "Enemy");
        result = winner(score);
        turn++;
    }

    displayScale(score);
    printf(result == 1 ? "\n*** The scale tips in your favor. YOU WIN! ***\n"
                       : "\n*** The scale tips against you. YOU LOSE. ***\n");

    freePlayer(&p);
    freeQueue(&q);
    return result == 1;
}
