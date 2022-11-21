#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* portul folosit */
#define PORT 3000
/* codul de eroare returnat de anumite apeluri */
extern int errno;

int main ()
{
  struct sockaddr_in server;	// structura folosita de server
  struct sockaddr_in from;	
  char msg_rec[101];
  char comanda[101];
  int sd;			//descriptorul de socket 

  /* crearea unui socket */
  if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1) {
      perror ("[server]Eroare la socket().\n");
      return errno;
  }
  /* pregatirea structurilor de date */
  bzero (&server, sizeof (server));
  bzero (&from, sizeof (from));
  /* umplem structura folosita de server */
  /* stabilirea familiei de socket-uri */
    server.sin_family = AF_INET;	
  /* acceptam orice adresa */
    server.sin_addr.s_addr = htonl (INADDR_ANY);
  /* utilizam un port utilizator */
    server.sin_port = htons (PORT);
  /* atasam socketul */
  if (bind (sd, (struct sockaddr *) &server, sizeof (struct sockaddr)) == -1)
    {
      perror ("[server]Eroare la bind().\n");
      return errno;
    }

  /* punem serverul sa asculte daca vin clienti sa se conecteze */
  if (listen (sd, 5) == -1)
    {
      perror ("[server]Eroare la listen().\n");
      return errno;
    }

  /* servim in mod iterativ clientii... */
  int ordin = 0, numar = 0;
  while (1)
    {
      ordin++;
      int client;
      int length = sizeof (from);

      printf ("[server]Asteptam la portul %d...\n",PORT);
      fflush (stdout);

      /* acceptam un client (stare blocanta pina la realizarea conexiunii) */
      client = accept (sd, (struct sockaddr *) &from, &length);

      /* eroare la acceptarea conexiunii de la un client */
      if (client < 0)
	{
	  perror ("[server]Eroare la accept().\n");
	  continue;
	}

      /* s-a realizat conexiunea, se astepta mesajul */
      printf ("[server]Asteptam mesajul...\n");
      fflush (stdout);
      
      /* citirea mesajului */
      if (read (client, msg_rec, 101) <= 0)
        {
        perror ("[server]Eroare la read() de la client.\n");
        close (client);	/* inchidem conexiunea cu clientul */
        continue;		/* continuam sa ascultam */
        }
	
      printf ("[server]Mesajul a fost receptionat...%d\n", numar);
      

    int fd = open("ex2.txt", O_RDONLY);
    char ch;
    char line[101] = "";
    int cursor = 0;
    while(1) {
        int cod_term;
        read(fd, &ch, 1);
        if(ch == '\0') {
            // new line
            char* p = strtok(line, ":");
            char* q = strtok(NULL, ":");
            if(strcmp(p, msg_rec) == 0) {
                strcpy(comanda, q);
            }
        } else {
            line[cursor++] = ch;
        }
    }
    int p[2];
    pipe(p);
    int child = fork();
    char rez_execlp[101] = "";
    if(child == 0) {
        close(p[0]);
        execl(comanda, comanda, NULL) > p[1];
    } else {
        close(p[1]);
        read(p[1], rez_execlp, 101);
        printf("Rezultatul este?? %s", rez_execlp); fflush(stdout);
    }
      
      /* returnam mesajul clientului */
      if (write (client, rez_execlp, 101) <= 0)
	{
	  perror ("[server]Eroare la write() catre client.\n");
	  continue;		/* continuam sa ascultam */
	}
      else
	printf ("[server]Mesajul a fost trasmis cu succes.\n");
      /* am terminat cu acest client, inchidem conexiunea */
    shutdown(client, SHUT_RDWR);
    }				/* while */
}				/* main */
