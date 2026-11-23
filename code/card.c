#include "card.h"

MCard cardList[MAX_CARDS];
int   cardCount = 0;

static const char *SIGIL_NAMES[SIG_COUNT] = {
    "None", "Fledgling", "Touch of Death", "Rabbit Hole", "Bees Within",
    "Unkillable", "Airborne", "Many Lives", "Worthy Sacrifice",
    "Mighty Leap", "Waterborne", "Mystery"
};

/*============================ CARD CREATION ============================*/

/* Reads "Name, Health, Attack, Sigil, Cost" lines. Lines starting with # are
   comments. Returns the number of cards loaded, or -1 if the file is missing. */
int loadCards(const char *filename)
{
    FILE *fp = fopen(filename, "r");
    char line[128];

    if (fp == NULL)
        return -1;

    cardCount = 0;
    while (cardCount < MAX_CARDS && fgets(line, sizeof line, fp)) {
        MCard c;
        int len;

        if (line[0] == '#')
            continue;
        if (sscanf(line, " %31[^,],%d,%d,%d,%d",
                   c.name, &c.health, &c.attack, &c.sigil, &c.cost) != 5)
            continue;

        len = (int)strlen(c.name);              /* trim trailing spaces */
        while (len > 0 && c.name[len - 1] == ' ')
            c.name[--len] = '\0';

        if (isValidCard(c))
            cardList[cardCount++] = c;
        else
            printf("Skipping invalid card: %s\n", c.name);
    }

    fclose(fp);
    return cardCount;
}

/* Returns a fresh copy of the named card (an empty card if not found). */
MCard findCard(const char *name)
{
    MCard none = {"", 0, 0, 0, 0};
    for (int i = 0; i < cardCount; i++)
        if (strcmp(cardList[i].name, name) == 0)
            return cardList[i];
    return none;
}

/* Random card (never a Squirrel) costing at most maxCost blood. */
MCard randomCard(int maxCost)
{
    MCard c;
    do {
        c = cardList[rand() % cardCount];
    } while (c.cost > maxCost || strcmp(c.name, "Squirrel") == 0);
    return c;
}

/*============================== VALIDATION =============================*/

int isValidCard(MCard c)
{
    return c.name[0] != '\0' && c.health > 0 && c.attack >= 0 &&
           c.sigil >= 0 && c.sigil < SIG_COUNT && c.cost >= 0 && c.cost <= 4;
}

int isEmptySlot(MCard c)
{
    return c.health <= 0;
}

/*=============================== DISPLAY ===============================*/

const char *sigilName(int sigil)
{
    if (sigil < 0 || sigil >= SIG_COUNT)
        return "?";
    return SIGIL_NAMES[sigil];
}

void printCard(MCard c)
{
    printf("%-14s ATK %d  HP %d  Blood %d  Sigil: %s\n",
           c.name, c.attack, c.health, c.cost, sigilName(c.sigil));
}

void printCardList(void)
{
    printf("------------------- CARD LIST -------------------\n");
    for (int i = 0; i < cardCount; i++)
        printCard(cardList[i]);
    printf("-------------------------------------------------\n");
}

/*==================== SIGIL (TYPE-BONUS) CALCULATIONS ==================*/

/* Blood a card gives when sacrificed. */
int bloodValue(MCard c)
{
    return c.sigil == SIG_WORTHY_SACRIFICE ? 3 : 1;
}

/* Fledgling: +1 attack, +1 health, loses the sigil. Returns 1 if it grew. */
int growCard(MCard *c)
{
    if (isEmptySlot(*c) || c->sigil != SIG_FLEDGLING)
        return 0;
    c->attack++;
    c->health++;
    c->sigil = SIG_NONE;
    return 1;
}

/* Does this attack skip the defender and hit the scale? */
int attacksDirectly(MCard attacker, MCard defender)
{
    if (isEmptySlot(defender) || defender.sigil == SIG_WATERBORNE)
        return 1;
    return attacker.sigil == SIG_AIRBORNE && defender.sigil != SIG_MIGHTY_LEAP;
}

/* Applies damage to the defender. Returns 1 if the defender died. */
int damageCard(MCard attacker, MCard *defender)
{
    if (attacker.sigil == SIG_TOUCH_OF_DEATH)
        defender->health = 0;
    else
        defender->health -= attacker.attack;
    return defender->health <= 0;
}
