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

int* connections;
int listenfd = 0;


void initialize_server();

void send_message(){

}

int main(int argc, char *argv[]){

    char sendBuff[4096];
    initialize_server();


    int index = 0;
    while(1){
        connections[index] = accept(listenfd, (struct sockaddr*)NULL, NULL);
	printf("%d\n",connections[index]);

	strcpy(sendBuff,"Ana are mere!");
	strcat(sendBuff,"\0");
	printf("%s\n",sendBuff);
	snprintf(sendBuff, sizeof(sendBuff),"Ana are mere!");

        write(connections[index], sendBuff, strlen(sendBuff));
	
	close(connections[index]);
        sleep(1);
        index++;
     }

}

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
