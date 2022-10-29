
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>

struct stat info;

char * getname(char* name_ext) {
    char* res = malloc(50);
    int cursor=0, i=0;
    while(strchr(name_ext+cursor, '/') != NULL)
        cursor++;
    res=strtok(name_ext+cursor, ".");
    return res;
}

int main(int argc, char* argv[]) {
    if(argc < 2) {
        perror("Prea putine argumente!"); exit(1);
    }
    
    if(stat(argv[1], &info) == -1) {
        perror("Eroare la stat!"); exit(2);
    }

        // ino_t    st_ino;    /* inode's number */
    char* name = getname(argv[1]);
    printf("Numele fisierului/directorului este: %s\n", name);
    long int seconds = info.st_mtimespec.tv_sec;
    char* buff = ctime(&seconds);
    printf("Data ultimei modificari este: %s\n", buff);
    
    return 0;
}