#include "player_card.h"

//==========================Card creation. Read from the .txt file and copy it into an array=======================

int loadCards(const char *filename, Mcard cards[], int maxCards) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Error: could not open %s\n", filename);
        return -1;
    }

     int count = 0;
    while (count < maxCards &&
           fscanf(fp, "%49s %d %d %d %d", cards[count].name,
                  &cards[count].health, &cards[count].attack, &cards[count].sigil, &cards[count].requirement) == 5) {
        count++;
    }

    fclose(fp);
    return count;
}
//When calling in main should look like this: 
/*int main(void) {
    Mcard cardList[MAX_CARDS];
    int n = loadCards("card_definitions.txt", cardList, MAX_CARDS);
 
    if (n < 0) {
        return 1;
    }
 --------------------------DISPLAY THE CARD LIST------------------------
    for (int i = 0; i < n; i++) {
        printf("Card name: %s \n
                --------------------\n
                Health: %d\n
                Attack: %d\n
                Sigil: %d\n
                Requirement: %d\n",
               cardList[i].name, cardList[i].health, cardList[i].attack, cardList[i].sigil, cardList[i].requirement);
    }
 
    return 0;
}*/

//====================================================DISPLAY FUNCTIONS=========================================

void displayScale(int score, int maxScore)
{
    if (score >  maxScore) score =  maxScore;
    if (score < -maxScore) score = -maxScore;
    int pos = BAR + score * BAR / maxScore;

    printf("OPPONENT [");
    for (int i = 0; i <= 2 * BAR; i++)
        putchar(i == pos ? 'O' : i == BAR ? '|' : '-');
    printf("] YOU (%+d)\n", score);
}

void printBoard(Mcard *board[8])
{
    for (int r = 4; r >= 0; r -= 4) {                 /* top row first */
        for (int line = 0; line < 8; line++) {
            for (int i = r; i < r + 4; i++) {
                Mcard *c = board[i];
                char s[64] = "";

                if (line == 0 || line == 7) {         /* top/bottom edge */
                    printf("+%.*s+ ", W, "--------------------");
                    continue;
                }
                if (line == 1)       snprintf(s, sizeof s, "%d", i + 1);
                else if (!c)         { if (line == 3) snprintf(s, sizeof s, "  empty"); }
                else if (line == 2)  snprintf(s, sizeof s, "%s", c->name);
                else if (line == 3)  snprintf(s, sizeof s, "ATK %d", c->attack);
                else if (line == 4)  snprintf(s, sizeof s, "HP  %d", c->health);
                else if (line == 5)  snprintf(s, sizeof s, "SIG %d", c->sigil);
                else if (line == 6)  snprintf(s, sizeof s, "REQ %d", c->requirement);

                printf("|%-*.*s| ", W, W, s);
            }
            putchar('\n');
        }
    }
}
