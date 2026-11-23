#include <time.h>
#include "card.h"
#include "player.h"
#include "battle.h"

static void howToPlay(void)
{
    printf("HOW TO PLAY\n"
           "- Each turn, draw from your main deck or your squirrel deck.\n"
           "- Play cards into your 4 lanes. Cards cost blood: sacrifice cards\n"
           "  already on your board to pay (each gives 1 blood, Black Goat gives 3).\n"
           "  Squirrels are free, so use them as sacrifices.\n"
           "- Ring the bell to end your turn. Each of your cards attacks the\n"
           "  opponent card in front of it. If the lane is empty, the damage\n"
           "  goes to the scale.\n"
           "- Tip the scale to +%d to win. If it reaches -%d, you lose.\n"
           "- Option 3 in the main menu lists every card and its sigil.\n",
           MAX_SCORE, MAX_SCORE);
}

int main(void)
{
    int running = 1;

    srand((unsigned)time(NULL));

    if (loadCards("card_definitions.txt") <= 0 &&
        loadCards("code/card_definitions.txt") <= 0) {
        printf("Error: could not load card_definitions.txt\n");
        return 1;
    }

    printf("WELCOME TO INSCRYPTION\n----------------------\n");

    while (running) {
        int choice = readInt("\n1.Play  2.How to play  3.Card list  4.Quit\n> ", 1, 4);

        switch (choice) {
        case 1: {
            int difficulty = readInt("1.Easy  2.Medium  3.Hard\nSelect a difficulty: ", 1, 3);
            playBattle(difficulty);
            break;
        }
        case 2:
            howToPlay();
            break;
        case 3:
            printCardList();
            break;
        case 4:
            printf("Goodbye!\n");
            running = 0;
            break;
        }
    }

    return 0;
}
