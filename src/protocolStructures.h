#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

typedef enum {
    MSG_DATA,
    MSG_ALERT
} MessageType;

#pragma pack(push, 1)
typedef struct{
	char flightcode[7];
	uint32_t timestamp;
	float latitude;
	float longitude;
	uint16_t altitude;
	char departure[5];
	char arrival[5];
} AirplaneInfo;

typedef struct {
    char message[33];
} AlertData;

#pragma pack(pop)