#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define SIZE 64

#pragma pack(push, 1)
typedef enum {
    MSG_DATA,
    MSG_ALERT,
	MSG_COORDINATES
} TYPE_PAYLOAD;

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
	TYPE_PAYLOAD type;
	char payload[64]; //both payloads should be 64B long. will fix lengths
} Package;

#pragma pack(pop)