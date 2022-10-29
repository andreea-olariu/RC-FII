#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <utmp.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>

#define FIFO_NAME_1 "client2server"
#define FIFO_NAME_2 "server2client"
#define COMMAND_SIZE 101
#define USER_SIZE 101
#define COMMANDS "help_login_logout_quit_get-logged-users_get-proc-info"
#define USERS "users.txt"
#define RESPONSE 1001
#define UTMP "/var/run/utmp"
#define DATE_SIZE 101

void get_command_from_client(char*);
int get_pid_from_client();
void read_from_fd(char*, int, int);
int validate_command(char*);
void login_child_work(char*, char*, char*);
int validate_username(char*);
void help_child_work();
void send_message_to_client(char*);
void logout_child_work(char*, char*);
void get_logged_users_child_work(char*);
void get_proc_info_child_work(char*, char*);
void get_after_column(char*, char*);
void write_to_fd(char*, int);

char conn_user[USER_SIZE];

int main(int argc, char* argv[]) {
    char ask[4] = "ask";
    // CREEZ UN FIFO SA COMUNIC CU CLIENTUL
    if(access(FIFO_NAME_2, F_OK) != 0)  {
        if(-1 == mkfifo(FIFO_NAME_2, 0666)) {
            perror("[server]Eroare la crearea fifo!"); exit(1);
        } 
    }
    while(1) {
        int socket_vector[2];
        if(-1 == socketpair(AF_UNIX, SOCK_STREAM, 0, socket_vector)) {
            perror("[server]Eroare la crearea socketpair!"); exit(1);         
        }
        int validate_child;
        if(-1 == (validate_child = fork())) {
            perror("[server]Eroare la crearea copilului care se ocupa de validarea comenzii!"); exit(1);
        }
        if(validate_child == 0) {
            close(socket_vector[1]);
            char command[COMMAND_SIZE] = "";
            int nr_of_bytes;
            read(socket_vector[0], &nr_of_bytes, 4);
            read_from_fd(command, nr_of_bytes, socket_vector[0]);
            char ok = '1';
            if(0 == validate_command(command)) {
                char msg[RESPONSE] = "";
                sprintf(msg, "%s nu este valida...\n", command);
                send_message_to_client(msg);
                sleep(2);
                send_message_to_client(ask);
                ok = '0';
            } 
            // Transmite parintelui daca este ok comanda
            if(-1 == write(socket_vector[0], &ok, 1)) {
                perror("[server]Eroare la transmiterea ok-ului pentru urmatoarea comanda!"); exit(1); 
            }
            close(socket_vector[0]);
        } else {
            close(socket_vector[0]);
            char command[COMMAND_SIZE] = "";
            get_command_from_client(command);
            if(strcmp(command, "") != 0) {
                write_to_fd(command, socket_vector[1]);
            }
            char ch;
            if(-1 == read(socket_vector[1], &ch, 1)) {
                perror("[server]Eroare la citirea din socket!"); exit(1);
            }
            if(ch == '1') {
                // Fiecare comanda sa aiba un copil
                if(strcmp(command, "help") == 0) {
                    int help_child;
                    if(-1 == (help_child = fork())) {
                        perror("[server]Eroare la crearea copilului care se ocupa de comanda help!"); exit(1);
                    }
                    if(help_child == 0) {
                        help_child_work();
                        sleep(2);
                        send_message_to_client(ask);
                        return 0;
                    } 
                } else if(strstr(command, "login") != 0) {
                    int p[2];
                    if(-1 == pipe(p)) {
                        perror("[server]Eroare la crearea pipe-ului dintre copilul login si parinte!"); exit(1);
                    }
                    int login_child;
                    if(-1 == (login_child = fork())) {
                        perror("[server]Eroare la crearea copilului care se ocupa de comanda login!"); exit(1);
                    }
                    if(login_child == 0) {
                        close(p[0]);
                        char msg[RESPONSE] = "";
                        char user[USER_SIZE] = "";
                        login_child_work(msg, user, command);
                        write_to_fd(msg, p[1]);
                        if(strcmp(user, "") != 0) {
                            write_to_fd(user, p[1]);
                        }
                        close(p[1]);
                        return 0;
                    } else {
                        close(p[1]);
                        char user[USER_SIZE] = "";
                        char msg[RESPONSE] = "";
                        int i = 0, len;
                        read(p[0], &len, 4);
                        read_from_fd(msg, len, p[0]);
                        if(strcmp(conn_user, "") == 0) {
                            read(p[0], &len, 4);
                            read_from_fd(user, len, p[0]); 
                            strcpy(conn_user, user);
                        }
                        close(p[0]);
                        send_message_to_client(msg);
                        sleep(2);
                        send_message_to_client(ask);
                    }
                } else if(strcmp(command, "logout") == 0) {
                     int p[2];
                    if(-1 == pipe(p)) {
                        perror("[server]Eroare la crearea pipe dintre copilul logout si parinte!"); exit(1);
                    }
                    int logout_child;
                    if(-1 == (logout_child = fork())) {
                        perror("[server]Eroare la crearea copilului care se ocupa de comanda logout!"); exit(1);
                    }
                    if(logout_child == 0) {
                        close(p[0]);
                        char msg[RESPONSE] = "";
                        logout_child_work(msg, conn_user);
                        write_to_fd(msg, p[1]);
                        close(p[1]);
                        return 0;
                    } else {
                        close(p[1]);
                        char msg[RESPONSE] = "";
                        int i = 0, len;
                        read(p[0], &len, 4);
                        read_from_fd(msg, len, p[0]);
                        strcpy(conn_user, "");
                        close(p[0]);
                        send_message_to_client(msg);
                        sleep(2);
                        send_message_to_client(ask);
                    }
                } else if(strcmp(command, "quit") == 0) {
                    char tmp[4] = "PID";
                    send_message_to_client(tmp);
                    int pid = get_pid_from_client();
                    kill(pid, SIGINT);
                } else if(strcmp(command, "get-logged-users") == 0) {
                    int p[2];
                    if(-1 == pipe(p)) {
                        perror("[server]Eroare la crearea pipe-ului dintre copilul logged-users si parinte!"); exit(1);
                    }
                    int logged_users_child;
                    if(-1 == (logged_users_child = fork())) {
                        perror("[server]Eroare la crearea copilului care se ocupa de comanda get-logged-users!"); exit(1);
                    }
                    if(logged_users_child == 0) {
                        close(p[0]);
                        char msg[RESPONSE] = "";
                        get_logged_users_child_work(msg);
                        write_to_fd(msg, p[1]);
                        close(p[1]);
                        return 0;
                    } else {
                        close(p[1]);
                        char msg_[RESPONSE] = "";
                        int i = 0, len;
                        read(p[0], &len, 4);
                        read_from_fd(msg_, len, p[0]);
                        close(p[0]);
                        send_message_to_client(msg_);
                        sleep(2);
                        send_message_to_client(ask);
                    }
                } else if(strstr(command, "get-proc-info") != 0) {
                    int p[2];
                    if(-1 == pipe(p)) {
                        perror("[server]Eroare la crearea pipe-ului dintre copilul proc-info si parinte!"); exit(1);
                    }
                    int proc_info_child;
                    if(-1 == (proc_info_child = fork())) {
                        perror("[server]Eroare la crearea copilului care se ocupa de comanda get-proc-info!"); exit(1);
                    }
                    if(proc_info_child == 0) {
                        close(p[0]);
                        char msg[RESPONSE] = "";
                        get_proc_info_child_work(msg, command);
                        write_to_fd(msg, p[1]);
                        close(p[1]);
                        return 0;
                    } else {
                        close(p[1]);
                        char msg_[1001] = "";
                        int i = 0, len;
                        read(p[0], &len, 4);
                        read_from_fd(msg_, len, p[0]);
                        close(p[0]);
                        send_message_to_client(msg_);
                        sleep(2);
                        send_message_to_client(ask);
                    }
                }
             }
            close(socket_vector[1]);
        }
    }
}

void get_command_from_client(char command[]) {
    int fd;
    if(-1 == (fd = open(FIFO_NAME_1, O_RDONLY))) {
        perror("[server]Eroare la deschiderea fifo!"); exit(1);
    }
    int bytes_expected;
    read(fd, &bytes_expected, 4);
    read_from_fd(command, bytes_expected, fd);
    close(fd);
}

int get_pid_from_client() {
    int fd;
    if(-1 == (fd = open(FIFO_NAME_1, O_RDONLY))) {
        perror("[server]Eroare la deschiderea fifo!"); exit(1);
    }
    int bytes_expected, pid;
    read(fd, &bytes_expected, 4);
    read(fd, &pid, bytes_expected);
    close(fd);
    return pid;
}

void read_from_fd(char command[], int bytes_expected, int fd) {
    int i = 0;
    while(bytes_expected-- > 0) {
        int cod_term;
        char ch;
        cod_term = read(fd, &ch, 1);
        if(cod_term < 0) {
            perror("[server]Eroare la citire!"); exit(1); 
        } 
        command[i++] = ch;
    }
    command[i] = '\0';
}

void write_to_fd(char command[], int fd) {
    int len = strlen(command);
    int bytes_written;
    if(-1 == (bytes_written = write(fd, &len, 4))) {
        perror("[server]Eroare la scriere in fd!"); exit(1);
    }
    if(bytes_written != 4) {
        perror("[server]Eroare la scriere in fd!"); exit(1);
    }
    if(-1 == (bytes_written = write(fd, command, strlen(command)))) {
        perror("[server]Eroare la scriere in fd!"); exit(1);
    } 
    if(bytes_written != strlen(command)) {
        perror("[server]Eroare la scriere in fdo!"); exit(1);
    }
}

void login_child_work(char msg[], char user[], char command[]) {
    get_after_column(command, user);
    if(validate_username(user) == 0) {
        strcpy(msg, user);
        strcat(msg, " nu are acces!");
        strcat(msg, "\0");
        strcpy(user, "");
    } else {
        if(strcmp(conn_user, "") != 0) {
            strcpy(msg, "Exista deja un utilizator conectat! Asteptati deconectarea acestuia.");
            strcat(msg, "\0");
            strcpy(user, "");
        } else {
            // Seteaza utilizatorul curent
            strcpy(conn_user, user);
            strcpy(msg, "Logat ca ");
            strcat(msg, user);
            strcat(msg, "\0");
        }
    }   
}

int validate_username(char str[]) {
    int fd_users;
    if(-1 == (fd_users = open(USERS, O_RDONLY))) {
        perror("Eroare la deschiderea fisierului cu users!"); exit(1); 
    } 
    int cursor = 0;
    char user[USER_SIZE] = "";
    while(1) {
        int cod_term;
        char ch;
        cod_term = read(fd_users, &ch, 1);
        if(cod_term < 0) {
            perror("Eroare la citirea din fisierului de users!"); exit(1); 
        } else if(cod_term == 0) {
            break;
        }
        if(ch == '\n') {
            // AM TERMINAT DE CITIT UN USER
            user[cursor] = '\0';
            if(strcmp(user, str) == 0) {
                return 1;
            }
            cursor = 0;  
        } else {
            user[cursor++] = ch;
        }
    }
    return 0;
}

int validate_command(char command[]) {
    if(strstr(COMMANDS, command) != 0) {
        return 1;
    }
    if(strstr(command, "login:") != 0) {
        return 1;
    }
    if(strstr(command, "get-proc-info:") != 0) {
        return 1;
    }
    return 0;
}

void help_child_work() {
    char help[RESPONSE] = "\nComenzile disponibile sunt:\n\nlogin - cere autentificarea\nget-logged-users - ofera informatii despre utilizatorii sistemului\nget-proc-info : pid - ofera informatii despre procesul indicat de \"pid\"\nlogout - incheie sesiunea\nquit - inchide aplicatia\n\n";
    send_message_to_client(help);
}

void send_message_to_client(char msg[]) {
    int fd;
    if(-1 == (fd = open(FIFO_NAME_2, O_WRONLY))) {
        perror("Eroare la deschiderea fifo-ului de comunicare dintre client si server!"); exit(1);
    }
    write_to_fd(msg, fd);
    close(fd);
}

void logout_child_work(char msg[], char conn_user[]){
    if(strcmp(conn_user, "") != 0) {
        strcpy(msg, conn_user);
        strcat(msg, " deconectat cu succes!\n");
        strcpy(conn_user, "");
    } else {
        strcpy(msg, "Niciun utilizator nu este logat!\n");
    }
    strcat(msg, "\0");
}

void get_logged_users_child_work(char msg[]) {
    if(strcmp(conn_user, "") == 0) {
        strcpy(msg, "Nu putem executa comanda daca nu este niciun user conectat..."); 
    } else {
        int fd;
        if(-1 == (fd = open(UTMP, O_RDONLY))) {
            perror("Eroare la deschiderea fisierului utmp!"); exit(1);
        }
        struct utmp* info = getutent();
        strcpy(msg, "");
        while(info != NULL) {
            time_t sec = info->ut_tv.tv_sec;
            // int micro_sec = info->ut_tv.tv_usec;
            struct tm date_s;
            char date[DATE_SIZE];
            date_s = *localtime(&sec);
            strftime(date, sizeof(date), "%a %d-%m-%Y %H:%M", &date_s);
            char tmp[2501];
            sprintf(tmp, "Numele userului este: %s\nAre hostname-ul pentru remote login: %s\nData intrarii %s\n", info->ut_user, info->ut_host, date);
            strcat(msg, tmp);
            info = getutent();
       }
    }
}

void get_proc_info_child_work(char msg[], char command[]) { 
    if(strcmp(conn_user, "") == 0) {
        strcpy(msg, "Niciun user conectat");
        strcat(msg, "\0");
    } else {
    char pid[51] = "";
    get_after_column(command, pid);
    int fd_pid;
    char path[101];
    sprintf(path, "/proc/%s/status", pid);
    if(-1 == (fd_pid = open(path, O_RDONLY))) {
        perror("Eroare la deschiderea fisierului cu informatii despre proces!"); exit(1); 
    } 
    int cursor = 0;
    char line[501] = "";
    char name[101], state[101], ppid[101], uid[101], vmsize[101];
    while(1) {
        int cod_term;
        char ch;
        cod_term = read(fd_pid, &ch, 1);
        if(cod_term < 0) {
            perror("Eroare la citirea din fisierului de users!"); exit(1); 
        } else if(cod_term == 0) {
            break;
        }
        if(ch == '\n') {
            // AM TERMINAT DE CITIT O LINIE
            line[cursor] = '\0';
            if(strstr(line, "Name") != 0) {
                get_after_column(line, name);
            } else if(strstr(line, "State") != 0) {
                get_after_column(line, state);
            } else if(strstr(line, "PPid") != 0) {
                get_after_column(line, ppid);
            } else if(strstr(line, "Uid") != 0) {
                get_after_column(line, uid);
            }  else if(strstr(line, "VmSize") != 0) {
                get_after_column(line, vmsize);
            }
            cursor = 0;  
        } else {
            line[cursor++] = ch;
        }
    }
    sprintf(msg, "Procesul cu pidul %s\nnume:%s\nstate: %s \nppid: %s\nuid: %s\nvmsize: %s\n\0", pid, name, state, ppid, uid, vmsize);
 } 
}

void get_after_column(char line[], char to[]) {
    int meet = 0;
    int j = 0;
    for(int i = 0; line[i] != '\0'; i++) {
        if(meet == 1) {
            to[j++] = line[i];
        }
        if(line[i] == ':') {
            meet = 1;
        }  
    }
    to[j] = '\0';
    while(to[0] == ' ') {
        strcpy(to, to + 1);
    }
    while(to[strlen(to) - 1] == ' ') {
        to[strlen(to) - 2] = '\0';
    }
}