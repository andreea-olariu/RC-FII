   #include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <string.h> 
#include <stdlib.h>

int prim(int nr) {
    if(nr < 2) {
        return 0;
    } 
    for(int d = 2; d * d <= nr; d++) {
        if(nr % d == 0) {
            return 0;
        }
    }
    return 1;
}

#define SIZE 101

int main(int argc, char* argv[]) {
    int p[2];
    if(-1 == pipe(p)) {
        perror("Eroare la crearea pipe-ului\n"); exit(1);
    }
    pid_t fiu;
    if(-1 == (fiu = fork())) {
        perror("Eroare la fork\n"); exit(2);
    }

    int target = 2;
    if(fiu == 0) {
        // Sunt in fiu 
        int bytes_read;
        int num_read;
        if(-1 == (bytes_read = read(p[0], &num_read, sizeof(num_read)))) {
            perror("Eroare la citire in fiu\n"); exit(4);
        }
        if(bytes_read != sizeof(num_read)) {
            perror("Eroare la read in fiu\n"); exit(4);
        }

        printf("Numarul este %d\n", num_read);
        int result = prim(num_read);
        close(p[0]);
        if(result == 1) {
            // transmit yes catre tata
            int bytes_written;
            char answ[SIZE] = "yes";
            printf("Sunt aici\n");
            if(-1 == (bytes_written = write(p[1], &answ, strlen(answ)))) {
                perror("Eroare la write din fiu catre tata\n"); exit(4);
            } 
            if(bytes_written != strlen(answ)) {
                perror("Eroare la write din fiu catre tata\n"); exit(4);
            }
        } else {
            // transmit no catre tata
            int bytes_written;
            char answ[SIZE] = "no";
            if(-1 == (bytes_written = write(p[1], &answ, strlen(answ)))) {
                perror("Eroare la write din fiu catre tata\n"); exit(4);
            } 
            if(bytes_written != strlen(answ)) {
                perror("Eroare la write din fiu catre tata\n"); exit(4);
            }
        }
        close(p[1]);
    } else {
        // Sunt in tata
        int bytes_written;
        if(-1 == (bytes_written = write(p[1], &target, sizeof(target)))) {
            perror("Eroare la write in tata\n"); exit(4);
        } 
        if(bytes_written != sizeof(target)) {
            perror("Eroare la write in tata\n"); exit(4);
        }
        printf("Am scris in fiu numarul...");
        wait(NULL);
        close(p[1]);
        int bytes_read; 
        char rec[SIZE];
        int cursor = 0;
        char ch;
        while(1) {
            int cod_term = read(p[0], &ch, 1);
            if(cod_term < 1) break;
            rec[cursor++] = ch;
        }
        rec[cursor] = '\0';
        printf("\n\nRASPUNSUL ESTE %s\n\n", rec);
        close(p[0]);
    }       

    return 0;
}