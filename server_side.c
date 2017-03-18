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

#define MAX_BUFFER_SIZE 500
#define NUMBER_OF_USERS 1000
int current_number_of_users = 0;

int *connections;
struct sockaddr_in server_addr, client_addr;
int socket_desc, client_sock, socket_length, *new_sock;
message *client_auth;
char data[PACK_SIZE];

typedef struct userList{
    char username[4];
    char password[100];
} userList;

userList connected_users[NUMBER_OF_USERS];


void auth_hadler(int socket) {
    int rec;

    if ((rec = (int) recv(socket, data, PACK_SIZE, 0)) > 0) {
        deserialize(data);
        printf("User %s\n", client_auth->username);
    }
}


void initialize_server() {
    connections = calloc(NUMBER_OF_USERS, sizeof(int));
    //Create socket
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1) {
        printf("Could not create socket");
        exit(1);
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
        exit(2);
    }
    puts("bind done");
    //Listen
    listen(socket_desc, 10);// NUMBER_OF_USERS);
}

void *connection_handler(void *socket_desc) {

    //Get the socket descriptor
    int sock = *(int *) socket_desc;
    int read_size;
    char buffer_from_clients[MAX_BUFFER_SIZE];

    //Receive a message from client
    while (1) {
        memset(buffer_from_clients, 0, MAX_BUFFER_SIZE);
        message message_received;
        if ((read_size = (int) recv(sock, buffer_from_clients, TOTAL_HEADER_LEN, 0)) > 0) {
            //Send the message back to
            printf("Message received from cliend id: %d, %s\n", sock, buffer_from_clients);
            message_received = deserialize(buffer_from_clients);
        }
        memset(buffer_from_clients, 0, MAX_BUFFER_SIZE);
        if ((read_size = (int) recv(sock, buffer_from_clients, message_received.length, 0)) > 0) {
            strcpy(message_received.body, buffer_from_clients);
        }

        if (strcmp(message_received.type, "AUT") == 0) {
            printf("Authenticated userId: %s\n", message_received.username);
            for(int i = 0; i < current_number_of_users; i++){
                if(strcmp(connected_users[i].username, message_received.username) != 0) {
                    strcpy(connected_users[current_number_of_users].username, message_received.username);
                    strcpy(connected_users[current_number_of_users].password, message_received.body);
                    current_number_of_users++;
                }
            }
            //TODO check user in list and send back a ACK/ERR message
        } else if (strcmp(message_received.type, "MSG") == 0) {
            for (int index = 0; index < current_number_of_users; index++) {
                if (connections[index] > 0) {
                    printf("Client: %d\n", connections[index]);
                    char *message_to_send;
                    message_to_send = serialize(message_received);
                    //TODO format string to a desired structure
                    write(connections[index], message_to_send, strlen(message_to_send));
                }
            }
        } else if (strcmp(message_received.type, "DIS") == 0) {
            printf("Log out\n");

        }

    }
}


int main(int argc, char *argv[]) {

    initialize_server();
    userList connected_users[NUMBER_OF_USERS];
    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    socket_length = sizeof(struct sockaddr_in);

    while ((connections[current_number_of_users] = accept(socket_desc, (struct sockaddr *) &client_addr,
                                                          (socklen_t *) &socket_length))) {
        puts("Connection accepted");

        pthread_t sniffer_thread;
        new_sock = malloc(1);
        *new_sock = connections[current_number_of_users];

        auth_hadler(socket_desc);


        if (pthread_create(&sniffer_thread, NULL, connection_handler, (void *) new_sock) < 0) {
            perror("could not create thread");
            return 1;
        }

        puts("Handler assigned");
        current_number_of_users++;
    }

    if (client_sock < 0) {
        perror("accept failed");
        return 1;
    }

    return 0;

}
