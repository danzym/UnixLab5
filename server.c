#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

typedef int (*func_t)(int);

int f(int x) {
    return x % 2;
}

int g(int x) {
    return (x + 1) % 2;
}

void run_server(int port, func_t func) {
    int server_fd, client_sock, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    int buffer = 0;

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

    if (listen(server_fd, 1) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    if ((client_sock = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    while (1) {
        valread = read(client_sock, &buffer, sizeof(buffer));
        if (valread > 0) {
            int result = func(buffer);
            send(client_sock, &result, sizeof(result), 0);
        }
    }
}

int main(int argc, char const *argv[]) {
    int port = atoi(argv[1]);
    char func_id = argv[2][0];

    if (func_id == 'f') {
        run_server(port, f);
    } else if (func_id == 'g') {
        run_server(port, g);
    } else {
        fprintf(stderr, "Unknown function identifier.\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}
