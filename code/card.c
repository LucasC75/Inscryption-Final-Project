#include "player_card.h"

//Card creation. Read from the .txt file and copy it into an array 

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
 
