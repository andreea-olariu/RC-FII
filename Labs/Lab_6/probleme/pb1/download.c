#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/stat.h>
#include <fcntl.h>


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
        printf("%s aici? ", msg); fflush(stdout);
    }

    read(sd, path, SIZE);
    int fsursa = open(path, O_RDONLY);
    while(1) {
        char ch;
        int cod = read(fsursa, &ch, 1);
        if(cod <= 0) {
            break;
        }
        write(sd, &ch, 1);
    }

    close(fsursa);
    shutdown(sd, 2);
}