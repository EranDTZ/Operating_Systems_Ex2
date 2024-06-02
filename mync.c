#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc < 3 || strcmp(argv[1], "-e") != 0) {
        fprintf(stderr, "Usage: %s -e <command>\n", argv[0]);
        exit(1);
    }

    char *cmd = argv[2];
    for (int i = 3; i < argc; ++i) {
        strcat(cmd, " ");
        strcat(cmd, argv[i]);
    }

    system(cmd);

    return 0;
}
