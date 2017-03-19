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
#define MAX_BUFFER_SIZE 500

int sock;
message msg_auth;


char *format_for_display(message msg) {
    char *buff = calloc(sizeof(message) + msg.length, sizeof(char));
    sprintf(buff, "%s[%s]%s : %s",KYEL, msg.username,KWHT, msg.body);
    return buff;
}

void client_rec_send(FILE *fp, int sockfd) {
    int maxfdp1;
    int stdineof;
    fd_set rset;
    char buffer[BUFFER_SIZE];
    int read_size;
    stdineof = 0;

    FD_ZERO(&rset);
    while (1) {
        strcpy(buffer, "");
        if (stdineof == 0)
            FD_SET(fileno(fp), &rset); //adds fp to set <<rset>>
        FD_SET(sockfd, &rset); //adds sockfd to rset
        maxfdp1 = max(fileno(fp), sockfd) + 1;
        select(maxfdp1, &rset, NULL, NULL, NULL);

        //reading from socket - writing to stdout
        if (FD_ISSET(sockfd, &rset)) {

            memset(buffer, 0, MAX_BUFFER_SIZE);


            if ((read_size = (int) read(sockfd, buffer, TOTAL_HEADER_LEN)) == 0) {

                if (stdineof == 1)
                    return;
                else {
                    printf("server terminated...\n");
                    exit(1);
                }
            }
//            printf("Message received from cliend id: %d, %s\n", sock, buffer);
            message message_received = deserialize(buffer);

            memset(buffer, 0, MAX_BUFFER_SIZE);
            if (read_size > 0 && (read_size = (int) read(sockfd, buffer, message_received.length)) == 0) {
                if (stdineof == 1)
                    return;
                else {
                    printf("server terminated...\n");
                    exit(1);
                }
            }
            strcpy(message_received.body, buffer);

            if (read_size > 0) {
                if (strcmp(message_received.type, "MSG") == 0 &&
                    strcmp(message_received.username, msg_auth.username) != 0) {
                    char *m = format_for_display(message_received);
                    write(fileno(stdout), m, strlen(m));
                } else if (strcmp(message_received.type, "DIS") == 0) {
                    exit(0);
                }
                memset(buffer, '\0', BUFFER_SIZE);
            }
        }

        //reading from stdin - writing to socket
        if (FD_ISSET(fileno(fp), &rset)) {
            if ((read_size = read(fileno(fp), buffer, BUFFER_SIZE)) == 0) {

                stdineof = 1;
                shutdown(sockfd, SHUT_WR);
                FD_CLR(fileno(fp), &rset);
                continue;
            }
            // buffer_from_server contine mesajul

            if (strcmp(buffer, "\n") != 0) {
                message msg;
                if (strcmp(buffer, "-logout\n") == 0) {
                    msg = create_message("DIS", msg_auth.username, "");
                } else {
                    msg = create_message("MSG", msg_auth.username, buffer);
                }
                char *m = serialize(msg);
                write(sockfd, m, strlen(m));
                memset(buffer, '\0', BUFFER_SIZE);
            }
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


int auth_req(int sockfd) {

    char username[5], password[21];
    puts("Username:");
    scanf("%4s", username);
    puts("Pass:");
    scanf("%s", password);

    msg_auth = create_message("AUT", username, password);
    char *b = serialize(msg_auth);

    //printf("[DEBUG] Serialized data: %s\n", b);
    write(sockfd, b, strlen(b));

    int read_size;
    char buffer_from_server[MAX_BUFFER_SIZE];
    message auth_response;
    if ((read_size = (int) recv(sock, buffer_from_server, TOTAL_HEADER_LEN, 0)) > 0) {
        auth_response = deserialize(buffer_from_server);
    }
    printf("[SERVER_RESPONSE] %s\n", auth_response.type);
    
    if(strncmp(auth_response.type, "ERR", 3) == 0){
        return 0;    
    }

    return 1;

}

void logout(int sockfd) {

}

int main(int argc, char *argv[]) {
    initialize_client();
    int auth_success = 0;
    int n;
    do {
        auth_success = auth_req(sock);
    } while(!auth_success);
    
    client_rec_send(stdin, sock);

    close(sock);
    return 0;
}
