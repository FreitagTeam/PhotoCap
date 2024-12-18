#include "ZombieMessage.h"
#include "ZombieEncoder.h"
#include "ZombieDatagram.h"

TemperatureMessage decodeTemperatureMessage(ZombieDatagram datagram)
{
	TemperatureMessage message;
	datagram.getTemperature(message.Temperature, 2);
	return message;
}


ZombieDatagram encodeTemperatureDatagram(TemperatureMessage message)
{
	ZombieDatagram datagram;
	datagram.addUint8(message.Type);
	datagram.addTemperature(message.Temperature);
	return datagram;
}

TvcMessage decodeTvcMessage(ZombieDatagram datagram)
{
	TvcMessage message;
	datagram.getTemperature(message.Temperature, 2);
	datagram.getUint16(message.Vcc, 6);
	datagram.getUint32(message.MessageCount, 8);
	datagram.getUint8(message.DeviceId, 12);
	return message;
}

void decodeTvcMessage(ZombieDatagram &datagram, TvcMessage &message)
{
	datagram.getTemperature(message.Temperature, 2);
	datagram.getUint16(message.Vcc, 6);
	datagram.getUint32(message.MessageCount, 8);
	datagram.getUint8(message.DeviceId, 12);
}

ZombieDatagram encodeTvcDatagram(TvcMessage message)
{
	ZombieDatagram datagram;
	return datagram;
	// not implemented
}

LvcMessage decodeLvcMessage(ZombieDatagram datagram)
{
	LvcMessage message;
	datagram.getUint16(message.Lux, 2);
	datagram.getUint16(message.Vcc, 4);
	datagram.getUint32(message.MessageCount, 6);
	datagram.getUint8(message.DeviceId, 10);
	return message;
}

void decodeLvcMessage(ZombieDatagram &datagram, LvcMessage &message)
{
	datagram.getUint16(message.Lux, 2);
	datagram.getUint16(message.Vcc, 4);
	datagram.getUint32(message.MessageCount, 6);
	datagram.getUint8(message.DeviceId, 10);
}

ZombieDatagram encodeLvcDatagram(LvcMessage message)
{
	ZombieDatagram datagram;
	return datagram;
	// not implemented
}

DvcMessage decodeDvcMessage(ZombieDatagram datagram)
{
	DvcMessage message;
	datagram.getDhrystoneMips(message.DhrystoneMips, 2);
	datagram.getUint16(message.Vcc, 6);
	datagram.getUint32(message.MessageCount, 8);
	datagram.getUint8(message.DeviceId, 12);
	return message;
}

void decodeDvcMessage(ZombieDatagram &datagram, DvcMessage &message)
{
	datagram.getDhrystoneMips(message.DhrystoneMips, 2);
	datagram.getUint16(message.Vcc, 6);
	datagram.getUint32(message.MessageCount, 8);
	datagram.getUint8(message.DeviceId, 12);
}

ZombieDatagram encodeDvcDatagram(DvcMessage message)
{
	ZombieDatagram datagram;
	return datagram;
	// not implemented
}

MnistResultMessage decodeMnistResultMessage(ZombieDatagram datagram)
{
	MnistResultMessage message;
	datagram.getUint8(message.PredictedDigit, 2);
	datagram.getUint16(message.Vcc, 3);
	datagram.getUint32(message.MessageCount, 5);
	datagram.getUint8(message.SleepLoops, 9);
	datagram.getUint8(message.DeviceId, 10);
	return message;
}

void decodeMnistResultMessage(ZombieDatagram &datagram, MnistResultMessage &message)
{
	datagram.getUint8(message.PredictedDigit, 2);
	datagram.getUint16(message.Vcc, 3);
	datagram.getUint32(message.MessageCount, 5);
	datagram.getUint8(message.SleepLoops, 9);
	datagram.getUint8(message.DeviceId, 10);
}

ZombieDatagram encodeDvcDatagram(MnistResultMessage message)
{
	ZombieDatagram datagram;
	return datagram;
	// not implemented
}

///

MnistRelayedResultMessage decodeMnistRelayedResultMessage(ZombieDatagram datagram)
{
	MnistRelayedResultMessage message;
	datagram.getUint8(message.PredictedDigit, 2);
	datagram.getUint16(message.Vcc, 3);
	datagram.getUint32(message.MessageCount, 5);
	datagram.getUint8(message.SleepLoops, 9);
	datagram.getUint8(message.DeviceId, 10);
	datagram.getUint16(message.RelayVcc, 11);
	datagram.getUint8(message.RelaySleepLoops, 13);
	datagram.getUint8(message.RelayDeviceId, 14);
	return message;
}

void decodeMnistRelayedResultMessage(ZombieDatagram &datagram, MnistRelayedResultMessage &message)
{
	datagram.getUint8(message.PredictedDigit, 2);
	datagram.getUint16(message.Vcc, 3);
	datagram.getUint32(message.MessageCount, 5);
	datagram.getUint8(message.SleepLoops, 9);
	datagram.getUint8(message.DeviceId, 10);
	datagram.getUint16(message.RelayVcc, 11);
	datagram.getUint8(message.RelaySleepLoops, 13);
	datagram.getUint8(message.RelayDeviceId, 14);
}

ZombieDatagram encodeDvcDatagram(MnistRelayedResultMessage message)
{
	ZombieDatagram datagram;
	return datagram;
	// not implemented
}

///


VcMessage decodeVcMessage(ZombieDatagram datagram)
{
	VcMessage message;
	datagram.getUint16(message.Vcc, 2);
	datagram.getUint32(message.MessageCount, 4);
	datagram.getUint8(message.SleepLoops, 8);
	datagram.getUint8(message.DeviceId, 9);
	return message;
}

void decodeVcMessage(ZombieDatagram &datagram, VcMessage &message)
{
	datagram.getUint16(message.Vcc, 2);
	datagram.getUint32(message.MessageCount, 4);
	datagram.getUint8(message.SleepLoops, 8);
	datagram.getUint8(message.DeviceId, 9);
}

ZombieDatagram encodeVcDatagram(VcMessage message)
{
	ZombieDatagram datagram;
	return datagram;
	// not implemented
}

Atmega328pIdMessage decodeAtmega328pIdMessage(ZombieDatagram datagram)
{
	Atmega328pIdMessage message;
	for (int i=2; i<=message.Length-2; i++)
	{
		datagram.getUint8(message.Id[i-2], i);
	}
	return message;
}

void decodeAtmega328pIdMessage(ZombieDatagram &datagram, Atmega328pIdMessage &message)
{
	for (int i=2; i<=message.Length-2; i++)
	{
		datagram.getUint8(message.Id[i-2], i);
	}
}

ZombieDatagram encodeAtmega328pIdDatagram(Atmega328pIdMessage message)
{
	ZombieDatagram datagram;
	return datagram;
	// not implemented
}