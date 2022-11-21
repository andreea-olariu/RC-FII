#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>

/* codul de eroare returnat de anumite apeluri */
extern int errno;

/* portul de conectare la server*/
int port;
int main (int argc, char *argv[])
{
  int sd;			// descriptorul de socket
  struct sockaddr_in server;	// structura folosita pentru conectare 
  int numar = 0;

  /* exista toate argumentele in linia de comanda? */
  if (argc != 3)
    {
      printf ("Sintaxa: %s <adresa_server> <port>\n", argv[0]);
      return -1;
    }

  /* stabilim portul */
  port = atoi (argv[2]);

  /* cream socketul */
  if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
      perror ("Eroare la socket().\n");
      return errno;
    }

  /* umplem structura folosita pentru realizarea conexiunii cu serverul */
  /* familia socket-ului */
  server.sin_family = AF_INET;
  /* adresa IP a serverului */
  server.sin_addr.s_addr = inet_addr(argv[1]);
  /* portul de conectare */
  server.sin_port = htons (port);
  
  /* ne conectam la server */
  if (connect (sd, (struct sockaddr *) &server,sizeof (struct sockaddr)) == -1)
    {
      perror ("[client]Eroare la connect().\n");
      return errno;
    }

  /* citirea mesajului */
  printf ("[client]Introduceti un numar: ");
  fflush (stdout);

    scanf("%d", &numar);
  
  /* trimiterea mesajului la server */
  if (write (sd, &numar, 100) <= 0)
    {
      perror ("[client]Eroare la write() spre server.\n");
      return errno;
    }

  /* citirea raspunsului dat de server 
     (apel blocant pina cind serverul raspunde); Atentie si la cum se face read- vezi cursul! */

    int ordine = 0;
  if (read (sd, &ordine, 4) < 0)
    {
      perror ("[client]Eroare la read() de la server 1.\n");
      return errno;
    }
    int x = 0;
    if (read (sd, &x, 4) < 0)
    {
      perror ("[client]Eroare la read() de la server 1.\n");
      return errno;
    }
  /* afisam mesajul primit */
  printf ("[client]Clientul cu  ordinul %d afiseaza nr incrementat %d\n", ordine, x);

  /* inchidem conexiunea, am terminat */
  close (sd);
}
