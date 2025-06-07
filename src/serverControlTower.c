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
    printf("New plane on radar. CODE: %s, PORT %d --- FROM: %s TO: %s. INITIAL LAT: %4.4f, INITIAL LONG:%4.4f\n", info->planeCode, info->address.sin_port, info->departure, info->arrival, info->latitude, info->longitude);

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
                if(planeslist.planes[i]->sockfd == info->address.sin_port){
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

        if(pack.type == MSG_DATA){
            AirplaneInfo packPlaneInfo;
            memcpy(&packPlaneInfo, pack.payload, sizeof(AirplaneInfo));
            printf("[%s - %d] AT LAT: %4.6f, LONG: %4.6f, ALT: %u; FROM: %s TO: %s, STATE: %s\n", packPlaneInfo.flightcode, packPlaneInfo.timestamp, packPlaneInfo.latitude, packPlaneInfo.longitude, packPlaneInfo.altitude, packPlaneInfo.departure, packPlaneInfo.arrival, packPlaneInfo.message);

        }else if(pack.type == MSG_ALERT){
            AlertData packAlertData;
        //}else if(pack.type == MSG_COORDINATES){
        //    Coordinates packCoordinates;
        //
        //}else{
            printf("ERROR Serv Receiving package");
            break;
        }


        //buffer[bytes_received] = '\0';
        //printf("[%s - PORT:%d] ", info->planeCode, info->address.sin_port, )

    }
    close(plane_socket);
}

void invioMessaggio(const char * message, int sender_fd, int receiver_fd){

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
    printf("ok\n");
    memset(&serverSockAddr, '\0', sizeof(serverSockAddr));
    printf("one\n");
    serverSockAddr.sin_family = AF_INET;
    printf("two\n");
    serverSockAddr.sin_port = htons(port);
    printf("three\n");
    serverSockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    printf("bind! It's a server\n");
    //bind! It's a server
    if(bind(sockfd, (struct sockaddr *) &serverSockAddr, sizeof(serverSockAddr))<0){
        exit_with_sys_err("bind failed");
    }

    printf("Listen! It's a server\n");
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

    while(true){
        tracked_plane * plane = malloc(sizeof(tracked_plane));
        socklen_t socklenpl = sizeof(plane->address);

        if((plane->sockfd = accept(serverSocketFD, (struct sockaddr *)&plane->address, &socklenpl))<0){
            printf("Accept failed");
        }




    }

    printf("Chiusura server...\n");
    pthread_mutex_destroy(&planeslist.mutex);
    close(serverSocketFD);
    exit(EXIT_SUCCESS);
}