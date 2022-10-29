#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <string.h>

#define STRING_SIZE 10

int main(int argc, char *argv[]) {
    if(argc < 3) {
        perror("Prea putine argumente!"); exit(1);
    } else if(argc > 3) {
        perror("Prea multe argumente!"); exit(2);
    }

    printf("Hello World!");
    int input_fd, output_fd;
    if((input_fd = open(argv[1], O_RDONLY)) == -1) {
        perror("Eroare la deschiderea fisierului de input!"); exit(3);
    }

    if((output_fd = open(argv[2], O_WRONLY)) == -1) {
        perror("Eroare la deschidere fisierului de output!"); exit(4);
    }

    int bytes_in, bytes_out, to_write = 0;
    char buffer[STRING_SIZE];
    char strings[2*STRING_SIZE + 1];
    int counter_string = 0;
    int position = 0;

    while((bytes_in = read(input_fd, buffer, STRING_SIZE)) > 0 && counter_string < 3) {
        for(int i = 0; i < bytes_in; i++) {
            strings[position++] = buffer[i];
        }
        to_write += bytes_in;
        counter_string++;
    }
    strings[position] = '\0';
    if((bytes_out = write(output_fd, strings, to_write)) != to_write) {
        perror("Eroare la scriere!"); exit(5);
    }
    close(input_fd); close(output_fd);
}