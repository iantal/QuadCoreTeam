#include <stdio.h> //printf
#include <stdlib.h>
#include <unistd.h>
#include <string.h>    //strlen
#include <sys/socket.h>    //socket
#include <arpa/inet.h> //inet_addr
#include "common.h"

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

#define max(a, b)    ((a) > (b) ? (a) : (b))
#define BUFFER_SIZE 4000

int sock;
message msg_auth;
char data[PACK_SIZE];

char *serialize(message msg) {
    char *buff = calloc(sizeof(message) + msg.length, sizeof(char));
    sprintf(buff, "%s%s%03d%s", msg.type, msg.username, msg.length, msg.body);
    return buff;
}

void client_rec_send(FILE *fp, int sockfd) {
    int maxfdp1;
    int stdineof;
    int i;
    fd_set rset;
    char buf[BUFFER_SIZE];
    char message_to_print[BUFFER_SIZE];
    int n;
    char lines[1024];
    stdineof = 0;

    FD_ZERO(&rset);
    while (1) {
        strcpy(buf, "");
        if (stdineof == 0)
            FD_SET(fileno(fp), &rset); //adds fp to set <<rset>>
        FD_SET(sockfd, &rset); //adds sockfd to rset
        maxfdp1 = max(fileno(fp), sockfd) + 1;
        select(maxfdp1, &rset, NULL, NULL, NULL);

        //reading from socket - writing to stdout
        if (FD_ISSET(sockfd, &rset)) {
            if ((n = (int) read(sockfd, buf, BUFFER_SIZE)) == 0) {

                if (stdineof == 1)
                    return;
                else {
                    printf("server terminated...\n");
                    exit(1);
                }
            }

            write(fileno(stdout), buf, n);
            memset(buf, '\0', BUFFER_SIZE);

        }

        //reading from stdin - writing to socket
        if (FD_ISSET(fileno(fp), &rset)) {
            if ((n = read(fileno(fp), buf, BUFFER_SIZE)) == 0) {

                stdineof = 1;
                shutdown(sockfd, SHUT_WR);
                FD_CLR(fileno(fp), &rset);
                continue;
            }

            // buf contine mesajul
            message msg;
            msg.body = malloc(n * sizeof(char));
            strcpy(msg.type, "MSG");
            strcpy(msg.username, msg_auth.username);
            msg.length = (unsigned int) n;
            strcpy(msg.body, buf);
            char *m = serialize(msg);
            //write(sockfd, buf, n);
            write(sockfd, m, strlen(m));
            memset(buf, '\0', BUFFER_SIZE);
            memset(m, '\0', strlen(m));
        }
    }
}


void initialize_client() {
    struct sockaddr_in server;

    //Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        printf("Could not create socket");
        exit(1);
    }
    puts("Socket created");

    server.sin_addr.s_addr = inet_addr(IP_ADDRESS);
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);

    //Connect to remote server
    if (connect(sock, (struct sockaddr *) &server, sizeof(server)) < 0) {
        perror("connect failed. Error");
        exit(2);
    }
    puts("Connected\n");
}


void auth_req(int sockfd) {

    strcpy(msg_auth.type, "AUT");

    puts("Username:");
    scanf("%4s", msg_auth.username);


    puts("Pass:");
    msg_auth.body = calloc(21, sizeof(char));
    scanf("%s", msg_auth.body);

    msg_auth.length = (unsigned int) strlen(msg_auth.body);


    char *b = serialize(msg_auth);
    printf("[DEBUG] Serialized data: %s\n", b);

    write(sockfd, b, strlen(b));


}

void logout(int sockfd) {

}

int main(int argc, char *argv[]) {
    initialize_client();
    int auth_success = 0;
    int n;
    auth_req(sock);

    //}while(auth_success ==0);
    client_rec_send(stdin, sock);

    close(sock);
    return 0;
}
