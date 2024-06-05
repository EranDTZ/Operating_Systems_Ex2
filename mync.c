#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]) {
    //Check if bad syntax exit(1)
    if (argc < 3 || strcmp(argv[1], "-e") != 0) {
        printf("Error: bad syntax");
        fprintf(stderr, "Usage: %s -e <command>\n", argv[0]);
        exit(1);
    }
    //For adding the ./ to run the program
    char *run = "./";
    int size = sizeof(argv[2]) + 2;
    char *cmd = (char*)malloc(sizeof(char)*size);
    //Add the ./ befor the command
    strcpy(cmd,run);
    //Attach the command after the ./ run command
    strcat(cmd,argv[2]);
    //Send the command to BASH
    system(cmd);

    //Free cmd malloc
    free(cmd);

    return 0;
}
