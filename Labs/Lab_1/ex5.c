#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <string.h>

#define SIZE 10000
#define SIZE2 10000
int main(int argc, char*argv[]) {
    int fd;
    if(-1 == (fd = open("/etc/passwd", O_RDONLY))) {
        perror("Nu am putut deschide /etc/passwd"); exit(1);
    }
    char ch;
    char line[SIZE];
    int cursor=0;

    while(1) {
        int cod_term = read(fd, &ch, 1);
        if(cod_term <= 0) break;
        if(ch!='\n') {
           line[cursor++]=ch;
        } 
        else {
            line[cursor]='\0';
            cursor=0;
            // check the line
           char name[SIZE2], uid[SIZE2], shell[SIZE2], aux[SIZE2];
           int count=0;
           int j=0;
           for(int i=0; line[i]!='\0'; i++) {
                while(line[i]!=':' && line[i] != '\0') {
                   aux[j++]=line[i];
                   i++;
                }
               aux[j]='\0';
                if(count==0){
                    strcpy(name, aux);
                } else if(count==2) {
                    strcpy(uid, aux);
                }
                j=0;
                count++;
                strcpy(aux,"");
           }
          aux[j]='\0';
           strcpy(shell, aux);
           int nr=0;
           for(int i=0; uid[i]!='\0'; i++) {
                nr = nr*10+(uid[i]-'0');
           }
           printf("%d, %d\n", getuid(), nr);
           if(nr==getuid()) {
                printf("Utilizatorul %s a initiat procesul si foloseste shellul %s\n", name, shell);
                break;
           }
        }
    }
    close(fd);
    return 0;
}