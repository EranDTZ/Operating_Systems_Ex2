#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

void handle_tcp_connection(int sockfd) {
    char buffer[1024];
    int n;

    // Read a single message from the connection and print it
    if ((n = read(sockfd, buffer, 1024)) > 0) {
        buffer[n] = '\0';
        printf("Received: %s\n", buffer);
    }
}

void start_tcp_server(int port) {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    handle_tcp_connection(new_socket);
    close(new_socket);
    close(server_fd);
}

void start_tcp_client(const char *host, int port) {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, host, &serv_addr.sin_addr) <= 0) {
        perror("Invalid address / Address not supported");
        exit(EXIT_FAILURE);
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection Failed");
        exit(EXIT_FAILURE);
    }

    // Read a single message from stdin and send it
    if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        send(sock, buffer, strlen(buffer), 0);
    }

    close(sock);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s -i TCPS<PORT> | -o TCPClocalhost<PORT>\n", argv[0]);
        exit(1);
    }

    char *flag = argv[1];
    char *param = argv[2];

    if (strncmp(flag, "-i", 2) == 0 && strncmp(param, "TCPS", 4) == 0) {
        int port = atoi(param + 4);
        start_tcp_server(port);
    } else if (strncmp(flag, "-o", 2) == 0 && strncmp(param, "TCPClocalhost", 12) == 0) {
        int port = atoi(param + 12);
        start_tcp_client("127.0.0.1", port);
    } else {
        fprintf(stderr, "Unsupported flag or parameter\n");
        exit(1);
    }

    return 0;
}
