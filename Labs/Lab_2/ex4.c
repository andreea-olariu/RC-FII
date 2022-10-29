#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define SIZE 101

int main(int argc, char* argv[]) {
    printf("Introduceti comanda: ");
    char command[SIZE];
    scanf("%s", command);

    printf("Introduceti directorul in care se gaseste executabilul: ");
    char dir[SIZE];
    scanf("%s", dir);

    char full_command[2*SIZE];
    strcpy(full_command, dir);
    strcat(full_command, "/");
    strcat(full_command, command);

    char* args[2];

    args[0] = malloc(SIZE);
    strcpy(args[0], full_command);
    args[1] = NULL;
    execv(full_command, args);
    return 0;
}