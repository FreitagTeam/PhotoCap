/*
Name:		ZombieDatagram.h
Created:	7/28/2018 12:34:41 AM
Author:		Richard Freitag
Description: The datagram containing the actual bit and byte values that need to be transmitted/received
*/

#pragma once
#if ARDUINO >= 100
    #include "Arduino.h"
#else
    #include <stdint.h>
    typedef uint8_t byte;
#endif

#include "ZombieEncoder.h"

class ZombieDatagram {
    public:
        ZombieDatagram();
        ~ZombieDatagram();
        ZombieDatagram& addUnixtime(uint32_t unixtime);
        ZombieDatagram& addLatLng(double latitude, double longitude);
        ZombieDatagram& addUint8(uint8_t i);
        ZombieDatagram& addUint16(uint16_t i);
        ZombieDatagram& addUint32(uint32_t i);
        ZombieDatagram& addTemperature(float temperature);
        ZombieDatagram& addHumidity(float humidity);
        ZombieDatagram& addBitmap(bool a, bool b, bool c, bool d, bool e, bool f, bool g, bool h);
		
		void getUint8(uint8_t &j, uint16_t pos);
		void getUint16(uint16_t &j, uint16_t pos);
		void getUint32(uint32_t &j, uint16_t pos);
		void getUnixtime(uint32_t &unixtime, uint16_t pos);
		void getLatLng(double &latitude, double &longitude, uint16_t pos);
        void getTemperature(float &temperature, uint16_t pos);
		void getDhrystoneMips(float &DhrystoneMips, uint16_t pos);
		void getHumidity(float &humidity, uint16_t pos);
        void getBitmap(bool &a, bool &b, bool &c, bool &d, bool &e, bool &f, bool &g, bool &h, uint16_t pos);

        byte* getBytes();
		void setBytes(byte* buffer);
        int getLength();
    private:
        ZombieEncoder _reallocBuffer(int delta);
        byte* _buffer;
        int _currentSize;
};
