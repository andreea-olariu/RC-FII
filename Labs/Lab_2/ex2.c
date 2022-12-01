
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>

int main(int argc, char* argv[]) {
    pid_t fiu;

    mkfifo("hello", 0777);  
    int p[2];
    pipe(p);
    if(-1 == (fiu=fork())) {
        perror("Eroare la fork!"); exit(1);
    }
    if(fiu == 0) {  
        close(p[0]);
        dup2(p[1], 1);
        execlp("ls", "ls", "-a", "-l", NULL);
        close(p[1]);
    } else {
        dup2(1, p[1]);
        close(p[1]);
        char sir[1001];
        read(p[0], sir, 1000);
        printf("%s", sir); fflush(stdout);
        wait(NULL);
        printf("Fiu terminat!");
        close(p[0]);
    }
    return 0;
}