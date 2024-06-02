#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>

void start_unix_domain_socket_server(char *path, char *cmd) {
    int server_fd, new_socket;
    struct sockaddr_un address;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    address.sun_family = AF_UNIX;
    strcpy(address.sun_path, path);

    unlink(path);
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

    system(cmd);
    close(new_socket);
    unlink(path);
}

void start_unix_domain_socket_client(char *path) {
    int sock = 0;
    struct sockaddr_un serv_addr;
    char buffer[1024] = {0};

    if ((sock = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    serv_addr.sun_family = AF_UNIX;
    strcpy(serv_addr.sun_path, path);

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection Failed");
        exit(EXIT_FAILURE);
    }

    // Read a single message from stdin and send it
    if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        write(sock, buffer, strlen(buffer));
    }

    close(sock);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s -e <command> -i UDSSD<path> | -o UDSSC<path>\n", argv[0]);
        exit(1);
    }

    char *cmd = argv[2];
    char *flag = argv[3];
    char *param = argv[4];

    if (strncmp(flag, "-i", 2) == 0 && strncmp(param, "UDSSD", 5) == 0) {
        char *path = param + 5;
        start_unix_domain_socket_server(path, cmd);
    } else if (strncmp(flag, "-o", 2) == 0 && strncmp(param, "UDSSC", 5) == 0) {
        char *path = param + 5;
        start_unix_domain_socket_client(path);
    } else {
        fprintf(stderr, "Unsupported flag or parameter\n");
        exit(1);
    }

    return 0;
}
