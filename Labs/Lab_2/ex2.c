
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    pid_t fiu;
    if(-1 == (fiu=fork())) {
        perror("Eroare la fork!"); exit(1);
    }
    if(fiu == 0) {
        execlp("ls", "ls", "-a", "-l", NULL);
    } else {
        wait(NULL);
        printf("Fiu terminat!");
    }
    return 0;
}