/* main.c - program setup, main menu and the game loop */
#include "player.h"
#include <time.h>

static void showRules(void)
{
    printf("\nHOW TO PLAY\n"
           "-----------\n"
           "- The board has 4 lanes. Your cards are on the bottom row,\n"
           "  the opponent's cards are on the top row.\n"
           "- Start each turn by drawing from your deck or the squirrel deck.\n"
           "- Cards cost blood. Pay it by sacrificing your cards on the board.\n"
           "  Squirrels are free to play, so they make great sacrifices.\n"
           "- Ring the bell to end your turn. Each card attacks the card across\n"
           "  from it. If that space is empty, the damage goes on the scale.\n"
           "- When the scale tips 5 to one side, that side loses a life.\n"
           "  You have 2 lives. The opponent has 1 (2 on Hard).\n"
           "- If both of your decks run out, you take 1 damage each turn.\n"
           "- Pick \"View all cards\" to see every card and what its sigil does.\n");
}

static void playGame(void)
{
    Player player;
    int difficulty = readNumber("\nDifficulty: 1) Easy  2) Medium  3) Hard (boss with 2 lives)\n> ", 1, 3);

    setupPlayer(&player);   /* player setup: builds your deck and squirrel deck */
    printf("\nYour deck:\n");
    showDeck(&player.deck);

    battle(&player, difficulty);
    freePlayer(&player);    /* frees whatever is left in the decks */
}

int main(void)
{
    srand((unsigned) time(NULL));

    if (loadCards("card_definitions.txt") <= 0 || findCard("Squirrel") == NULL) {
        printf("Could not load the cards. Make sure card_definitions.txt is in\n"
               "the folder you run the game from.\n");
        return 1;
    }

    printf("WELCOME TO INSCRYPTION\n----------------------\n");
    while (1) {
        int choice = readNumber("\n1) Play  2) How to play  3) View all cards  4) Quit\n> ", 1, 4);
        if (choice == 1)
            playGame();
        else if (choice == 2)
            showRules();
        else if (choice == 3)
            printAllCards();
        else
            break;
    }

    printf("Goodbye!\n");
    return 0;
}
