#include <sys/socket.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/uio.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <fcntl.h>

#define PORT 2024
#define SIZE 101
#define RESPONSE 1001

int find_in_file(char*, char*);

int main(int agrc, char* argv[]) {
    int sd;
    if(-1 == (sd = socket(AF_INET, SOCK_STREAM, 0))) {
        perror("[server]Eroare la socket()"); exit(1);
    }
    struct sockaddr_in server;
    bzero(&server, sizeof(server));
    struct sockaddr_in from;
    bzero(&from, sizeof(from));

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(PORT);

    if(-1 == bind(sd, (const struct sockaddr *)&server, sizeof(server))) {
        perror("[server]Eroare la bind()"); exit(1);
    }

    if(-1 == listen(sd, 5)) {
        perror("[server]Eroare la listenq()"); exit(1);
    }
    int client;
    char key[SIZE];
    char msg[SIZE];
    char command[SIZE];
    char result[SIZE];
    while(1) {
        int len = sizeof(from);
        if(-1 == (client = accept(sd, (struct sockaddr*) &from, &len))) {
            perror("[server]Eroare la accept()"); exit(1);
        }
        read(client, key, SIZE);
        if(find_in_file(key, command) == -1) {
            strcpy(msg, "Unknown key");
            write(client, msg, SIZE);
            close(client);
            break;
        } else {
            find_in_file(key, command);
            int child = fork();
            if(child == 0) {
                freopen("aux.txt", "a+", stdout); 
                execl(command, command, 0);
            } 
            else {
                wait(NULL);
                int fd = open("aux.txt", O_RDONLY);
                read(fd, result, SIZE);
                write(client, result, SIZE);
                close(client);
                break;
            }
        }
    }
}

int find_in_file(char key[], char comanda[]) {
    char path[SIZE] = "./ex2.txt";
    int fd = open(path, O_RDONLY);
    char line[SIZE];
    int cursor = 0;
    while(1) {
        char ch;
        int cod = read(fd, &ch, 1);
        if(cod < 0) {
            perror("[server] Eroare la citire!"); exit(1);
        } else if(cod == 0) {
            return -1;
        }
        if(ch == '\n') {
            line[cursor] = '\0';
            if(strstr(line, key)) {
                int ok = 0, index = 0;
                for(int i = 0; line[i] != '\0'; i++) {
                    if(ok == 0 && line[i] == ':') ok = 1;
                    else if(ok == 1 && line[i] != ':') {
                        comanda[index++] = line[i];
                    }
                    else if(ok == 1 && line[i] == ':') {
                        comanda[index] = '\0';
                    }
                }
                return 1;
            }
            cursor = 0;
            strcpy(line, "");
        } else {
            line[cursor++] = ch;
        }
    }
    return -1;
}