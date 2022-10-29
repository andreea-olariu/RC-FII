
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>

int main(int argc, char *argv[])
{
    pid_t fiu1;
    if (-1 == (fiu1 = fork())) {
        perror("Eroare la fiu 1!");
        exit(1);
    }

    if (fiu1 == 0) {
        // SUNT IN FIUL 1
        execlp("zsh", "zsh", "-c", "who > hei.txt", 0);
    }
    else {
        // SUNT IN TATA
        wait(NULL);
        pid_t fiu2;
        if (-1 == (fiu2 = fork())) {
            perror("Eroare la fiu 1!");
            exit(1);
        }

        if(fiu2 == 0) {
            // SUNT IN FIUL 2
            int fd;
            if(-1 == (fd = open("hei.txt", O_RDONLY))) {
                perror("eroare la citire"); exit(1);
            }
            char buff[101];
            int cursor = 0;
            while(1) {
                char ch;
                int cod_term = read(fd, &ch, 1);
                if(cod_term <= 0) break;

                buff[cursor++] = ch;
            }   
            printf(buff);
        } else {    
            wait(NULL);
            // SUNT IN TATA
        }
    }
    return 0;
}