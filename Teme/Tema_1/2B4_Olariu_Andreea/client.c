#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define COMMAND_SIZE 101
#define FIFO_NAME_1 "client2server"
#define FIFO_NAME_2 "server2client"
#define RESPONSE 1001

void ask_command(char*);
void write_in_server(char*, int);
void read_from_server(char*, int);

int main(int argc, char* argv[]) { 
    // CREEZ UN FIFO SA COMUNIC CU SERVERUL
    if(access(FIFO_NAME_1, F_OK) != 0)  {
        if(-1 == mkfifo(FIFO_NAME_1, 0666)) {
            perror("[client]Eroare la crearea fifo"); exit(1);
        } 
    }
    int fifo1, fifo2;
    if(-1 == (fifo1 = open(FIFO_NAME_1, O_WRONLY))) {
        perror("[client]Eroare la deschiderea fifo in capat scriere!"); exit(1);
    }
    char command[COMMAND_SIZE] = "";
    ask_command(command);
    write_in_server(command, fifo1);
    close(fifo1);
    while(1) {
        if(-1 == (fifo2 = open(FIFO_NAME_2, O_RDONLY))) {
            perror("Eroare la deschiderea fifo-ului!"); exit(1);
        }
        char mess[RESPONSE];
        read_from_server(mess, fifo2);
        close(fifo2);
        if(strcmp(mess, "PID") == 0) {
            if(-1 == (fifo1 = open(FIFO_NAME_1, O_WRONLY))) {
                perror("Eroare la deschiderea fifo-ului!"); exit(1);
            }
            int pid = getpid();
            int bytes_to_write = 4;
            int bytes_written;
            if(-1 == (bytes_written = write(fifo1, &bytes_to_write, 4))) {
                perror("[client]Eroare la scriere in fifo!"); exit(1);
            }
            if(bytes_written != 4) {
                perror("[client]Eroare la scriere in fifo!"); exit(1);
            }
            if(-1 == (bytes_written = write(fifo1, &pid, bytes_to_write))) {
                perror("[client]Eroare la scriere in fifo!"); exit(1);
            } 
            if(bytes_written != 4) {
                perror("[client]Eroare la scriere in fifo!"); exit(1);
            }
            close(fifo1);
        } else if(strcmp(mess, "ask") == 0) {
            if(-1 == (fifo1 = open(FIFO_NAME_1, O_WRONLY))) {
                perror("Eroare la deschiderea fifo-ului!"); exit(1);
            }
            strcpy(command, "");
            ask_command(command);
            write_in_server(command, fifo1);
            close(fifo1);
        } else {
            printf("%s\n", mess); fflush(stdout);
        }
    }
}

void ask_command(char command[]) {
    printf("Introduceti o comanda...\nPentru a vedea ce comenzi sunt disponibile, tastati \"help\": ");
    int cursor = 0;
    while(1) {
        char ch;
        ch = getc(stdin);
        if(ch != '\n') {
            command[cursor++] = ch;
        } else {
            break;
        }
    }
    command[cursor] = '\0';
}

void write_in_server(char command[], int fifo1) {
    int len = strlen(command);
    int bytes_written;
    if(-1 == (bytes_written = write(fifo1, &len, 4))) {
        perror("[client]Eroare la scriere in fifo!"); exit(1);
    }
    if(bytes_written != 4) {
        perror("[client]Eroare la scriere in fifo!"); exit(1);
    }
    if(-1 == (bytes_written = write(fifo1, command, strlen(command)))) {
        perror("[client]Eroare la scriere in fifo!"); exit(1);
    } 
    if(bytes_written != strlen(command)) {
        perror("[client]Eroare la scriere in fifo!"); exit(1);
    }
}

void read_from_server(char mess[], int fifo2) {
    int bytes_expected;
    read(fifo2, &bytes_expected, 4);
    int i = 0;
    while(bytes_expected--) {
        int cod_term;
        char ch;
        cod_term = read(fifo2, &ch, 1);
        if(cod_term == 0) break;
        else if(cod_term < 0) {
            perror("Eroare la citirea din fifo-ul in client de la sever!"); exit(1);
        }
        mess[i++] = ch;
    }
    mess[i] = '\0'; 
}