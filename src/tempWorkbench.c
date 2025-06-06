#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <stdint.h>
#include <string.h>
#include "lib-misc.h"

typedef struct  {
    char planeCode[7];
    uint32_t timestamp;
    float latitude;
    float longitude;
    uint8_t altitude;
    char departure[5];
    char arrival[5];
} PlaneData;

typedef struct{
	char flightcode[7];
	uint32_t timestamp;
	float latitude;
	float longitude;
	uint16_t altitude;
	char departure[5];
	char arrival[5];
} airplaneInfo;

//infos eee = {43.03129, "LIFO", "FIFO", 128};
int main(int argc, char * argv[]){
    if((argc != 2) || (strlen(argv[1])!=6)){
        printf("Usage: %s <PLANECODE (6 CHAR)>\n",argv[0]);
        exit(EXIT_FAILURE);
    }

    printf("Sizeof argv[1]: %ld \n", strlen(argv[1]));
    time(0);
    airplaneInfo flight={"BRO420",time(0),12.3842,29.9842,5000,"LPAD","FHHE"};
    //printf("%4.4f, %s, %s\n", eee.flolo, eee.departure2, eee.departure2);
    //printf("%ld, %ld, %ld, %lu and total struct weight: %ld\n", sizeof(eee.flolo), sizeof(eee.departure2), sizeof(eee.arrival2), sizeof(eee.jeje), sizeof(eee));
    printf("flightcode: %s, size: %ld\n", flight.flightcode, sizeof(flight.flightcode));
    printf("timestamp: %u, size: %ld\n", flight.timestamp, sizeof(flight.timestamp));
    printf("latitude: %4.4f, size: %ld\n", flight.latitude, sizeof(flight.latitude));
    printf("longitude: %4.4f, size: %ld\n", flight.longitude, sizeof(flight.longitude));
    printf("altitude: %hhu, size: %ld\n", flight.altitude, sizeof(flight.altitude));
    printf("departure: %s, size: %ld\n", flight.departure, sizeof(flight.departure));
    printf("arrival: %s, size: %ld\n", flight.arrival, sizeof(flight.arrival));
    printf("flight entire size: %ld\n", sizeof(flight));



}