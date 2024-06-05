#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>




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



/*---------------------------------------------------------------------------------------------------------------------*/




int main(int argc, char *argv[]){
    // Check if bad syntax, exit(1)
    if (argc < 5 || strcmp(argv[1], "-e") != 0) {
        printf("Error: bad syntax\n");
        fprintf(stderr, "Usage: %s -e <command> -i/-o/-b TCPS<PORT> | TCPC<IP,PORT>\n", argv[0]);
        exit(1);
    }

    //For adding the ./ to run the program
    char *run = "./";
    int size = (sizeof(argv[2]) + sizeof(argv[3])) + 3;
    char *cmd = (char*)malloc(sizeof(char)*size);
    //Add the ./ befor the command
    strcpy(cmd,run);
    //Attach the command after the ./ run command
    strcat(cmd,argv[2]);
    strcat(cmd," ");
    strcat(cmd,argv[3]);

    char *flag = argv[4];
    char *param = argv[5];
    int sock;

    // Execute the command with input/output redirection
    if (strncmp(flag, "-i", 2) == 0 && strncmp(param, "TCPS", 4) == 0) {
        int port = atoi(param + 4);
        sock = tcpServer(port);
        if (sock < 0)
        {
            fprintf(stderr, "Failed to start server on port %d\n", port);
            exit(1);
        }
    }
    else if (strncmp(flag, "-o", 2) == 0 && strncmp(param, "TCPC", 4) == 0) {
        char *host = strtok(param + 4, ",");
        int port = atoi(strtok(NULL, ","));
        sock = tcpClient(port, host);
        if (sock < 0)
        {
            fprintf(stderr, "Failed to connect to server at %s:%d\n", host, port);
            exit(1);
        }
    } 
    else {
        fprintf(stderr, "Unsupported flag or parameter\n");
        exit(1);
    }

    close(sock);
    return 0;
}



// ./mync_tcp -e “ttt 123456789” -i TCPS4455
// ./mync_tcp -e “ttt 123456789” -o TCPC127.0.0.1,4455




