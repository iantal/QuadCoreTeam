#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <pthread.h>
#include "common.h"


#define NUMBER_OF_USERS 1000

int *connections;
struct sockaddr_in server_addr, client_addr;
int socket_desc, client_sock, socket_length, *new_sock;

//the thread function
void *connection_handler(void *);

void initialize_server();

int main(int argc, char *argv[]) {

    char sendBuff[4096];
    initialize_server();

    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    socket_length = sizeof(struct sockaddr_in);
    int index = 0;
    while ((connections[index] = accept(socket_desc, (struct sockaddr *) &client_addr, (socklen_t *) &socket_length))) {
        puts("Connection accepted");

        pthread_t sniffer_thread;
        new_sock = malloc(1);
        *new_sock = connections[index];

        if (pthread_create(&sniffer_thread, NULL, connection_handler, (void *) new_sock) < 0) {
            perror("could not create thread");
            return 1;
        }

        //Now join the thread , so that we dont terminate before the thread
        //pthread_join( sniffer_thread , NULL);
        puts("Handler assigned");
        index++;
    }

    if (client_sock < 0) {
        perror("accept failed");
        return 1;
    }

    return 0;

}

void initialize_server() {
    connections = calloc(NUMBER_OF_USERS, sizeof(int));
    //Create socket
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1) {
        printf("Could not create socket");
    }
    puts("Socket created");

    //Prepare the sockaddr_in structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    //Bind
    if (bind(socket_desc, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        //print the error message
        perror("bind failed. Error");
    }
    puts("bind done");
    //Listen
    listen(socket_desc, 10);// NUMBER_OF_USERS);
}

//void send_message(char *sendBuff) {
//    int i;
//    for (i = 0; i < 100; i++) {
//        if (connections[i] > 0) {
//            printf("client id: %d\n", connections[i]);
//            write(connections[i], sendBuff, strlen(sendBuff));
//        }
//    }
//}

/*
 * This will handle connection for each client
 * */
void *connection_handler(void *socket_desc) {


    //Get the socket descriptor
    int sock = *(int *) socket_desc;
    int read_size;
    char *message, client_message[BUFFER_SIZE];

    //Receive a message from client
    while (1) {
        memset(client_message,0,BUFFER_SIZE);
        if ((read_size = (int) recv(sock, client_message, BUFFER_SIZE, 0)) > 0) {
            //Send the message back to
            printf("Message received from cliend id: %d, %s\n", sock, client_message);
            int index;
            for (index = 0; index < 1000; index++) {
                if (connections[index] > 0) {
                    printf("Client: %d\n", connections[index]);
                    write(connections[index], client_message, strlen(client_message));
                }
            }
        }
    }
}