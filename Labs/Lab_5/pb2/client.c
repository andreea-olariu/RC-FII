#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/uio.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#define SIZE 101
#define RESPONSE 1001

int main(int argc, char* argv[]) {
    int sd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server;
    bzero(&server, sizeof(server));

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(argv[1]);
    server.sin_port = htons (atoi(argv[2]));

    connect(sd, (const struct sockaddr*)&server, sizeof(server));
    int x = 28;
    char key[SIZE] = "1111";
    char msg_server[SIZE];
    write(sd, key, SIZE);
    read(sd, msg_server, SIZE);
    printf("Rezultatul comenzii este: %s", msg_server); fflush(stdout);

}