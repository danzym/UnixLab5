#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/select.h>
#include <sys/time.h>
#include <errno.h>

#define TIMEOUT_SEC 10

int connect_to_helper(int port) {
    int sock = 0;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }
    return sock;
}

int main(int argc, char const *argv[]) {
    int sock_f = connect_to_helper(8080);
    int sock_g = connect_to_helper(8081);
    int x = 123;  // Example input
    int result_f = 0, result_g = 0, read_f = 0, read_g = 0;
    fd_set readfds;
    struct timeval tv;

    send(sock_f, &x, sizeof(x), 0);
    send(sock_g, &x, sizeof(x), 0);

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(sock_f, &readfds);
        FD_SET(sock_g, &readfds);

        tv.tv_sec = TIMEOUT_SEC;
        tv.tv_usec = 0;

        int activity = select(FD_SETSIZE, &readfds, NULL, NULL, &tv);

        if ((activity < 0) && (errno != EINTR)) {
            printf("select error");
        }

        if (FD_ISSET(sock_f, &readfds)) {
            read_f = read(sock_f, &result_f, sizeof(result_f));
        }

        if (FD_ISSET(sock_g, &readfds)) {
            read_g = read(sock_g, &result_g, sizeof(result_g));
        }

        if (read_f > 0 || result_f != 0) {
            printf("Result: True\n");
            break;
        }

        if (read_g > 0 || result_g != 0) {
            printf("Result: True\n");
            break;
        }

        if (activity == 0) {
            printf("Timeout. No data received.\n");
            break;
        }
    }

    close(sock_f);
    close(sock_g);
    return 0;
}
