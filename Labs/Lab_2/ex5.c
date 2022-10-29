
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>

void handler_sigusr1(int sig)
{
    int fd;
    if (-1 == (fd = open("aux.txt", O_CREAT | O_TRUNC | O_WRONLY)))
    {
        perror("Eroare la creare!");
        exit(1);
    }
    int bytes_in;
    char *sentence = "Am primit semnal\n";
    if (-1 == (bytes_in = write(fd, sentence, strlen(sentence))))
    {
        perror("Eroare scriere!");
        exit(2);
    }
    if (bytes_in != strlen(sentence))
    {
        perror("Eroare scriere!");
        exit(2);
    }
    close(fd);
}

int main(int argc, char *argv[])
{
    int counter = 0;
    // sigset_t mask;
    // sigemptyset(&mask);
    // sigaddset(&mask, SIGINT);
    // sigprocmask(SIG_SETMASK, &mask, NULL);
    signal(SIGINT, SIG_IGN);

    while (1)
    {
        printf("Se afiseaza a %d oara pid-ul %d\n", counter, getpid());
        counter++;
        sleep(3);

        signal(SIGUSR1, handler_sigusr1);
        if(counter > 20) {
            // sigprocmask(SIG_UNBLOCK, &mask, NULL);
            signal(SIGINT, SIG_DFL);
        }
    }
    return 0;
}