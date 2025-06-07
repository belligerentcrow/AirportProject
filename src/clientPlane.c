#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include "lib-misc.h"
#include <time.h>
#include "protocolStructures.h"
#include "coordinatesMethods.h"

#define MAX_ALTITUDE 3000

int socketfd;


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
    srand(time(NULL));
    int numBytes;
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
    Coordinates initCoord = generatePoint();
    AirplaneInfo plane = {
        .timestamp = time(NULL),
        .latitude = initCoord.latitude,
        .longitude = initCoord.longitude,
        .altitude = initCoord.altitude,
        .departure = "AAAA",
        .arrival = "ZZZZ"
    };
    strcpy(plane.flightcode, argv[3]);
    strcpy(plane.message, initCoord.message);
    printf("[%s - %d] AT LAT: %4.6f, LONG: %4.6f, ALT: %u; FROM: %s TO: %s, STATE: %s\n", plane.flightcode, plane.timestamp, plane.latitude, plane.longitude, plane.altitude, plane.departure, plane.arrival, plane.message);
    Package p= {
        .type = MSG_DATA
    };
    memcpy(p.payload, &plane, sizeof(plane));
    send(socketfd, &p, sizeof(p), 0);


    

    pthread_t threadPosition, threadSend, threadReceive;
    pthread_create(&threadPosition, NULL, (void*)threadChangePosition, NULL);
    pthread_create(&threadSend, NULL, (void*)threadInvio, NULL);
    pthread_create(&threadReceive, NULL, (void*)threadRicezione, NULL);

    close(socketfd);
    return 0;
}