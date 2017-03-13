#include <stdio.h> //printf
#include <stdlib.h>
#include <unistd.h>
#include <string.h>    //strlen
#include <sys/socket.h>    //socket
#include <arpa/inet.h> //inet_addr
 
#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

#define MAXLINE 4096
#define max(a,b)    ((a) > (b) ? (a) : (b))

void client_rec_send(FILE *fp, int sockfd){
    int maxfdp1;
    int stdineof;
    int i;
    fd_set rset;
    char buf[MAXLINE];
    char message_to_print[MAXLINE];
    int n;
    char lines[1024];
    stdineof = 0;

    FD_ZERO(&rset);
    while(1){
        strcpy(buf,"");
        if (stdineof == 0)
            FD_SET(fileno(fp), &rset); //adds fp to set <<rset>>
        FD_SET(sockfd, &rset); //adds sockfd to rset
        maxfdp1 = max(fileno(fp), sockfd) + 1;
        select(maxfdp1, &rset, NULL, NULL, NULL);

        //reading from socket - writing to stdout
        if (FD_ISSET(sockfd, &rset)){
            if ((n = read(sockfd, buf, MAXLINE)) == 0){ 
                
                if (stdineof == 1)
                    return;
                else{
                    printf("server terminated...\n");
                    exit(1);
                }
            }
            
            write(fileno(stdout), buf, n);
            strcpy(buf,"");

        }

        //reading from stdin - writing to socket
        if (FD_ISSET(fileno(fp), &rset)){
            if ((n = read(fileno(fp), buf, MAXLINE)) == 0){
                
                stdineof = 1;
                shutdown(sockfd, SHUT_WR);
                FD_CLR(fileno(fp), &rset);
                continue;
            }
           
            write(sockfd, buf, n);
            strcpy(buf,"");
        }
    }
}


int main(int argc , char *argv[]){
    int sock;
    struct sockaddr_in server;
    char message[1000] , server_reply[2000];
     
    //Create socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)    {
        printf("Could not create socket");
    }
    puts("Socket created");
     
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons( 8888 );
 
    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)    {
        perror("connect failed. Error");
        return 1;
    }
     
    puts("Connected\n");
    
    client_rec_send(stdin, sock);
     
    close(sock);
    return 0;
}