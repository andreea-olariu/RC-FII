
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>

int main(int argc, char* argv[]) {
    int sockets[2], child;
    char buf[1024];

    socketpair(AF_UNIX, SOCK_STREAM, 0, sockets);
    // AF_UNIX : comunicare locala
    child = fork();

    if(child) {
        close(sockets[0]);

        read(sockets[1], buf, 1024);
        printf("Parent proces: %s\n", buf);
        write(sockets[1], "first", 5);

        close(sockets[1]);
    } else {
        close(sockets[1]);

        write(sockets[0], "second", 6);
        read(sockets[0], buf, 1024);
        printf("Child process: %s\n", buf);

        close(sockets[0]);
    }
    return 0;
}   