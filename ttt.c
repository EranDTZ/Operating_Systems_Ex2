#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define N 3


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
            if (i != 8)
            {
                printf("_________\n");
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
            return 0;
        }
        
    }
    return 1; 
}

int main(int argc, char *argv[]) {

    if (argc != 2) {
        printf("Error: No strategy was provided.\n");
        exit(1);
    }

    char *strategy = argv[1];
    if (strlen(strategy) != 9) {
        printf("Error: Strategy must have nine digits.\n");
        exit(1);
    }


    for (int i = 0; i < 9; ++i) {
        for (int j = (i+1); j < 9; j++)
        {
            if (strategy[i] == strategy[j])
            {
                printf("Error: All nine digits must be different.\n");
                exit(1);
            }
            
        }
    }

    int board[9] = {0}; // empty = 0, AI = 1, Human = 2
    int moves = 0;
    int flag = -1;

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
        //Make AI_move on board
        board[AI_move] = 1;
        moves++;

        //Chack if AI wins for all combinitions.
        if (moves >= 5) {
            if ((board[0]==1 && board[1]==1 && board[2]==1) || (board[3]==1 && board[4]==1 && board[5]==1) || (board[6]==1 && board[7]==1 && board[8]==1))
            {
                flag = 1;
                break;
            }
            if ((board[0]==1 && board[3]==1 && board[6]==1) || (board[1]==1 && board[4]==1 && board[7]==1) || (board[2]==1 && board[5]==1 && board[8]==1))
            {
                flag = 1;
                break;
            }
            if ((board[0]==1 && board[4]==1 && board[8]==1) || (board[2]==1 && board[4]==1 && board[6]==1))
            {
                flag = 1;
                break;
            }
        }

        //If the board all full then its a DRAW
        if ((moves == 9) || (board_full(board))) {
            flag = 0;
            break;
        }
        
        //For every move print board for the player to see - AI move + which place if available
        printf("\n");
        print_board(board);

        int human_move;
        scanf("%d", &human_move);
        //Chack if human_move is llegal
        if (board[human_move - 1] != 0)
        {
            printf("Error: Illegal operation - The place is not available.\n");
            exit(1);
        }
        if (human_move < 1 || human_move > 9 || board[human_move - 1] != 0) {
            printf("Error: Illegal operation - You went out of bounds.\n");
            exit(1);
        }
        //Make human_move on board
        board[human_move - 1] = 2;
        moves++;
        //Chack if Human wins for all combinitions.
        if (moves > 5) {
            if ((board[0]==2 && board[1]==2 && board[2]==2) || (board[3]==2 && board[4]==2 && board[5]==2) || (board[6]==2 && board[7]==2 && board[8]==2))
            {
                flag = 2;
                break;
            }
            if ((board[0]==2 && board[3]==2 && board[6]==2) || (board[1]==2 && board[4]==2 && board[7]==2) || (board[2]==2 && board[5]==2 && board[8]==2))
            {
                flag = 2;
                break;
            }
            if ((board[0]==2 && board[4]==2 && board[8]==2) || (board[2]==2 && board[4]==2 && board[6]==2))
            {
                flag = 2;
                break;
            }
        }
    }
    //Print lest move board
    printf("\n");
    print_board(board);
    //Check & Print who won.
    if (flag == 0)
    {
        printf("DRAW\n");
    }
    if (flag == 1)
    {
        printf("AI WIN!\n");
    }
    if (flag == 2)
    {
        printf("YOU WIN!\n");
    }

    return 0;
}
