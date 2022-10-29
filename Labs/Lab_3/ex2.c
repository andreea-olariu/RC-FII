#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/uio.h>

char nume_fifo[101] = "fifo";
char path[101] = "./prog.c";

void fiu1f();
void fiu2f();
void fiu3f();

int main(int argc, char *argv[])
{
    if (-1 == mknod(nume_fifo, S_IFIFO | 0666, 0)) {
        perror("Eroare la crearea fifo!");
        exit(1);
    }
    pid_t fiu;
    if (-1 == (fiu = fork())) {
        perror("Eroare la fork!");
        exit(2);
    }
    if (fiu == 0) {
        fiu1f();
    }
    else {
        pid_t fiu2;
        if (-1 == (fiu2 = fork())) {
            perror("Eroare la crearea fiului 2!");
            exit(5);
        }
        if (fiu2 == 0) {
            fiu2f();
        }
        else {
            wait(NULL);
            pid_t fiu3;
            if (-1 == (fiu3 = fork())) {
                perror("Eroare la crearea fiului 3!");
                exit(5);
            }
            if (fiu3 == 0) {
                fiu3f();
            } else {
                wait(NULL);
            }
        }
    }
    // execlp("rm", "rm", nume_fifo, 0);
}

void fiu1f()
{
    // cat
    int fd;
    if (-1 == (fd = open(path, O_RDONLY))) {
        perror("Eroare la deschiderea fisierului!");
        exit(2);
    }
    int fd_fifo_scriere;
    if (-1 == (fd_fifo_scriere = open("fifo", O_WRONLY))) {
        perror("Eroare la deschiderea fisierului!");
        exit(2);
    }

    while (1) {
        char ch;
        int cod_term = read(fd, &ch, 1);
        if (cod_term == 0) {
            printf("Am terminat citirea!");
            break;
        } else if (cod_term < 0) {
            perror("Eroare la citire!");
            exit(3);
        }
        // vreau sa scriu in fifo ce citesc
        if (-1 == write(fd_fifo_scriere, &ch, 1)) {
            perror("Eroare la scrierea in fifo!");
            exit(3);
        }
    }
    close(fd_fifo_scriere);
    close(fd);
}

void fiu2f()
{
    int fd_fifo_citire;
    if (-1 == (fd_fifo_citire = open(nume_fifo, O_RDONLY))) {
        perror("Eroare la deschiderea fifo-ului in citire!");
        exit(6);
    }
    int fd_fifo_scriere;
    if (-1 == (fd_fifo_scriere = open(nume_fifo, O_WRONLY))) {
        perror("Eroare la deschiderea fifo-ului in scriere!");
        exit(6);
    }

    // citesc rand cu rand
    char line[10001];
    int cursor = 0;
    while (1) {
        int cod_term;
        char ch;
        cod_term = read(fd_fifo_citire, &ch, 1);
        if (cod_term == 0) {
            printf("Am terminat citirea!");
            break;
        } else if(cod_term < 0) {
            perror("Eroare la citire!");
            exit(8);
        }
        if (ch == '\n') {
            line[cursor] = '\0';
            if (strstr(line, "include") != NULL){
                // scriu in fifo liniile care corespund
                int bytes_in;
                if (-1 == (bytes_in = write(fd_fifo_scriere, line, strlen(line)))){
                    perror("Eroare scriere!");
                    exit(8);
                }
                if (bytes_in != strlen(line)){
                    perror("Eroare la scriere!");
                    exit(8);
                }
            }
            strcpy(line, "");
            cursor = 0;
        }
        else{
            line[cursor++] = ch;
        }
    }
    close(fd_fifo_citire);
    close(fd_fifo_scriere);
}

void fiu3f()
{
    // scriu din fifo in prog.c
    int fd_fifo_citire;
    if (-1 == (fd_fifo_citire = open(nume_fifo, O_RDONLY))) {
        perror("Eroare la deschiderea fifo-ului in citire!");
        exit(6);
    }


    int fd;
    if (-1 == (fd = open(path, O_WRONLY))){
        perror("Eroare la deschiderea fisierului!");
        exit(2);
    }
    while (1) {
        int cod_term;
        char ch;
        cod_term = read(fd_fifo_citire, &ch, 1);
        if (cod_term == 0) {
            printf("Am terminat citirea!");
            break; 
        } else if(cod_term < 0) {
            perror("Eroare la citire!");
            exit(8);
        }
        if(-1 == write(fd, &ch, 1)) {
            perror("Eroare la scriere!");
            exit(9);
        }
    }
    close(fd);
    close(fd_fifo_citire);
}