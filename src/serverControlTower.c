#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdbool.h>
#include "lib-misc.h"
#include "protocolStructures.h"
#include "coordinatesMethods.h"

#define MAX_PLANES 20
#define PORT 8080
#define MAX_BUFFER 1024;
#define TOWER_LATITUDE 55.95045 
#define TOWER_LONGITUDE -3.36061
#define MAX_RADIUS_FROM_TOWER 10000 //10km

typedef struct{
    int sockfd;
    struct sockaddr_in address;
    char planeCode[7];
	uint32_t timestamp;
	float latitude;
	float longitude;
	uint8_t altitude;
	char departure[5];
	char arrival[5];
} tracked_plane;

typedef struct {
    tracked_plane *planes[MAX_PLANES];
    int count;
    pthread_mutex_t mutex;
} planes_list;

planes_list planeslist = {.count=0};


void gestioneAerei(void *argv){
    tracked_plane * info = (tracked_plane*)argv;
    int plane_socket = info->sockfd;
    Package pack;
    char plane_ip[INET_ADDRSTRLEN];

    inet_ntop(AF_INET, &info->address.sin_addr, plane_ip, INET_ADDRSTRLEN);
    int plane_port = ntohs(info->address.sin_port);
    

    //first contact with a plane
    pthread_mutex_lock(&planeslist.mutex);
    planeslist.planes[planeslist.count++] = info;
    pthread_mutex_unlock(&planeslist.mutex);

    while(true){

        //struct PlaneData receivedPlaneData;
        ssize_t bytes_received = recv(plane_socket, &pack, sizeof(pack), 0);
        
        //gestire disconnessione
        if(bytes_received==0){
            printf("PLANE %s - %d OFF THE RADAR\n", plane_ip, info->address.sin_port);
            pthread_mutex_lock(&planeslist.mutex); //sezione critica
            for(int i =0; i < planeslist.count; i++){
                //rimuovo aereo dai tracked planes
                if(planeslist.planes[i]->sockfd == info->sockfd){
                    free(planeslist.planes[i]); // l'aereo vola via
                    //sistemo l'array con tutti gli altri
                    for(int j = i; j < planeslist.count -1; j++){
                        planeslist.planes[j] = planeslist.planes[j+1];
                    }
                    planeslist.count--;
                    break;
                }
            }
            pthread_mutex_unlock(&planeslist.mutex); //fuori dalla sezione critica
            break;
        }
        //logica di smistamento pacchetti
        if(pack.type == MSG_DATA){
            printf("DATA UPDATE \n");
            AirplaneInfo packPlaneInfo;
            memcpy(&packPlaneInfo, pack.payload, sizeof(AirplaneInfo));
            printf("[%s - %d] AT LAT: %4.6f, LONG: %4.6f, ALT: %u; FROM: %s TO: %s, STATE: %s\n", packPlaneInfo.flightcode, packPlaneInfo.timestamp, packPlaneInfo.latitude, packPlaneInfo.longitude, packPlaneInfo.altitude, packPlaneInfo.departure, packPlaneInfo.arrival, packPlaneInfo.message);

            //since MSG_DATA should be the first packet to arrive it should work
            //the plane with the first data packet should also be the latter one in list
            //otherwise, i need to figure out a better way to identify it
            pthread_mutex_lock(&planeslist.mutex);
            strcpy(planeslist.planes[planeslist.count-1]->planeCode, packPlaneInfo.flightcode);
            planeslist.planes[planeslist.count-1]->latitude = packPlaneInfo.latitude;
            planeslist.planes[planeslist.count-1]->longitude = packPlaneInfo.longitude;
            planeslist.planes[planeslist.count-1]->altitude = packPlaneInfo.altitude;
            planeslist.planes[planeslist.count-1]->timestamp = packPlaneInfo.timestamp;
            strcpy(planeslist.planes[planeslist.count-1]->departure, packPlaneInfo.departure);
            strcpy(planeslist.planes[planeslist.count-1]->arrival, packPlaneInfo.arrival);
            pthread_mutex_unlock(&planeslist.mutex);   

        }else if(pack.type == MSG_ALERT){
            AlertData packAlertData;
            memcpy(&packAlertData,pack.payload, sizeof(AlertData));
            printf("URGENT MESSAGE ALERT LEVEL %d FROM %s - %d. MESSAGE: %s", packAlertData.alertLevel, packAlertData.flightcode, packAlertData.timestamp, packAlertData.message);

        }else if(pack.type == MSG_COORDINATES){
            Coordinates packCoordinates;
            memcpy(&packCoordinates, pack.payload, sizeof(Coordinates));
            printf("[%s - %d] COORDINATES: %4.6f %4.6f %u -- MSG: %s\n", packCoordinates.planecode, packCoordinates.planetimestamp, packCoordinates.latitude, packCoordinates.longitude, packCoordinates.altitude, packCoordinates.message);
            //updating structures lockign with mutex to prevent race condition
            pthread_mutex_lock(&planeslist.mutex);
            for(int i =0; i<planeslist.count; i++){
                if(strcmp(planeslist.planes[i]->planeCode, packCoordinates.planecode)==0){
                    planeslist.planes[i]->latitude = packCoordinates.latitude;
                    planeslist.planes[i]->longitude = packCoordinates.longitude;
                    planeslist.planes[i]->altitude = packCoordinates.altitude;
                    planeslist.planes[i]->timestamp = packCoordinates.planetimestamp;
                }
            }
            pthread_mutex_unlock(&planeslist.mutex);
        }else{
            printf("ERROR Serv Receiving package");
            break;
        }
    }
    close(plane_socket);
}

void checkDistances(){
    while(true){
        int tot =0;
        //enter critical region - because the number of planes might change!
        pthread_mutex_lock(&planeslist.mutex);
        tot = planeslist.count;
        //check each plane distances with the others in the list

        for(int i =0; i<tot-1; i++){
            Coordinates plane1 = {
                .latitude = planeslist.planes[i]->latitude,
                .longitude = planeslist.planes[i]->longitude
            };
            for(int j =i+1; j<tot; j++){
                
                Coordinates plane2 = {
                .latitude = planeslist.planes[j]->latitude,
                .longitude = planeslist.planes[j]->longitude
                };

                //if two are very close to each other
                //printf("plane1: [%s] - %f %f -- plane2: [%s] - %f %f\n", planeslist.planes[i]->planeCode, planeslist.planes[i]->latitude, planeslist.planes[i]->longitude, planeslist.planes[j]->planeCode, planeslist.planes[j]->latitude, planeslist.planes[j]->longitude);
                if((calcdistance((double)plane1.latitude, (double)plane1.longitude, (double)plane2.latitude, (double)plane2.longitude)) < 5.0){
                    //then alert them!
                    printf("ALERT!! %s and %s TOO CLOSE\n", planeslist.planes[i]->planeCode, planeslist.planes[j]->planeCode);
                    Package p;
                    AlertData tooClosePack;
                    strcpy(tooClosePack.flightcode, "CTOWER");
                    strcpy(tooClosePack.message, "ALERT - TOO CLOSE TO ANOTHER PLANE.");
                    p.type = MSG_ALERT;
                    memcpy(p.payload, &tooClosePack, sizeof(tooClosePack));
                    send(planeslist.planes[i]->sockfd, &p, sizeof(p), 0);
                    send(planeslist.planes[j]->sockfd, &p, sizeof(p), 0);
                }
            }
        }
        pthread_mutex_unlock(&planeslist.mutex);
        sleep(3);
    }
}

int create_server(uint16_t port){
    printf("CreatingServer...\n");
    int sockfd;
    struct sockaddr_in serverSockAddr;
    socklen_t socklen = sizeof(serverSockAddr);

    if((sockfd = socket(PF_INET, SOCK_STREAM, 0))<0){
        close(sockfd);
        exit_with_sys_err("Socket creation");
    }

    //cleanup and populate socket
    memset(&serverSockAddr, '\0', sizeof(serverSockAddr));
    serverSockAddr.sin_family = AF_INET;
    serverSockAddr.sin_port = htons(port);
    serverSockAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    //bind! It's a server
    if(bind(sockfd, (struct sockaddr *) &serverSockAddr, sizeof(serverSockAddr))<0){
        exit_with_sys_err("bind failed");
    }

    //listen! it's a server
    if(listen(sockfd, MAX_PLANES)<0){
        exit_with_sys_err("listen failed");
    }
    
    return sockfd;
}

int main(int argc, char * argv[]){
    

    if((argc!=3) || (atoi(argv[1])<1024) || (strlen(argv[2]) !=4)){
        printf("Usage: %s <PORT> <AIRPORT CODE (4 CHAR)>\n", argv[0]);
        if(atoi(argv[1])<1024){
            printf("Non usare porte riservate. P > 1024\n");
        }
        exit_with_err("Usage Error",1);
    }
    char airportCode[5];
    strcpy(airportCode, argv[2]);

    int serverSocketFD = create_server(atoi(argv[1]));

    if(serverSocketFD <0){
        close(serverSocketFD);
        exit_with_sys_err("Creating server failed");
    }
    printf("Server in ascolto su %d\n",atoi(argv[1]));

    pthread_t threadAirplane, threadChecks;
    pthread_create(&threadChecks, NULL, (void *)checkDistances, 0);

    while(true){
        tracked_plane * plane = malloc(sizeof(tracked_plane));
        socklen_t socklenpl = sizeof(plane->address);

        if((plane->sockfd = accept(serverSocketFD, (struct sockaddr *)&plane->address, &socklenpl))<0){
            printf("Accept failed");
        }

        
        pthread_create(&threadAirplane, NULL, (void*)gestioneAerei, (void*)plane);
        pthread_detach(threadAirplane);
    }

    printf("Chiusura server...\n");
    pthread_join(threadChecks, NULL);
    pthread_mutex_destroy(&planeslist.mutex);
    close(serverSocketFD);
    exit(EXIT_SUCCESS);
}