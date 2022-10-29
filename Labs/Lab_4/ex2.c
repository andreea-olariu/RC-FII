#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define SIZE 1024
#define OPERATORI "+-*/"

char* int_to_string(int n);
char* fiu_ori();
char* fiu_imp();

int main(int argc, char* argv[]) {
    char* expresie = malloc(SIZE);
    strcpy(expresie, "3*5/3");
    
    int sv[2];
    socketpair(AF_UNIX, SOCK_STREAM, 0, sv);

    int child_ori = fork();
    if(child_ori == 0) {
        close(sv[1]);
        char* expr = malloc(SIZE);
        read(sv[0], expr, SIZE);
        
        char new_expr = malloc(SIZE);
        strcpy(new_expr, child_work(expr, '*'));
        write(sv[0], new_expr, strlen(new_expr));
        close(sv[0]);

    } else {
        close(sv[0]);
        write(sv[1], expresie, strlen(expresie));
        char* tmp;
        read(sv[1], tmp, SIZE);
        strcpy(expresie, tmp);
        close(sv[1]);

        int sv2[2];
        socketpair(AF_UNIX, SOCK_STREAM, 0, sv2);
        int child_imp = fork(); 

        if(child_imp == 0) {
            close(sv2[1]);
            char* expr = malloc(SIZE);
            read(sv2[0], expr, SIZE);
            
            char new_expr = malloc(SIZE);
            strcpy(new_expr, child_work(expr, '/'));
            write(sv2[0], new_expr, strlen(new_expr));
            close(sv2[0]);

        } else {
            close(sv2[0]);
            write(sv2[1], expresie, strlen(expresie));
            char* tmp;
            read(sv2[1], tmp, SIZE);
            strcpy(expresie, tmp);
            close(sv2[1]);
        }
    }
    return 0;
}


char* int_to_string(int n) {
    char* rez_sir;
    int cursor=0;
    while(n) {
        rez_sir[cursor++] = (n % 10) + '0';
        n /= 10;
    }
    rez_sir[cursor] = '\0';
    for(int i = 0; rez_sir[i] < strlen(rez_sir) / 2; i++) {
        char aux = rez_sir[i];
        rez_sir[i] = rez_sir[strlen(rez_sir) - 1 - i];
        rez_sir[strlen(rez_sir) - 1 - i] = aux;
    }
    return rez_sir;
}

char child_work(char* expresie, char operator) {
 char* new_expr = malloc(SIZE);
    int x = 0;
    for(int i = 0; expresie[i] != '\0'; i++) {
        if(expresie[i] == operator) {
            int nr_inainte = 0, nr_dupa = 0;
            int j = i;
            while(!strchr(OPERATORI, expresie[j])) {
                nr_inainte = nr_inainte + (expresie[j] - '0') * 10;
                j--;
            }
            int inainte = j + 1;
            j = i;
            while(!strchr(OPERATORI, expresie[j])) {
                nr_dupa = nr_dupa * 10 + (expresie[j] - '0');
                j++;
            }
            int rezultat = 0;

            switch(operator) {
                case '*':
                    rezultat = nr_inainte * nr_dupa;
                case '/':
                    rezultat = nr_inainte / nr_dupa;
                case '+':
                    rezultat = nr_inainte + nr_dupa;
                case '-':
                    rezultat = nr_inainte - nr_dupa;
                default:
                    break;
            }

            char* rez_sir = int_to_string(rezultat);
            strcat(new_expr, rez_sir);
            x += strlen(rez_sir);

            char* tmp;
            strcpy(tmp, new_expr);
            int cc = strlen(tmp) - 1;
            for(int g = i + 1; expresie[g] != '\0'; i++) {
                tmp[cc++] = expresie[g];
            }
            strcpy(expresie, tmp);
            i = inainte;
        } else {
            new_expr[x++] = expresie[i];
        }
    }
    new_expr[x] = '\0';
    return new_expr;
}
