
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char* argv[]) {
    if(argc < 3) {
        perror("Prea putine argumente!"); exit(1);
    }
    int input_fd;
    if((input_fd = open(argv[1], O_RDONLY)) == -1) {
        perror("Eroare la deschiderea fisierului!"); exit(2);
    }
    char buff[1], line[256];
    int poz = 0;
    while(read(input_fd, buff, 1)) {
        if(buff[0] == '\n') {
            line[poz++] = '\0';
            if(argc == 4 && strcmp(argv[3], "-v") == 0) {
                if(strstr(line, argv[2]) == NULL) {
                    printf("%s\n", line);
                }
            } else if(strstr(line, argv[2]) != NULL) {
                printf("%s\n", line);
            } 
            // Reset
            poz = 0;
            memcpy(line, "", 0);
            continue;
        } 
        line[poz++] = buff[0];
    }

    if(argc == 4 && strcmp(argv[3], "-v") == 0) {
        if(strstr(line, argv[2]) == NULL) {
            printf("%s\n", line);
        }
    } else if(strstr(line, argv[2]) != NULL) {
        printf("%s\n", line);
    } 

    if(close(input_fd) == -1) {
        perror("Eroare la inchidere!"); 
    }
    return 0;
}