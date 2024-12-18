#if ARDUINO >= 100
    #include "Arduino.h"
#endif
#include "ZombieEncoder.h"

ZombieEncoder::ZombieEncoder(byte *buffer) {
  _buffer = buffer;
}

void ZombieEncoder::_intToBytes(byte *buf, int32_t i, uint8_t byteSize) {
    for(uint8_t x = 0; x < byteSize; x++) {
        buf[x] = (byte) (i >> (x*8));
    }
}

void ZombieEncoder::writeUnixtime(uint32_t unixtime) {
    _intToBytes(_buffer, unixtime, 4);
    _buffer += 4;
}

void ZombieEncoder::writeUint32(uint32_t j) {
    _intToBytes(_buffer, j, 4);
    _buffer += 4;
}

void ZombieEncoder::writeLatLng(float latitude, float longitude) {
    int32_t lat = latitude * 1e6;
    int32_t lng = longitude * 1e6;	
    _intToBytes(_buffer, lat, 4);
    _intToBytes(_buffer + 4, lng, 4);
    _buffer += 8;
}

void ZombieEncoder::writeUint16(uint16_t i) {
    _intToBytes(_buffer, i, 2);
    _buffer += 2;
}

void ZombieEncoder::writeUint8(uint8_t i) {
    _intToBytes(_buffer, i, 1);
    _buffer += 1;
}

void ZombieEncoder::writeHumidity(float humidity) {
    int32_t hum = humidity * 1e6;
    _intToBytes(_buffer, hum, 4);
    _buffer += 4;
}

void ZombieEncoder::writeTemperature(float temperature) {
    int32_t temp = temperature * 1e6;
    _intToBytes(_buffer, temp, 4);
    _buffer += 4;
}

void ZombieEncoder::writeBitmap(bool a, bool b, bool c, bool d, bool e, bool f, bool g, bool h) {
    uint8_t bitmap = 0;
    // LSB first
    bitmap |= (a & 1) << 7;
    bitmap |= (b & 1) << 6;
    bitmap |= (c & 1) << 5;
    bitmap |= (d & 1) << 4;
    bitmap |= (e & 1) << 3;
    bitmap |= (f & 1) << 2;
    bitmap |= (g & 1) << 1;
    bitmap |= (h & 1) << 0;
    writeUint8(bitmap);
}
