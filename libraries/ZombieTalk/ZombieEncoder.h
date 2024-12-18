/*
Name:		ZombieEncoder.h
Created:	7/28/2018 12:34:41 AM
Author:		Richard Freitag
Description: Encoder for the actual byte values. Here is where the bit and byte shifting happens
*/

#pragma once
#if ARDUINO >= 100
    #include "Arduino.h"
#else
    #include <stdint.h>
    typedef uint8_t byte;
#endif

	/*
	Message types defines the individual message that can be send
	1-100: basic message types, e.g. for UINT8, UINT16
	101-200: small message types, e.g. for temperature, humidity or more
	201-255: custom message types, e.g. for latidute/longitude or more complex types
	*/
	enum MessageType {
		TYPE_UNDEFINED 				=	0,
		TYPE_UINT8 					=	1,
		TYPE_UINT16 				=	2,
		TYPE_UINT32 				=	3,
		TYPE_ATMEGA328P_ID  		=	51,		// Chip IDs are mapped to https://github.com/ricaun/ArduinoUniqueID
		TYPE_UNIXTIME 				=	101,
		TYPE_VOLTAGE 				=	102,
		TYPE_TEMPERATURE 			=	103,
		TYPE_HUMIDITY 				=	104,
		TYPE_BITMAP 				=	105,	// bit-mask encoded in a byte
		TYPE_HEARTBEAT 				=	201,	// heartbeat
		TYPE_TVC 					=	202,	// temperature/vcc/count
		TYPE_LVC 					=	203,	// lux/vcc/count
		TYPE_DVC 					=	204,	// Dhrystones per Second/vcc/count ( Vax_Mips = Dhrystones_Per_Second / 1757.0; ) 
		TYPE_MNISTRESULT 			=	205,	// Result of Mnist predictor
		TYPE_VC						=	206,	// vcc/count
		TYPE_MNISTARRAY				=	207,	// Array of 25 MNIST predictions
		TYPE_MNISTIMAGEROW  		=	208,	// Array of MNIST image row containing 14 bytes of image data
		TYPE_RELAYED_MNISTRESULT	= 	225		// Mnist result that went through a relay
	};

class ZombieEncoder {
    public:
        ZombieEncoder(byte *buffer);
		void writeUint8(uint8_t i);
		void writeUint16(uint16_t i);
		void writeUint32(uint32_t i);
		void writeUnixtime(uint32_t unixtime);
        void writeLatLng(float latitude, float longitude);
        void writeTemperature(float temperature);
        void writeHumidity(float humidity);
        void writeBitmap(bool a, bool b, bool c, bool d, bool e, bool f, bool g, bool h);
    private:
        byte* _buffer;
        void _intToBytes(byte *buf, int32_t i, uint8_t byteSize);
};
