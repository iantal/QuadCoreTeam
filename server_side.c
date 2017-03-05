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

#define BUFFER_SIZE 4096
#define PORT 5000

int main(int argc, char *argv[]){
    int listenfd = 0;
    int* connfd = calloc(100, sizeof(int));
    struct sockaddr_in serv_addr; 
    char sendBuff[BUFFER_SIZE];

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff)); 

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(PORT); 

    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 

    listen(listenfd, 1000); 
	
    int index = 0;
    while(1){
        connfd[index] = accept(listenfd, (struct sockaddr*)NULL, NULL);
	printf("%d\n",connfd[index]);

	strcpy(sendBuff,"Ana are mere!");
	strcat(sendBuff,"\0");
	printf("%s\n",sendBuff);
	snprintf(sendBuff, sizeof(sendBuff),"Ana are mere!");

        write(connfd[index], sendBuff, strlen(sendBuff));
	
	close(connfd[index]);
        sleep(1);
        index++;
     }

}
