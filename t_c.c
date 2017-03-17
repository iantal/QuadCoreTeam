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

int sock;
message *client_auth;
char data[PACK_SIZE];

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
            memset(buf, '\0',BUFFER_SIZE);

        }

        //reading from stdin - writing to socket
        if (FD_ISSET(fileno(fp), &rset)) {
            if ((n = read(fileno(fp), buf, BUFFER_SIZE)) == 0) {

                stdineof = 1;
                shutdown(sockfd, SHUT_WR);
                FD_CLR(fileno(fp), &rset);
                continue;
            }

            write(sockfd, buf, n);
            memset(buf, '\0',BUFFER_SIZE);
        }
    }
}

void initialize_client() {
    struct sockaddr_in server;

    //Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        printf("Could not create socket");
    }
    puts("Socket created");

    server.sin_addr.s_addr = inet_addr(IP_ADDRESS);
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);

    //Connect to remote server
    if (connect(sock, (struct sockaddr *) &server, sizeof(server)) < 0) {
        perror("connect failed. Error");
    }
    puts("Connected\n");
}

char * serialize(message* msgPacket, int sock_fd)
{

//int i;
//    char *t = (char *) data;
//    i=0;
//    while(i<20){
//
//        *t = msgPacket->type[i];
//        t++;
//        i++;
//    }
//    char *uname = (char *)data;
//    i=0;
//    while(i<21){
//        *uname = msgPacket->username[i];
//        uname++;
//        i++;
//    }
//
//    int *len = (int *) uname;
//    *len = msgPacket->length;
//    len++;
//
//    char *p = (char*) len;
//    i = 0;
//    while (i < BUFFER_SIZE)
//    {
//        *p = msgPacket->body[i];
//        p++;
//        i++;
//    }

    message test;
    test.type = AUTH_REQ;
    strcpy(test.username, "gigi\0");
    test.length = 21;
    test.body = calloc(21,sizeof(char));
    strcpy(test.body, "10\0");

    char* buff = calloc(sizeof(test), sizeof(char));
    memcpy(buff, &test, sizeof(test));
    write(sock_fd, buff, sizeof(buff));
}

void auth_req(int sockfd){
    client_auth = malloc(1*sizeof(message));
//    puts("Username:");
//    char name[20];
//    fgets(name, 20, stdin);
//    strcpy(client_auth->username,name);
//    printf("User[ %s ]",name);
//    printf("\n");
//    puts("Pass:");
//    char pass[100];
//    fgets(pass, 100, stdin);
//    strcpy(client_auth->body,pass);
//    printf("%s\n",pass);
//    client_auth->length = 100;

    serialize(client_auth, sockfd);
    printf("[DEBUG] Serialized data: %s\n",data);
}

int main(int argc, char *argv[]) {
    initialize_client();
    auth_req(sock);
    client_rec_send(stdin, sock);

    close(sock);
    return 0;
}
