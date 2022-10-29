
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>

void parcurgere(char* path) {
    struct stat info;
    
    if(-1 == stat(path, &info)) {
        perror("Eroare la stat!"); exit(2);
    }
    if((info.st_mode & S_IFDIR) != S_IFDIR) {
        // Este fisier 
        printf("%s\n", path);
    } else {
        DIR *dd;
        struct dirent *de;

        if(NULL == (dd = opendir(path))) {
            perror("Eroare la deschiderea directorului!"); exit(4);
        }

         while(NULL != (de = readdir(dd)) ) {
            // urmatorul pointer sa nu fie parintele sau parintele parintelui
            if( strcmp(de->d_name,".") && strcmp(de->d_name,"..") ) {
                char nume[50];
                // facem calea ca char* pt urmatorul in mod recursiv
                sprintf(nume,"%s/%s",path,de->d_name);
                parcurgere(nume); 
            }
        }
        closedir(dd);
    }
}

int main(int argc, char* argv[]) {
    if(argc < 2) {
        perror("Prea putin argumente!"); exit(1);
    }   
    struct stat info;
    if(-1 == stat(argv[1], &info)) {
        perror("Eroare la stat!"); exit(2);
    }
    // printf("%d %d %d \n", (info.st_mode & S_IFDIR), info.st_mode, S_IFDIR);
    if((info.st_mode & S_IFDIR) != S_IFDIR) {
        perror("Argumentul dat nu este calea unui director"); exit(3);
    } 
    parcurgere(argv[1]);
    return 0;
}