
#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>

#define SIZE 1024

int main(int argc, char* argv[]) {

    int sv[2];
    socketpair(AF_UNIX, SOCK_STREAM, 0, sv);

    int child = fork();

    if(child == 0) {
        close(sv[1]);
        
        char primit[SIZE];
        char sir1[SIZE] = "Buna ziua!";

        read(sv[0], primit, SIZE);
        strcat(sir1, primit);
        write(sv[0], sir1, strlen(sir1));

        close(sv[0]);

    } else {
        close(sv[0]);

        char sir2[SIZE] = "Salut!";
        char final[SIZE];
        write(sv[1], sir2, strlen(sir2));
        read(sv[1], final, SIZE);
        printf("Am primit: %s\n", final);

        close(sv[1]);
    }
}