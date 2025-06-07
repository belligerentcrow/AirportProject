#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include "lib-misc.h"
#include <time.h>
#include <stdbool.h>
#include "protocolStructures.h"
#include "coordinatesMethods.h"

#define MAX_ALTITUDE 3000
#define COORDINATES_TO_TOWER "AFIS TX COOR TO TWR"

int socketfd;

typedef struct{
    char planecode[7];
    uint32_t timestamp;
    float latitude;
    float longitude;
    uint8_t altitude;
    char departure[5];
    char arrival[5];
    pthread_mutex_t mutex;
} Airplane_status;

Airplane_status airplaneStatus;

//Initializes structures when plane is created
AirplaneInfo initStructures(char * planecode, char * from, char * to){
    //generate initial coordinates
    Coordinates initCoord = generatePoint();
    
    //generate plane
    AirplaneInfo plane = {
        .timestamp = time(NULL),
        .latitude = initCoord.latitude,
        .longitude = initCoord.longitude,
        .altitude = initCoord.altitude,
    }; 
    //strcpy for strings
    strcpy(plane.flightcode, planecode);
    strcpy(plane.departure, from);
    strcpy(plane.arrival, to);
    strcpy(plane.message, COORDINATES_TO_TOWER);

    //initializing plane status too
    pthread_mutex_lock(&airplaneStatus.mutex);
    strcpy(airplaneStatus.planecode, planecode);
    airplaneStatus.timestamp = time(NULL);
    airplaneStatus.latitude = plane.latitude;
    airplaneStatus.longitude = plane.longitude;
    airplaneStatus.altitude = plane.altitude;
    strcpy(airplaneStatus.departure, from);
    strcpy(airplaneStatus.arrival, to);
    pthread_mutex_unlock(&airplaneStatus.mutex);

    //plane presentation
    printf("[%s - %d] AT LAT: %4.6f, LONG: %4.6f, ALT: %u; FROM: %s TO: %s, STATE: %s\n", plane.flightcode, plane.timestamp, plane.latitude, plane.longitude, plane.altitude, plane.departure, plane.arrival, plane.message);
    return plane;
}



void threadRicezione(void * argv){
    Package packrecv;
    while(true){
        ssize_t bytesRecv = recv(socketfd, &packrecv, sizeof(packrecv), 0);
        if(bytesRecv == 0){
            exit_with_sys_err("Server Closed the Connection");

        }else{
            //logica smistamento pacchetti
            //client can only get ALERTs from controltower
            if(packrecv.type==MSG_ALERT){
                AlertData alertPack;
                memcpy(&alertPack, packrecv.payload, sizeof(AlertData));
                printf("URGENT MESSAGE FROM CONTROL TOWER [%4.6f %4.6f]: %s\n", TOWER_LATITUDE,TOWER_LONGITUDE, alertPack.message);
            }else{
                printf("ERROR Client Receiving Package");
                break;
            }
        }
        fflush(stdout);
    }
}

Coordinates threadChangePosition(void *argv){
    //for now it generates a random point around the tower-- 
    //will update in the future to change position in a more coherent way
    while(true){
        Coordinates newCoordinates = generatePoint();
        pthread_mutex_lock(&airplaneStatus.mutex);
        airplaneStatus.latitude = newCoordinates.latitude;
        airplaneStatus.longitude = newCoordinates.longitude;
        airplaneStatus.altitude = newCoordinates.altitude;
        airplaneStatus.timestamp = time(NULL);
        pthread_mutex_unlock(&airplaneStatus.mutex);
        sleep(3);
    }
}

void threadInvio(void *argv){

    while(true){
        //srand(time(NULL));
        Coordinates coordsToSend;
        uint32_t currentTime;
        char pcode[7];
        pthread_mutex_lock(&airplaneStatus.mutex);
        strcpy(pcode, airplaneStatus.planecode);
        coordsToSend.latitude = airplaneStatus.latitude;
        coordsToSend.longitude = airplaneStatus.longitude;
        coordsToSend.altitude = airplaneStatus.altitude;
        currentTime = airplaneStatus.timestamp;
        pthread_mutex_unlock(&airplaneStatus.mutex);
        Coordinates coordsPack = {
            .latitude = coordsToSend.latitude,
            .longitude = coordsToSend.longitude,
            .altitude = coordsToSend.altitude,
            .planetimestamp = currentTime,
        };
        strcpy(coordsPack.planecode, pcode);
        strcpy(coordsPack.message, COORDINATES_TO_TOWER);
        Package p ={
            .type = MSG_COORDINATES
        };
        memcpy(p.payload, &coordsPack,sizeof(coordsPack));

        send(socketfd, &p, sizeof(p), 0);

        sleep(3);
    }
}

int main(int argc, char * argv[]){
    if( (argc != 6) || (strlen(argv[3])!=6) || (strlen(argv[4])!=4) || (strlen(argv[5])!=4)){
        printf("Usage: %s <IP> <PORT> <PLANECODE (6 CHAR)> <DEPARTURE FROM (4 CHAR)> <ARRIVAL TO (4 CHAR)>",argv[0]);
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

    AirplaneInfo airplane = initStructures(argv[3], argv[4], argv[5]);
    /*
    Package p= {
        .type = MSG_DATA
    };
    memcpy(p.payload, &airplane, sizeof(airplane));
    send(socketfd, &p, sizeof(p), 0);
    */

    pthread_t threadPosition, threadSend, threadReceive;
    pthread_create(&threadPosition, NULL, (void*)threadChangePosition, NULL);
    pthread_create(&threadSend, NULL, (void*)threadInvio, NULL);
    pthread_create(&threadReceive, NULL, (void*)threadRicezione, NULL);

    pthread_join(threadSend, NULL);
    pthread_join(threadReceive, NULL);
    pthread_join(threadPosition, NULL);
    //pthread_create(&threadPosition, NULL, (void*)threadRicezione, NULL);


    pthread_mutex_destroy(&airplaneStatus.mutex);
    close(socketfd);
    return 0;
}