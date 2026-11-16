#include <stdio.h>

/* Discard whatever is left on the current input line */
void clear_input(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int main(void) {
    int selec;
    int difficulty = 0;
    int loop = 1;

    printf("WELCOME TO INSCRYPTION\n----------------------\n\n");

    while (loop == 1) {
        printf("1.Play 2.How to play 3.Quit\n> ");

        if (scanf("%d", &selec) != 1) {   /* user typed a non-number */
            clear_input();
            printf("Not an option\n");
            continue;
        }
        printf("----------------------\n");

        switch (selec) {
        case 1:
            do {
                printf("1.easy 2.medium 3.hard\nPlease select a difficulty: ");
                if (scanf("%d", &difficulty) != 1) {
                    clear_input();
                    difficulty = 0;
                }
            } while (difficulty < 1 || difficulty > 3);
            loop = 0;
            break;
        case 2:
            printf("How to play: ...\n");
            printf("----------------------\n");
            break;          
        case 3:
            printf("Goodbye!\n");
            loop = 0;
            break;
        default:
            printf("Not an option\n");
            break;          
        }
    }

    return 0;
}
