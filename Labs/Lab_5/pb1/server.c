#include <sys/socket.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/uio.h>
#include <unistd.h>
#include <netinet/in.h>

#define PORT 2024
#define SIZE 101
int main(int agrc, char* argv[]) {
    int sd;
    if(-1 == (sd = socket(AF_INET, SOCK_STREAM, 0))) {
        perror("[server]Eroare la socket()"); exit(1);
    }
    struct sockaddr_in server;
    bzero(&server, sizeof(server));
    struct sockaddr_in from;
    bzero(&from, sizeof(from));

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(PORT);

    if(-1 == bind(sd, (const struct sockaddr *)&server, sizeof(server))) {
        perror("[server]Eroare la bind()"); exit(1);
    }

    if(-1 == listen(sd, 5)) {
        perror("[server]Eroare la listenq()"); exit(1);
    }

    int ordin = 0, numar, client, len;
    while(1) {
        ordin += 1;
        len = sizeof(from);
        if(-1 == (client = accept(sd, (struct sockaddr*) &from, &len))) {
            perror("[server]Eroare la accept()"); exit(1);
        }
        read(client,&numar, 4);
        numar += 1;

        write(client, &ordin, 4);
        write(client, &numar, 4);
        close(client);
    }
}