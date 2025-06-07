#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
//#include "coordinatesMethods.h"
#include "lib-misc.h"

#define M_PI  3.14159265358979323846
#define TOWER_LATITUDE 55.95045 
#define TOWER_LONGITUDE -3.36061

typedef struct{
	char flightcode[7];
	uint32_t timestamp;
	float latitude;
	float longitude;
	uint16_t altitude;
	char departure[5];
	char arrival[5];
    char message[32];
} AirplaneInfo;

typedef struct {
	char flightcode[7];
	uint32_t timestamp;
	uint8_t alertLevel;
    char message[48];
} AlertData;

typedef struct{
    char planecode[7];
	float latitude;
	float longitude;
	uint16_t altitude;
    uint32_t timestamp;
	char message[40];
} Coordinates;

//infos eee = {43.03129, "LIFO", "FIFO", 128};
int main(int argc, char * argv[]){
    //if((argc != 2) || (strlen(argv[1])!=6)){
    //    printf("Usage: %s <PLANECODE (6 CHAR)>\n",argv[0]);
    //    exit(EXIT_FAILURE);
    //}
    srand(time(NULL));
    //printf("argv[1] = %s, Sizeof argv[1]: %ld \n", argv[1], strlen(argv[1]));
    time(0);
    /*Coordinates mycoordinates = generatePoint();
    
    AirplaneInfo flight = {.timestamp=time(NULL), .altitude=mycoordinates.altitude, .latitude=mycoordinates.latitude, .longitude=mycoordinates.longitude, .departure="LAXP", .arrival="EDNB"};
    strcpy(flight.flightcode, argv[1]);
    
    printf("flightcode: %s, size: %ld\n", flight.flightcode, sizeof(flight.flightcode));
    printf("timestamp: %u, size: %ld\n", flight.timestamp, sizeof(flight.timestamp));
    printf("latitude: %4.4f, size: %ld\n", flight.latitude, sizeof(flight.latitude));
    printf("longitude: %4.4f, size: %ld\n", flight.longitude, sizeof(flight.longitude));
    printf("altitude: %hhu, size: %ld\n", flight.altitude, sizeof(flight.altitude));
    printf("departure: %s, size: %ld\n", flight.departure, sizeof(flight.departure));
    printf("arrival: %s, size: %ld\n", flight.arrival, sizeof(flight.arrival));
    printf("flight entire size: %ld\n", sizeof(flight));
    printf("Size of 2 floats + 1 int: %ld\n\n", sizeof(float)+sizeof(float)+sizeof(uint16_t));
        */
    printf("Size of AlertData: %ld \n", sizeof(AlertData));
    printf("Size of AirplaneInfo: %ld \n", sizeof(AirplaneInfo));
    printf("Size of Coordinates: %ld \n", sizeof(Coordinates));
    printf("Generating coordinates...\n\n");
    /*
    Coordinates newcoor = generatePoint();*/
    //printf("coordinates: %4.6f, %4.6f, %u, %s --- Sizeof coordiantes = %ld\n", newcoor.latitude, newcoor.longitude, newcoor.altitude, newcoor.message, sizeof(newcoor));
    printf("Sizeof coordinates: %ld", sizeof(Coordinates));

}