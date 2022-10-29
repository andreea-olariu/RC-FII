#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>

void handler(int sig) {
    printf("Fortune\n");
}

int main(int argc, char *argv[]) {
    pid_t fiu;
    signal(SIGQUIT, handler);
    if(-1 == (fiu=fork())) {
        perror("Eroare!"); exit(1);
    }
    if(fiu == 0) {
        // FIU 
    } else {
        // TATAL
        if(fiu % 2 == 0) {
            kill(fiu, SIGQUIT);
            wait(NULL);
        } else {
            printf("Lost\n");
            raise(SIGINT);
        }
    }
    return 0;
}