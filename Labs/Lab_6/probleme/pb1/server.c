#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <strings.h>
#include <string.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>

#define PORT 2024
#define SIZE 101

void ask_file_source(char*, int, char*);
void ask_download(char*, char*);

int main(int argc, char* argv[]) {
    int sd;
    sd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server; 
    struct sockaddr_in from; 
    char path[SIZE] = "";
    char download[SIZE] = "download";
    char extension[SIZE] = "";

    bzero(&server, sizeof(server));
    bzero(&server, sizeof(from));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl (INADDR_ANY);
    server.sin_port = htons (PORT);

    bind(sd, (const struct sockaddr*)&server, sizeof(server));
    listen(sd, 1);
    while(1) {
        int client;
        int len;
        client = accept(sd, (struct sockaddr*)&from, &len);
        int p[2];
        pipe(p);
        int pid = fork();
        if(pid == 0) {
            close(p[0]);
            if(strcmp(path, "") == 0) {
                ask_file_source(path, client, extension);
                strcat(download, extension);
                write(p[1], path, SIZE);
            }
            ask_download(path, download);
            close(p[1]);
        } else {
            close(p[1]);
            if(strcmp(path, "") == 0)
                read(p[0], path, SIZE);
            waitpid(-1, NULL, WNOHANG);
            close(p[0]);
        }
    }
}

void ask_file_source(char* path, int client, char* extension) {
    char msg[SIZE];
    if(strcmp(path, "") == 0) {
        strcpy(msg, "Astept!");
        write(client, msg, SIZE);
        read(client, path, SIZE);
    } else {
        strcpy(msg, "Update: Exista deja un path...");
        write(client, msg, SIZE);
    }
    printf("Path-ul este: %s\n", path); fflush(stdout);

    int ok = 0;
    int idx = 0;
    for(int i = 0; path[i] != '\0'; i++) {
        if(path[i] == '.') {
            ok = 1;
        }
        if(ok == 1) {
            extension[idx++] = path[i];
        }
    }
    extension[idx] = '\0';
}

void ask_download(char* path, char* download) {
    int fsursa = open(path, O_RDONLY);
    int fdest;
    fdest = open(download, O_WRONLY | O_CREAT, 0777);
    while(1) {
        char ch;
        int cod = read(fsursa, &ch, 1);
        if(cod <= 0) {
            break;
        }
        write(fdest, &ch, 1);
    }
    close(fdest);
    close(fsursa);
}