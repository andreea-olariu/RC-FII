#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <strings.h>

#define SIZE 101

int main(int argc, char* argv[]) {
    int sd;			
    struct sockaddr_in server;
    char path[SIZE];
    
    bzero(&server, sizeof(server));
    sd = socket(AF_INET, SOCK_STREAM, 0);

    int port = atoi(argv[2]);
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(argv[1]);
    server.sin_port = htons (port);

    connect(sd, (const struct sockaddr*) &server, sizeof(server));
    
    char msg[SIZE]; 
    read(sd, msg, SIZE);
    if(strcmp(msg, "Astept!") == 0) {
        printf("Dati o cale de fisier... "); fflush(stdout);
        scanf("%s", path);
        write(sd, path, SIZE);
    } else {
        printf(msg); fflush(stdout);
    }
    shutdown(sd, 2);
}