#include <time.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#define M_PI  3.14159265358979323846
#define TOWER_LATITUDE 55.95045 
#define TOWER_LONGITUDE -3.36061

#pragma pack(push, 1)
typedef struct{
    char planecode[7];
	float latitude;
	float longitude;
	uint16_t altitude;
    uint32_t planetimestamp;
	char message[40];
} Coordinates;
#pragma pack(pop)

float meters_to_lat(float meters){
    return meters/111000.0;
};

float meters_to_long(float meters, float lat){
    float lat_rad = lat * (M_PI /180.0);
    return meters / (111000.0 * cos(lat_rad));
}

int generateRadius(){
    return rand()%10000+1;
}

uint16_t generateAltitude(){
    return (uint16_t) rand()%10000+500;
}

Coordinates generatePoint(){
    srand(time(NULL));
    float randAngle = ((float)rand()/RAND_MAX)*2.0 * M_PI;
    int randRadius = generateRadius();
    uint16_t randAlt = generateAltitude();

    float offsLAT = meters_to_lat(randRadius * sin(randAngle));
    float offsLONG = meters_to_long(randRadius * cos(randAngle), TOWER_LATITUDE);

    float randLat = TOWER_LATITUDE + offsLAT;
    float randLong = TOWER_LONGITUDE + offsLONG;

    Coordinates mycoor = {.latitude=randLat, .longitude=randLong,.altitude=randAlt,.message="AFIS TX COOR TO TWR"};
    return mycoor;
}