// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <unistd.h>
// #include <netinet/in.h>
// #include <sys/types.h>
// #include <sys/socket.h>
// #include <arpa/inet.h>
// #include <signal.h>
// #include <errno.h>

// #define BUFFER_SIZE 1024

// void handle_error(const char *msg) {
//     perror(msg);
//     exit(EXIT_FAILURE);
// }

// // Function to create a TCP server
// int create_tcp_server(int port) {
//     int server_fd;
//     struct sockaddr_in address;
//     int opt = 1;

//     // Create socket file descriptor
//     if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
//         handle_error("socket failed");
//     }

//     // Attach socket to the port
//     if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
//         handle_error("setsockopt failed");
//     }
//     address.sin_family = AF_INET;
//     address.sin_addr.s_addr = INADDR_ANY;
//     address.sin_port = htons(port);

//     // Bind the socket to the network address and port
//     if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
//         handle_error("bind failed");
//     }

//     // Listen for incoming connections
//     if (listen(server_fd, 3) < 0) {
//         handle_error("listen failed");
//     }

//     return server_fd;
// }

// // Function to create a TCP client and connect to a server
// int create_tcp_client(const char *hostname, int port) {
//     int client_fd;
//     struct sockaddr_in serv_addr;

//     if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
//         handle_error("Socket creation error");
//     }

//     serv_addr.sin_family = AF_INET;
//     serv_addr.sin_port = htons(port);

//     // Convert IPv4 and IPv6 addresses from text to binary form
//     if (inet_pton(AF_INET, hostname, &serv_addr.sin_addr) <= 0) {
//         handle_error("Invalid address/ Address not supported");
//     }

//     if (connect(client_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
//         handle_error("Connection failed");
//     }

//     return client_fd;
// }

// void run_ttt(int input_fd, int output_fd, int dup_fd, char *strategy) {
//     // Redirect stdin and stdout
//     if (dup2(input_fd, STDIN_FILENO) == -1) {
//         handle_error("dup2 input_fd");
//     }

//     if (dup_fd != -1) {
//         // Create a pipe to duplicate output
//         int pipe_fd[2];
//         if (pipe(pipe_fd) == -1) {
//             handle_error("pipe failed");
//         }

//         if (fork() == 0) {
//             // Child process for duplicating output
//             close(pipe_fd[1]); // Close write end of pipe
//             char buffer[BUFFER_SIZE];
//             ssize_t nread;
//             while ((nread = read(pipe_fd[0], buffer, BUFFER_SIZE)) > 0) {
//                 if (write(output_fd, buffer, nread) == -1) {
//                     handle_error("write to output_fd failed");
//                 }
//                 if (write(dup_fd, buffer, nread) == -1) {
//                     handle_error("write to dup_fd failed");
//                 }
//             }
//             close(pipe_fd[0]);
//             exit(0);
//         } else {
//             // Parent process
//             close(pipe_fd[0]); // Close read end of pipe
//             if (dup2(pipe_fd[1], STDOUT_FILENO) == -1) {
//                 handle_error("dup2 pipe_fd[1]");
//             }
//             close(pipe_fd[1]);
//         }
//     } else {
//         if (dup2(output_fd, STDOUT_FILENO) == -1) {
//             handle_error("dup2 output_fd");
//         }
//     }

//     // Execute the ttt program
//     execlp("./ttt", "ttt", strategy, (char *)NULL);
//     handle_error("execlp ttt failed");
// }

// int main(int argc, char *argv[]) {
//     int opt;
//     int server_mode = 0, server_o_mode = 0, both_mode = 0;
//     int input_port = 0;
//     char *exec_command = NULL;
//     char *strategy = argv[3];
//     strategy[9] = '\0';

//     while ((opt = getopt(argc, argv, "e:i:o:b:")) != -1) {
//         switch (opt) {
//             case 'e':
//                 exec_command = optarg;
//                 break;
//             case 'i':
//                 server_mode = 1;
//                 input_port = atoi(optarg + 4); // Extract port from TCPS#####
//                 break;
//             case 'o':
//                 server_o_mode = 1;
//                 input_port = atoi(optarg + 4); // Extract port from TCPS#####
//                 break;
//             case 'b':
//                 both_mode = 1;
//                 input_port = atoi(optarg + 4); // Extract port from TCPS#####
//                 break;
//             default:
//                 fprintf(stderr, "Usage: %s -e <command> [-i TCPSport] [-o TCPSport] [-b TCPSport]\n", argv[0]);
//                 exit(EXIT_FAILURE);
//         }
//     }

//     if (!exec_command) {
//         fprintf(stderr, "Execution command is required\n");
//         exit(EXIT_FAILURE);
//     }

//     // strategy = exec_command;

//     int server_fd = -1, new_socket = -1;
//     pid_t pid;

//     if (server_mode || server_o_mode || both_mode) {
//         server_fd = create_tcp_server(input_port);

//         // Wait for a client to connect
//         if ((new_socket = accept(server_fd, NULL, NULL)) < 0) {
//             handle_error("accept failed");
//         }
//     }

//     if ((pid = fork()) == 0) {
//         // Child process
//         if (server_mode && !both_mode) {
//             run_ttt(new_socket, STDOUT_FILENO, -1, strategy);
//         } else if (server_o_mode && !both_mode) {
//             run_ttt(new_socket, new_socket, -1, strategy);
//         } else if (both_mode) {
//             run_ttt(new_socket, STDOUT_FILENO, new_socket, strategy);
//         } else {
//             // Default execution without redirection
//             execlp("./ttt", "ttt", strategy, (char *)NULL);
//             handle_error("execlp ttt failed");
//         }
//     } else if (pid > 0) {
//         // Parent process: Wait for child to complete
//         int status;
//         waitpid(pid, &status, 0);

//         // Close sockets
//         if (server_fd != -1) close(server_fd);
//         if (new_socket != -1) close(new_socket);

//         // Check child exit status
//         if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
//             fprintf(stderr, "Child process exited with status %d\n", WEXITSTATUS(status));
//         }
//     } else {
//         handle_error("fork failed");
//     }

//     return 0;
// }

// /*To get input from client to server and print output to server:
//  terminal server: ./mync -e “ttt 123456789” -i TCPS4450
//  terminal client: nc localhost 4450
// */

// /*To get input from client to server and print output to client:
// terminal server: ./mync -e “ttt 123456789” -o TCPS4450
// terminal client: nc localhost 4450
// */

// /*To get input from client to server and print output to server & client:
// terminal server: ./mync -e “ttt 123456789” -b TCPS4450
// terminal client: nc localhost 4450
// */

// /*To get input from client to server at port (E.g. 4050) and print output to client at anater port (E.g. 4455):
// terminal server: ./mync -e “ttt 123456789” -i TCPS4050 -o TCPClocalhost,4455
// terminal client input: nc localhost 4050
// terminal client output: nc localhost 4455
// */


/*--------------------------------------------------------------------------------------------------------------*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>
#include <errno.h>

#define BUFFER_SIZE 1024

void handle_error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

// Function to create a TCP server
int create_tcp_server(int port) {
    int server_fd;
    struct sockaddr_in address;
    int opt = 1;

    // Create socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        handle_error("socket failed");
    }

    // Attach socket to the port
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        handle_error("setsockopt failed");
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    // Bind the socket to the network address and port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        handle_error("bind failed");
    }

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0) {
        handle_error("listen failed");
    }

    return server_fd;
}

// Function to create a TCP client and connect to a server
int create_tcp_client(const char *hostname, int port) {
    int client_fd;
    struct sockaddr_in serv_addr;

    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        handle_error("Socket creation error");
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, hostname, &serv_addr.sin_addr) <= 0) {
        handle_error("Invalid address/ Address not supported");
    }

    if (connect(client_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        handle_error("Connection failed");
    }

    return client_fd;
}

// Function to create a UDP server
int create_udp_server(int port) {
    int server_fd;
    struct sockaddr_in address;

    // Create socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_DGRAM, 0)) == 0) {
        handle_error("socket failed");
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    // Bind the socket to the network address and port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        handle_error("bind failed");
    }

    return server_fd;
}

// Function to create a UDP client
int create_udp_client(const char *hostname, int port, struct sockaddr_in *serv_addr) {
    int client_fd;

    if ((client_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        handle_error("Socket creation error");
    }

    serv_addr->sin_family = AF_INET;
    serv_addr->sin_port = htons(port);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, hostname, &serv_addr->sin_addr) <= 0) {
        handle_error("Invalid address/ Address not supported");
    }

    return client_fd;
}

void run_ttt(int input_fd, int output_fd, int dup_fd, char *strategy) {
    // Redirect stdin and stdout
    if (dup2(input_fd, STDIN_FILENO) == -1) {
        handle_error("dup2 input_fd");
    }

    if (dup_fd != -1) {
        // Create a pipe to duplicate output
        int pipe_fd[2];
        if (pipe(pipe_fd) == -1) {
            handle_error("pipe failed");
        }

        if (fork() == 0) {
            // Child process for duplicating output
            close(pipe_fd[1]); // Close write end of pipe
            char buffer[BUFFER_SIZE];
            ssize_t nread;
            while ((nread = read(pipe_fd[0], buffer, BUFFER_SIZE)) > 0) {
                if (write(output_fd, buffer, nread) == -1) {
                    handle_error("write to output_fd failed");
                }
                if (write(dup_fd, buffer, nread) == -1) {
                    handle_error("write to dup_fd failed");
                }
            }
            close(pipe_fd[0]);
            exit(0);
        } else {
            // Parent process
            close(pipe_fd[0]); // Close read end of pipe
            if (dup2(pipe_fd[1], STDOUT_FILENO) == -1) {
                handle_error("dup2 pipe_fd[1]");
            }
            close(pipe_fd[1]);
        }
    } else {
        if (dup2(output_fd, STDOUT_FILENO) == -1) {
            handle_error("dup2 output_fd");
        }
    }

    // Execute the ttt program
    execlp("./ttt", "ttt", strategy, (char *)NULL);
    handle_error("execlp ttt failed");
}

void handle_alarm(int sig) {
    exit(0);
}

int main(int argc, char *argv[]) {
    int opt;
    int server_mode = 0, server_o_mode = 0, both_mode = 0;
    int udp_mode = 0;
    int input_port = 0;
    int timeout = 0;
    char *exec_command = NULL;
    char *strategy = argv[3];
    strategy[9] = '\0';

    while ((opt = getopt(argc, argv, "e:i:o:b:t:")) != -1) {
        switch (opt) {
            case 'e':
                exec_command = optarg;
                break;
            case 'i':
                server_mode = 1;
                if (strncmp(optarg, "UDPS", 4) == 0) {
                    udp_mode = 1;
                    input_port = atoi(optarg + 4); // Extract port from UDPS#####
                } else if (strncmp(optarg, "TCPS", 4) == 0) {
                    input_port = atoi(optarg + 4); // Extract port from TCPS#####
                }
                break;
            case 'o':
                server_o_mode = 1;
                // Handle UDP client output here if needed
                break;
            case 'b':
                both_mode = 1;
                // Handle both modes if needed
                break;
            case 't':
                timeout = atoi(optarg);
                break;
            default:
                fprintf(stderr, "Usage: %s -e <command> [-i TCPSport | UDPSport] [-o TCPSport | UDPC<hostname,port>] [-b TCPSport | UDPSport] [-t timeout]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (!exec_command) {
        fprintf(stderr, "Execution command is required\n");
        exit(EXIT_FAILURE);
    }

    // Set up alarm for timeout
    if (timeout > 0) {
        signal(SIGALRM, handle_alarm);
        alarm(timeout);
    }

    int server_fd = -1, new_socket = -1;
    struct sockaddr_in udp_client_addr;
    pid_t pid;

    if (server_mode || server_o_mode || both_mode) {
        if (udp_mode) {
            server_fd = create_udp_server(input_port);
        } else {
            server_fd = create_tcp_server(input_port);

            // Wait for a client to connect
            if ((new_socket = accept(server_fd, NULL, NULL)) < 0) {
                handle_error("accept failed");
            }
        }
    }

    if ((pid = fork()) == 0) {
        // Child process
        if (udp_mode) {
            char buffer[BUFFER_SIZE];
            ssize_t nread;
            while ((nread = recvfrom(server_fd, buffer, BUFFER_SIZE, 0, NULL, NULL)) > 0) {
                if (write(STDOUT_FILENO, buffer, nread) == -1) {
                    handle_error("write to stdout failed");
                }
            }
        } else {
            if (server_mode && !both_mode) {
                run_ttt(new_socket, STDOUT_FILENO, -1, strategy);
            } else if (server_o_mode && !both_mode) {
                run_ttt(new_socket, new_socket, -1, strategy);
            } else if (both_mode) {
                run_ttt(new_socket, STDOUT_FILENO, new_socket, strategy);
            } else {
                // Default execution without redirection
                execlp("./ttt", "ttt", strategy, (char *)NULL);
                handle_error("execlp ttt failed");
            }
        }
    } else if (pid > 0) {
        // Parent process: Wait for child to complete
        int status;
        waitpid(pid, &status, 0);

        // Close sockets
        if (server_fd != -1) close(server_fd);
        if (new_socket != -1) close(new_socket);

        // Check child exit status
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
            fprintf(stderr, "Child process exited with status %d\n", WEXITSTATUS(status));
        }
    } else {
        handle_error("fork failed");
    }

    return 0;
}