#if ARDUINO >= 100
    #include "Arduino.h"
#endif
#include <stdlib.h>
#include "ZombieDatagram.h"
#include "ZombieEncoder.h"

ZombieDatagram::ZombieDatagram() {
    _currentSize = 0;
    _buffer = (byte*) malloc(_currentSize);
}

ZombieDatagram::~ZombieDatagram() {
    free(_buffer);
}

ZombieEncoder ZombieDatagram::_reallocBuffer(int delta) {
    void* temp = realloc(_buffer, (_currentSize + delta) * sizeof(byte));
    if (temp == NULL) {
        free(_buffer);
        printf("bad memory allocation!");
        while(true);
    } else {
        _buffer = (byte*) temp;
    }
    ZombieEncoder encoder(_buffer + _currentSize);
    _currentSize += delta;
    return encoder;
}

ZombieDatagram& ZombieDatagram::addUnixtime(uint32_t unixtime) {
    _reallocBuffer(4).writeUnixtime(unixtime);
    return *this;
}

ZombieDatagram& ZombieDatagram::addUint32(uint32_t i) {
    _reallocBuffer(4).writeUint32(i);
    return *this;
}


ZombieDatagram& ZombieDatagram::addLatLng(double latitude, double longitude) {
    _reallocBuffer(8).writeLatLng(latitude, longitude);
    return *this;
}

ZombieDatagram& ZombieDatagram::addUint16(uint16_t i) {
    _reallocBuffer(2).writeUint16(i);
    return *this;
}

ZombieDatagram& ZombieDatagram::addTemperature(float temperature) {
    _reallocBuffer(4).writeTemperature(temperature);
    return *this;
}

ZombieDatagram& ZombieDatagram::addUint8(uint8_t i) {
    _reallocBuffer(1).writeUint8(i);
    return *this;
}

ZombieDatagram& ZombieDatagram::addHumidity(float humidity) {
    _reallocBuffer(4).writeHumidity(humidity);
    return *this;
}

ZombieDatagram& ZombieDatagram::addBitmap(bool a, bool b, bool c, bool d, bool e, bool f, bool g, bool h) {
    _reallocBuffer(1).writeBitmap(a, b, c, d, e, f, g, h);
    return *this;
}

void ZombieDatagram::getUnixtime(uint32_t &unixtime, uint16_t pos) {
	uint8_t byteSize = 4;
	uint32_t i = 0;
	for(uint8_t x = 0; x < byteSize; x++) {
		i = (i << 8) + _buffer[pos+byteSize-1-x];
    }
	unixtime = i;
}

void ZombieDatagram::getUint32(uint32_t &j, uint16_t pos) {
	uint8_t byteSize = 4;
	uint32_t i = 0;
	for(uint8_t x = 0; x < byteSize; x++) {
		i = (i << 8) + _buffer[pos+byteSize-1-x];
    }
	j = i;
}


void ZombieDatagram::getLatLng(double &latitude, double &longitude, uint16_t pos) {
	uint8_t byteSize = 4;
	int32_t i = 0;
	for(uint8_t x = 0; x < byteSize; x++) {
		i = (i << 8) + _buffer[pos+byteSize-1-x];
    }
	latitude = i / 1e6;

	i = 0;
	for(uint8_t x = 0; x < byteSize; x++) {
		i = (i << 8) + _buffer[pos+4+byteSize-1-x];
    }
	longitude = i / 1e6;
}

void ZombieDatagram::getUint16(uint16_t &j, uint16_t pos) {
	uint8_t byteSize = 2;
	uint32_t i = 0;
	for(uint8_t x = 0; x < byteSize; x++) {
		i = (i << 8) + _buffer[pos+byteSize-1-x];
    }
	j = i;
}

void ZombieDatagram::getTemperature(float &temperature, uint16_t pos) {
	uint8_t byteSize = 4;
	uint32_t i = 0;
	for(uint8_t x = 0; x < byteSize; x++) {
		i = (i << 8) + _buffer[pos+byteSize-1-x];
    }
	temperature = i / 1e6;
}

void ZombieDatagram::getDhrystoneMips(float &DhrystoneMips, uint16_t pos) {
	uint8_t byteSize = 4;
	uint32_t i = 0;
	for (uint8_t x = 0; x < byteSize; x++) {
		i = (i << 8) + _buffer[pos + byteSize - 1 - x];
	}
	DhrystoneMips = i / 1e6;
}

void ZombieDatagram::getUint8(uint8_t &j, uint16_t pos) {
	uint8_t byteSize = 1;
	uint32_t i = 0;
	for(uint8_t x = 0; x < byteSize; x++) {
		i = (i << 8) + _buffer[pos+byteSize-1-x];
    }
	j = i;
}

void ZombieDatagram::getHumidity(float &humidity, uint16_t pos) {
	uint8_t byteSize = 4;
	uint32_t i = 0;
	for(uint8_t x = 0; x < byteSize; x++) {
		i = (i << 8) + _buffer[pos+byteSize-1-x];
    }
	humidity = i / 1e6;
}

void ZombieDatagram::getBitmap(bool &a, bool &b, bool &c, bool &d, bool &e, bool &f, bool &g, bool &h, uint16_t pos) {
	uint8_t byteSize = 1;
	uint32_t i = 0;
	for(uint8_t x = 0; x < byteSize; x++) {
		i = (i << 8) + _buffer[pos+byteSize-1-x];
    }
	
	a = (i >> 7) & 1;
	b = (i >> 6) & 1;
	c = (i >> 5) & 1;
	d = (i >> 4) & 1;
	e = (i >> 3) & 1;
	f = (i >> 2) & 1;
	g = (i >> 1) & 1;
	h = (i >> 0) & 1;
}

int ZombieDatagram::getLength() {
    return _currentSize;
}

byte* ZombieDatagram::getBytes() {
    return _buffer;
}

void ZombieDatagram::setBytes(byte* buffer)
{
	_buffer = buffer;
	_currentSize = buffer[0];
}
