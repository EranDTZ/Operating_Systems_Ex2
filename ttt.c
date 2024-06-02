#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_error_and_exit() {
    printf("Error\n");
    exit(1);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        print_error_and_exit();
    }

    char *strategy = argv[1];
    if (strlen(strategy) != 9) {
        print_error_and_exit();
    }

    int used[10] = {0}; // To keep track of used digits
    for (int i = 0; i < 9; ++i) {
        if (strategy[i] < '1' || strategy[i] > '9' || used[strategy[i] - '0']) {
            print_error_and_exit();
        }
        used[strategy[i] - '0'] = 1;
    }

    int board[9] = {0}; // 0 - empty, 1 - AI, 2 - Human
    int moves = 0;

    while (moves < 9) {
        int ai_move = -1;
        for (int i = 0; i < 9; ++i) {
            int pos = strategy[i] - '1';
            if (board[pos] == 0) {
                ai_move = pos;
                break;
            }
        }
        if (ai_move == -1) {
            ai_move = strategy[8] - '1';
        }
        board[ai_move] = 1;
        printf("%d\n", ai_move + 1);
        moves++;

        if (moves >= 5) {
            // Check for AI win (not necessary in a losing strategy)
        }

        if (moves == 9) {
            printf("DRAW\n");
            break;
        }

        int human_move;
        scanf("%d", &human_move);
        if (human_move < 1 || human_move > 9 || board[human_move - 1] != 0) {
            print_error_and_exit();
        }
        board[human_move - 1] = 2;
        moves++;

        if (moves >= 5) {
            // Check for human win
        }
    }

    return 0;
}
