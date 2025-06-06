#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include "lib-misc.h"
#include "protocolStructures.h"



void threadChangePosition(void *argv){

}

void threadRicezione(void * argv){

}

void threadInvio(void *argv){

}

int main(int argc, char * argv[]){
    if((argc != 4) || (strlen(argv[3])!=6)){
        printf("Usage: %s <IP> <PORT> <PLANECODE (6 CHAR)>",argv[0]);
        exit_with_err("Usage Error",1);
    }
    int socketfd, numBytes;
    struct sockaddr_in socketAddress;
    
    if((socketfd = socket(PF_INET, SOCK_STREAM, 0))<0){
        exit_with_sys_err("socket creation");
    }

    memset(&socketAddress, '\0', sizeof(socketAddress));
    socketAddress.sin_family = AF_INET;
    socketAddress.sin_port = htons(atoi(argv[2]));
    socketAddress.sin_addr.s_addr = inet_addr(argv[1]);

    if(connect(socketfd, (struct sockaddr *)&socketAddress, sizeof(socketAddress))<0){
        close(socketfd);
        exit_with_sys_err("Connect");
    }
    printf("Connesso...\n");

    pthread_t threadPosition, threadSend, threadReceive;
    pthread_create(&threadPosition, NULL, (void*)threadChangePosition, NULL);
    pthread_create(&threadSend, NULL, (void*)threadInvio, NULL);
    pthread_create(&threadReceive, NULL, (void*)threadRicezione, NULL);

    close(socketFileDescriptor);
    return 0;
}