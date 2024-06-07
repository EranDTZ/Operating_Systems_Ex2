#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/un.h>



/*----------------------------------------------------TCP---------------------------------------------------------------*/

int tcpServer(int port)
{
    struct sockaddr_in server;
    struct sockaddr_in client;
    socklen_t client_len = sizeof(client);

    memset(&server, 0, sizeof(server));
    memset(&client, 0, sizeof(client));
    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock < 0)
    {
        perror("socket(2)");
        return 1;
    }

    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    if (bind(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("bind(2)");
        close(sock);
        return 1;
    }

    if (listen(sock, 1) < 0)
    {
        perror("listen(2)");
        close(sock);
        return 1;
    }

    printf("Waiting for TCP connection...\n");
    int client_sock = accept(sock, (struct sockaddr *)&client, &client_len); // try to connect
    if (client_sock < 0)
    {
        perror("accept(2)");
        close(sock);
        return 1;
    }
    return client_sock;
}

// פונקציית לקוח
int tcpClient(int port, char *clientData)
{
    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    struct addrinfo hints, *res, *p;
    int status;
    char ipstr[INET_ADDRSTRLEN];

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // AF_INET or AF_INET6 to force version
    hints.ai_socktype = SOCK_STREAM;

    if ((status = getaddrinfo(clientData, NULL, &hints, &res)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return 2;
    }

    printf("IP addresses for %s:\n\n", clientData);

    for (p = res; p != NULL; p = p->ai_next)
    {
        void *addr;
        char *ipver;

        // get the pointer to the address itself
        struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
        addr = &(ipv4->sin_addr);
        ipver = "IPv4";

        // convert the IP to a string and print it:
        inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
        printf("  %s: %s\n", ipver, ipstr);
    }

    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock < 0)
    {
        perror("socket(2)");
        return 1;
    }

    if (inet_pton(AF_INET, ipstr, &server.sin_addr) <= 0)
    {
        perror("inet_pton(3)");
        close(sock);
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    printf("Connecting to Receiver...\n");
    int con = connect(sock, (struct sockaddr *)&server, sizeof(server));
    if (con < 0)
    {
        perror("connect(2)");
        close(sock);
        return 1;
    }

    freeaddrinfo(res); // free the linked list

    return sock;
}



/*----------------------------------------------------UDP---------------------------------------------------------------*/




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



/*----------------------------------------------------UNIX---------------------------------------------------------------*/


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