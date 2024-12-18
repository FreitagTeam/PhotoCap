#ifndef BONSAINET_H
#define BONSAINET_H

#include <SimpleSleep.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "bonsaiconfig.h"

extern RF24 radio; // CE, CSN
extern SimpleSleep simpleSleep;
extern SimpleSleep_Cal SleepCal;
extern uint8_t comChannel;

enum MessageType {
	TYPE_UNDEFINED 				=	0,
	TYPE_UINT8 					=	1,
	TYPE_UINT16 				=	2,
	TYPE_UINT32 				=	3,
	TYPE_ATMEGA328P_ID  		=	51,		// Chip IDs are mapped to https://github.com/ricaun/ArduinoUniqueID
	TYPE_STANDARD				=   100,
	TYPE_UNIXTIME 				=	101,
	TYPE_VOLTAGE 				=	102,
	TYPE_TEMPERATURE 			=	103,
	TYPE_HUMIDITY 				=	104,
	TYPE_BITMAP 				=	105,	// bit-mask encoded in a byte
	TYPE_STARTUP				=   106,    // node startup message
	TYPE_RELAY_RESYNC			=   107,	// relay resynchronization message
	TYPE_HEARTBEAT 				=	201,	// heartbeat
	TYPE_TVC 					=	202,	// temperature/vcc/count
	TYPE_LVC 					=	203,	// lux/vcc/count
	TYPE_DVC 					=	204,	// Dhrystones per Second/vcc/count ( Vax_Mips = Dhrystones_Per_Second / 1757.0; ) 
	TYPE_MNISTRESULT 			=	205,	// Result of Mnist predictor
	TYPE_VC						=	206,	// vcc/count
	TYPE_MNISTARRAY				=	207,	// Array of 25 MNIST predictions
	TYPE_MNISTIMAGEROW  		=	208,	// Array of MNIST image row containing 14 bytes of image data
	TYPE_RELAYED_MNISTRESULT	= 	225,	// Mnist result that went through a relay
	TYPE_RELAYED_VC				=   226	// VC message that went through a relay
};

enum DeviceType { SOURCE,
                  RELAY,
                  HOP,
                  FOG };

void setCommunicationSource(DeviceType deviceType);
void setCommunicationTarget(DeviceType deviceType);
void setDeviceType(DeviceType deviceType);

void setAsRelayFromSourceToFog();
void setAsRelayFromSourceToHop();
void setAsHopFromRelayToFog();

void setDeviceId(uint8_t deviceId);

void initRadio();
void initRadio(uint8_t level);
void initReading();
void initWriting();

bool sendStartupMessage(int16_t vcc, uint32_t messageCount, uint8_t sleepLoops);
bool sendRelayResyncMessage(int16_t vcc, uint32_t messageCount, uint8_t sleepLoops);

bool relayMessage(char data[BUFFERSIZE]);

bool sendVCMessage(int16_t vcc, uint32_t messageCount, uint8_t sleepLoops);
bool relayVCMessage(char data[BUFFERSIZE], int16_t vcc, uint8_t sleepLoops);

void sendMnistImage(byte image[196], int16_t vcc, uint32_t messageCount, uint8_t sleepLoops);
bool sendMnistImageRow(byte image[196], uint8_t imageRow, int16_t vcc, uint32_t messageCount, uint8_t sleepLoops);
bool sendMnistResult(byte result, int16_t vcc, uint32_t messageCount, uint8_t sleepLoops, uint8_t deviceId);
bool relayMnistResult(char data[BUFFERSIZE], int16_t vcc, uint8_t sleepLoops);

void setVcc(char data[BUFFERSIZE], int16_t vcc);
void setMessageCount(char data[BUFFERSIZE], uint32_t messageCount);

void relaySleep1(bool packageReceived, uint8_t sleepLoops, uint8_t remoteSleepLoops, int16_t sleepLoopsOffset, int16_t receivedSleepLoopsOffset);

#endif