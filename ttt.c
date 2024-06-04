#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define N 3

void print_error_and_exit() {
    printf("Error\n");
    exit(1);
}

void print_board(int board[9]){
    for (int i = 0; i < 9; i++)
    {
        if (i == 2 || i == 5 || i == 8)
        {
            if (board[i] == 0)
            {
                printf("  ");
                printf("\n");
            }
            if (board[i] == 1)
            {
                printf("X ");
                printf("\n");
            }
            if (board[i] == 2)
            {
                printf("O ");
                printf("\n");
            }
        }
        else {
            if (board[i] == 0) printf("  | ");
            if (board[i] == 1) printf("X | ");
            if (board[i] == 2) printf("O | ");
        }
    }
}

int board_full(int board[9]){
    for (int i = 0; i < 9; i++)
    {
        if (board[i]==0)
        {
            return 1;
        }
        
    }
    return 0; 
}

int main(int argc, char *argv[]) {

    if (argc != 2) {
        print_error_and_exit();
    }

    char *strategy = argv[1];
    if (strlen(strategy) != 9) {
        print_error_and_exit();
    }


    for (int i = 0; i < 9; ++i) {
        if (strategy[i] < '1' || strategy[i] > '9') {
            print_error_and_exit();
        }
    }

    int board[9] = {0}; // empty = 0, AI = 1, Human = 2
    int moves = 0;

    while (moves < 9) {
        int AI_move = -1;
        for (int i = 0; i < 9; ++i) {
            int pos = strategy[i] - '1';
            //Chack if strategy MSD is empty - if so AI_move = pos - else AI_move = LSD
            if (board[pos] == 0) {
                AI_move = pos;
                break;
            }
        }

        //If the board almost full - LSD
        if (AI_move == -1) {
            AI_move = strategy[8] - '1';
        }


        board[AI_move] = 1;
        moves++;

        //Chack if AI wins for all combinitions.
        if (moves >= 5) {
            if (board[0]==1&&board[1]==1&&board[2]==1 || board[3]==1&&board[4]==1&&board[5]==1 || board[6]==1&&board[7]==1&&board[8]==1)
            {
                printf("AI win!\n");
                break;
            }
            if (board[0]==1&&board[3]==1&&board[6]==1 || board[1]==1&&board[4]==1&&board[7]==1 || board[2]==1&&board[5]==1&&board[8]==1)
            {
                printf("AI win!\n");
                break;
            }
            if (board[0]==1&&board[4]==1&&board[8]==1 || board[2]==1&&board[4]==1&&board[6]==1)
            {
                printf("AI win!\n");
                break;
            }
        }

        //If the board all full then its a DRAW
        if (moves == 9 || !board_full) {
            printf("DRAW\n");
            break;
        }
        
        print_board(board);

        int human_move;
        scanf("%d", &human_move);
        if (human_move < 1 || human_move > 9 || board[human_move - 1] != 0) {
            print_error_and_exit();
        }
        board[human_move - 1] = 2;
        moves++;

        //Chack if Human wins for all combinitions.
        if (moves > 5) {
            if (board[0]==2&&board[1]==2&&board[2]==2 || board[3]==2&&board[4]==2&&board[5]==2 || board[6]==2&&board[7]==2&&board[8]==2)
            {
                printf("YOU win!\n");
                break;
            }
            if (board[0]==2&&board[3]==2&&board[6]==2 || board[1]==2&&board[4]==2&&board[7]==2 || board[2]==2&&board[5]==2&&board[8]==2)
            {
                printf("YOU win!\n");
                break;
            }
            if (board[0]==2&&board[4]==2&&board[8]==2 || board[2]==2&&board[4]==2&&board[6]==2)
            {
                printf("YOU win!\n");
                break;
            }
        }
    }

    print_board(board);

    return 0;
}
