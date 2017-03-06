#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

#include "common.h"


#define BUFFER_SIZE 4096
#define PORT 5000
#define MAXSZ 100

int main(int argc, char *argv[]) {
    int sockfd = 0;
    int n = 0;
    char recvBuff[BUFFER_SIZE];
    struct sockaddr_in serv_addr;

    char msg1[MAXSZ];
    char msg2[MAXSZ];


    memset(recvBuff, '0', sizeof(recvBuff));
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Error : Could not create socket \n");
        return 1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\n inet_pton error occured\n");
        return 1;
    }

    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        printf("\n Error : Connect Failed \n");
        return 1;
    }

    //send to sever and receive from server
     while(1){
        printf("\nEnter message to send to server:\n");
        fgets(msg1,MAXSZ,stdin);
        if(msg1[0]=='#')
            break;

        n=strlen(msg1)+1;
        send(sockfd,msg1,n,0);

        n=recv(sockfd,msg2,MAXSZ,0);
        printf("Receive message from  server::%s\n",msg2);
     }

    return 0;
}
