# MyNetCat and Tic-Tac-Toe AI

This project contains the implementation of a custom `netcat`-like program (`mync`) that supports TCP, UDP, and Unix Domain Sockets for communication, as well as a simple Tic-Tac-Toe AI (`ttt`). The `mync` program is divided into three separate implementations to handle different types of sockets: TCP, UDP, and Unix Domain Sockets.

## Files

- `ttt.c`: Contains the implementation of the Tic-Tac-Toe AI.
- `mync_tcp.c`: Contains the implementation of `mync` using TCP sockets.
- `mync_udp.c`: Contains the implementation of `mync` using UDP sockets.
- `mync_unix.c`: Contains the implementation of `mync` using Unix Domain Sockets.
- `Makefile`: Makefile to compile the project.

## Compilation

To compile the project, run:
```sh
make
```

This will produce four executables: `ttt`, `mync_tcp`, `mync_udp`, and `mync_unix`.

## Usage

### Tic-Tac-Toe AI

Run the Tic-Tac-Toe AI with:
```sh
./ttt 123456789
```

### TCP Server and Client

#### TCP Server
```sh
./mync_tcp -i TCPS4050
```

#### TCP Client
```sh
./mync_tcp -o TCPClocalhost4050
```

### UDP Server and Client

#### UDP Server
```sh
./mync_udp -e "ttt 123456789" -i UDPS4050 -t 10
```

#### UDP Client
```sh
./mync_udp -o UDPC127.0.0.1:4050
```

### Unix Domain Socket Server and Client

#### Unix Domain Socket Server
```sh
./mync_unix -e "ttt 123456789" -i UDSSD/tmp/mync_socket
```

#### Unix Domain Socket Client
```sh
./mync_unix -o UDSSC/tmp/mync_socket
```

## Description

### Part 3.5: mynetcat - Stage 2.5

The TCP server and client are implemented in `mync_tcp.c`. The server listens on a specified port and prints any received messages. The client connects to the server, reads a message from stdin, and sends it to the server.

### Part 3.6: mynetcat - Stage 3

The UDP server and client are implemented in `mync_udp.c`. The server listens on a specified port, executes a given command (e.g., the Tic-Tac-Toe AI), and has a timeout feature. The client sends a message read from stdin to the server.

### Part 3.7: mynetcat - Stage 4

The Unix Domain Socket server and client are implemented in `mync_unix.c`. The server listens on a specified Unix domain socket path, executes a given command, and the client connects to the server and sends a message read from stdin.

## Clean Up

To remove the compiled executables, run:
```sh
make clean
```

## Note

- Ensure that the paths specified for Unix domain sockets are valid and accessible.
- The Tic-Tac-Toe AI (`ttt`) is a simple placeholder that can be replaced with more complex logic if desired.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
```