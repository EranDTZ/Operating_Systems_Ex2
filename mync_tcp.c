#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

// Function to handle TCP server input redirection
void handle_tcp_input(int sockfd, FILE *output) {
    char buffer[1024];
    int n;

    // Read from the connection and write to output
    while ((n = read(sockfd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[n] = '\0';
        fprintf(output, "%s", buffer);
        fflush(output);
    }
}

// Function to handle TCP client output redirection
void handle_tcp_output(int sockfd, FILE *input) {
    char buffer[1024];

    // Read from input and write to the connection
    while (fgets(buffer, sizeof(buffer), input) != NULL) {
        send(sockfd, buffer, strlen(buffer), 0);
    }
}

void start_tcp_server(int port, FILE *input, FILE *output) {
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

    // Handle input and output redirection
    if (input) {
        dup2(new_socket, STDIN_FILENO);
        handle_tcp_input(new_socket, output);
    }
    if (output) {
        dup2(new_socket, STDOUT_FILENO);
        handle_tcp_output(new_socket, input);
    }

    close(new_socket);
    close(server_fd);
}

void start_tcp_client(const char *host, int port, FILE *input, FILE *output) {
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

    // Handle input and output redirection
    if (input) {
        dup2(sock, STDIN_FILENO);
        handle_tcp_input(sock, output);
    }
    if (output) {
        dup2(sock, STDOUT_FILENO);
        handle_tcp_output(sock, input);
    }

    close(sock);
}

int main(int argc, char *argv[]) {
    // Check if bad syntax, exit(1)
    if (argc < 5 || strcmp(argv[1], "-e") != 0) {
        printf("Error: bad syntax\n");
        fprintf(stderr, "Usage: %s -e <command> -i/-o/-b TCPS<PORT> | TCPC<IP,PORT>\n", argv[0]);
        exit(1);
    }

    //For adding the ./ to run the program
    char *run = "./";
    int size = (sizeof(argv[2]) + sizeof(argv[3])) + 2;
    char *cmd = (char*)malloc(sizeof(char)*size);
    //Add the ./ befor the command
    strcpy(cmd,run);
    //Attach the command after the ./ run command
    strcat(cmd,argv[2]);
    strcat(cmd,argv[3]);

    char *flag = argv[4];
    char *param = argv[5];

    // Execute the command with input/output redirection
    if (strncmp(flag, "-i", 2) == 0 && strncmp(param, "TCPS", 4) == 0) {
        int port = atoi(param + 4);
        start_tcp_server(port, NULL, stdout);
    } else if (strncmp(flag, "-o", 2) == 0 && strncmp(param, "TCPC", 4) == 0) {
        char *host = strtok(param + 4, ",");
        int port = atoi(strtok(NULL, ","));
        start_tcp_client(host, port, stdin, NULL);
    } else if (strncmp(flag, "-b", 2) == 0 && strncmp(param, "TCPS", 4) == 0) {
        int port = atoi(param + 4);
        start_tcp_server(port, stdin, stdout);
    } else {
        fprintf(stderr, "Unsupported flag or parameter\n");
        exit(1);
    }

    return 0;
}
