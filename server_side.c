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
#include "common.h"


#define NUMBER_OF_USERS 1000
#define MAXSZ 100


int* connections;
int listenfd = 0;
char sendBuff[4096];
int pid;
int clientAddressLength;


struct sockaddr_in serv_addr;//server receive on this address
struct sockaddr_in clientAddress;//server sends to client on this address

void initialize_server() {
    connections = calloc(NUMBER_OF_USERS, sizeof(int));
    struct sockaddr_in serv_addr;
    char sendBuff[BUFFER_SIZE];

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(PORT);

    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    listen(listenfd, NUMBER_OF_USERS);
}

void send_message(int connection_index, char *msg){
    strcpy(sendBuff,msg);
    printf("%s\n",msg);
    snprintf(sendBuff, sizeof(sendBuff),msg);
    write(connection_index, sendBuff, strlen(sendBuff));
    close(connection_index);
}

int main(int argc, char *argv[]){

    char msg[MAXSZ];
    int index = 0;
    int n;

    initialize_server();
    
    while(1){    
    
        printf("\n*****server waiting for new client connection:*****\n");
        //connections[index] = accept(listenfd, (struct sockaddr*)NULL, NULL);


        clientAddressLength=sizeof(clientAddress);
        connections[index]=accept(listenfd,(struct sockaddr*)&clientAddress,&clientAddressLength);
        printf("connected to client: %s\n",inet_ntoa(clientAddress.sin_addr));


        pid=fork();
        if(pid==0){//child process rec and send
            //rceive from client
            while(1){           
                n=recv(connections[index],msg,MAXSZ,0);
                if(n==0){
                    close(connections[index]);
                    break;
                }
                msg[n]=0;
                send(connections[index],msg,n,0);
                printf("Receive and set:%s\n",msg);
            }
            exit(0);
        }
        else{
            close(connections[index]);//sock is closed BY PARENT
        }
    }
    return 0;
}


