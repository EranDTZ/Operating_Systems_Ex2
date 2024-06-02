#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <signal.h>

void handle_timeout(int sig) {
    exit(0);
}

void start_udp_server(int port, char *cmd, int timeout) {
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t len;
    char buffer[1024];

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(port);

    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    signal(SIGALRM, handle_timeout);
    alarm(timeout);

    len = sizeof(cliaddr);
    while (1) {
        int n = recvfrom(sockfd, (char *)buffer, 1024, MSG_WAITALL, (struct sockaddr *) &cliaddr, &len);
        buffer[n] = '\0';
        system(cmd);
    }

    close(sockfd);
}

void start_udp_client(const char *host, int port) {
    int sockfd;
    struct sockaddr_in servaddr;
    char buffer[1024];

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = inet_addr(host);

    // Read a single message from stdin and send it
    if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        sendto(sockfd, buffer, strlen(buffer), 0, (const struct sockaddr *) &servaddr, sizeof(servaddr));
    }

    close(sockfd);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s -e <command> -i UDPS<PORT> -t <timeout> | -o UDPC<IP><PORT>\n", argv[0]);
        exit(1);
    }

    char *cmd = argv[2];
    char *flag = argv[3];
    char *param = argv[4];

    if (strncmp(flag, "-i", 2) == 0 && strncmp(param, "UDPS", 4) == 0) {
        int port = atoi(param + 4);
        int timeout = argc == 7 && strcmp(argv[5], "-t") == 0 ? atoi(argv[6]) : 10;
        start_udp_server(port, cmd, timeout);
    } else if (strncmp(flag, "-o", 2) == 0 && strncmp(param, "UDPC", 4) == 0) {
        char *ip = strtok(param + 4, ":");
        int port = atoi(strtok(NULL, ":"));
        start_udp_client(ip, port);
    } else {
        fprintf(stderr, "Unsupported flag or parameter\n");
        exit(1);
    }

    return 0;
}
