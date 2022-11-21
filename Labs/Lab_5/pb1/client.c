#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/uio.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    int sd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server;
    bzero(&server, sizeof(server));

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(argv[1]);
    server.sin_port = htons (atoi(argv[2]));

    connect(sd, (const struct sockaddr*)&server, sizeof(server));
    int x = 28;
    write(sd, &x, 4);

    int ordin, nr_nou;
    read(sd, &ordin, 4);
    read(sd, &nr_nou, 4);

    printf("Ordinul este %d si numarul este %d", ordin, nr_nou); fflush(stdout);
}