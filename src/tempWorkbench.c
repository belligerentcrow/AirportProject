#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "coordinatesMethods.h"
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


//infos eee = {43.03129, "LIFO", "FIFO", 128};
int main(int argc, char * argv[]){
    
    srand(time(NULL));
    
    time(0);
    printf("Size of AlertData: %ld \n", sizeof(AlertData));
    printf("Size of AirplaneInfo: %ld \n", sizeof(AirplaneInfo));
    printf("Size of Coordinates: %ld \n", sizeof(Coordinates));
    printf("Generating coordinates...\n\n");
    Coordinates x = generatePoint();
    sleep(3);
    Coordinates y = generatePoint();
    double dist = calcdistance((double)x.latitude, (double) x.longitude, (double)y.latitude, (double) y.longitude);
    printf("x: %4.6f %4.6f; y: %4.6f %4.6f --- dist: %4.6f\n", x.latitude, x.longitude, y.latitude, y.longitude, dist);

    /*
    Coordinates newcoor = generatePoint();*/
    //printf("coordinates: %4.6f, %4.6f, %u, %s --- Sizeof coordiantes = %ld\n", newcoor.latitude, newcoor.longitude, newcoor.altitude, newcoor.message, sizeof(newcoor));
    printf("Sizeof coordinates: %ld\n", sizeof(Coordinates));

}