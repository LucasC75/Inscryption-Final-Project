/* card.c - card creation, validation, display and sigil (type) bonuses */
#include "card.h"

MCard cardList[MAX_CARDS];
int cardCount = 0;

static const char *SIGIL_NAMES[SIGIL_COUNT] = {
    "", "Fledgling", "Touch of Death", "Rabbit Hole", "Bees Within", "Unkillable",
    "Airborne", "Many Lives", "Worthy Sacrifice", "Mighty Leap", "Waterborne", "Mystery"
};

static const char *SIGIL_TEXT[SIGIL_COUNT] = {
    "",
    "Grows +1 ATK and +1 HP after a turn on the board.",
    "Kills any card it damages.",
    "When played, gives its owner a Squirrel.",
    "When hit, gives its owner a Bee.",
    "When it dies, a fresh copy goes back to its owner.",
    "Flies over cards and hits the opponent directly.",
    "Survives being sacrificed.",
    "Counts as 3 blood when sacrificed.",
    "Blocks Airborne attackers.",
    "Dives underwater on the enemy turn, so attacks go past it.",
    "Does nothing... yet."
};

/* ================================ CREATION ================================ */

/* Reads "Name, Health, Attack, Sigil, Requirement" lines into cardList.
   Lines starting with # are comments. Returns the number of cards, or -1. */
int loadCards(const char *filename)
{
    char line[128];
    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
        return -1;

    cardCount = 0;
    while (cardCount < MAX_CARDS && fgets(line, sizeof line, fp) != NULL) {
        MCard c;
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r')
            continue;
        /* %31[^,] reads up to 31 characters (NAME_LEN - 1) until a comma */
        if (sscanf(line, " %31[^,],%d,%d,%d,%d", c.name, &c.health, &c.attack,
                   &c.sigil, &c.requirement) == 5 && validateCard(&c))
            cardList[cardCount++] = c;
        else
            printf("Skipping bad card line: %s", line);
    }

    fclose(fp);
    return cardCount;
}

/* A card is valid if it has a name, is alive, and its numbers are in range. */
int validateCard(const MCard *c)
{
    return c->name[0] != '\0' && c->health > 0 && c->attack >= 0 &&
           c->sigil >= 0 && c->sigil < SIGIL_COUNT &&
           c->requirement >= 0 && c->requirement <= 4;
}

/* Returns the card with this name, or NULL if it is not in the list. */
const MCard *findCard(const char *name)
{
    for (int i = 0; i < cardCount; i++)
        if (strcmp(cardList[i].name, name) == 0)
            return &cardList[i];
    return NULL;
}

/* Picks a random card. Rarity depends on blood cost:
   common = 0 blood (50%), uncommon = 1 (30%), rare = 2 (15%), epic = 3+ (5%).
   rarityBonus shifts the odds: negative = weaker cards, positive = stronger. */
MCard randomCard(int rarityBonus)
{
    int picks[MAX_CARDS];
    int count = 0;
    int tier;
    int roll = rand() % 100 + rarityBonus;

    if (roll < 50)      tier = 0;
    else if (roll < 80) tier = 1;
    else if (roll < 95) tier = 2;
    else                tier = 3;

    for (int i = 0; i < cardCount; i++) {
        int cost = cardList[i].requirement;
        if (cost == tier || (tier == 3 && cost > 3))
            picks[count++] = i;
    }
    if (count == 0)   /* no card of that rarity: pick any card */
        return cardList[rand() % cardCount];
    return cardList[picks[rand() % count]];
}

/* Board spaces with no card are stored as a card with 0 health. */
int isEmptySlot(const MCard *c)
{
    return c->health <= 0;
}

/* ================================ DISPLAY ================================ */

const char *sigilName(int sigil)
{
    return SIGIL_NAMES[sigil];
}

/* One line, e.g. "Wolf           ATK 3  HP 2  Cost 2" */
void printCard(const MCard *c)
{
    printf("%-14s ATK %d  HP %d  Cost %d  %s\n",
           c->name, c->attack, c->health, c->requirement, sigilName(c->sigil));
}

static void printBorder(int count)
{
    for (int i = 0; i < count; i++)
        printf("+----------------+ ");
    printf("\n");
}

/* Prints a row of board spaces as boxes, side by side. */
void printCardRow(const MCard row[], int count)
{
    char stats[20];

    printBorder(count);
    for (int i = 0; i < count; i++)
        printf("|%-16.16s| ", isEmptySlot(&row[i]) ? "" : row[i].name);
    printf("\n");
    for (int i = 0; i < count; i++) {
        if (isEmptySlot(&row[i]))
            snprintf(stats, sizeof stats, "    (empty)");
        else
            snprintf(stats, sizeof stats, "ATK %d  HP %d", row[i].attack, row[i].health);
        printf("|%-16s| ", stats);
    }
    printf("\n");
    for (int i = 0; i < count; i++)
        printf("|%-16.16s| ", isEmptySlot(&row[i]) ? "" : sigilName(row[i].sigil));
    printf("\n");
    printBorder(count);
}

/* Lists every card and what each sigil does. */
void printAllCards(void)
{
    printf("\nALL CARDS\n---------\n");
    for (int i = 0; i < cardCount; i++)
        printCard(&cardList[i]);

    printf("\nSIGILS\n------\n");
    for (int s = 1; s < SIGIL_COUNT; s++)
        printf("%-17s %s\n", SIGIL_NAMES[s], SIGIL_TEXT[s]);
}

/* ========================= TYPE (SIGIL) BONUSES ========================= */

/* How much blood this card gives when sacrificed. */
int bloodValue(const MCard *c)
{
    return c->sigil == WORTHY_SACRIFICE ? 3 : 1;
}

/* Does this attack go past the defending space and hit the scale? */
int attacksDirectly(const MCard *attacker, const MCard *defender)
{
    if (isEmptySlot(defender))
        return 1;
    if (defender->sigil == WATERBORNE)   /* defender is underwater */
        return 1;
    if (attacker->sigil == AIRBORNE && defender->sigil != MIGHTY_LEAP)
        return 1;
    return 0;
}

/* Damage done to the defending card. */
int damageDealt(const MCard *attacker, const MCard *defender)
{
    if (attacker->sigil == TOUCH_OF_DEATH)
        return defender->health;   /* always enough to kill it */
    return attacker->attack;
}

/* Fledgling: the card grows once, then loses the sigil. Returns 1 if it grew. */
int growUp(MCard *c)
{
    if (c->sigil != FLEDGLING)
        return 0;
    c->attack++;
    c->health++;
    c->sigil = NO_SIGIL;
    return 1;
}
