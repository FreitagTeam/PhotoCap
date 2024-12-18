/*
Name:		ZombieMessage.h
Created:	7/28/2018 12:34:41 AM
Author:		Richard Freitag
Description: These are the actual messages that contain the individual values
*/

#pragma once
#include "ZombieDatagram.h"
#include "ZombieEncoder.h"

// TYPE_TEMPERATURE
struct TemperatureMessage
{
	TemperatureMessage()
	{
		Type = MessageType::TYPE_TEMPERATURE;
		Temperature = 0.0;
	}
	byte Length;
	MessageType Type;
	float Temperature;
};

TemperatureMessage decodeTemperatureMessage(ZombieDatagram datagram);
ZombieDatagram encodeTemperatureDatagram(TemperatureMessage message);
void decodeTemperatureMessage(ZombieDatagram &datagram, TemperatureMessage &message);

// TYPE_TVC
struct TvcMessage
{
	TvcMessage()
	{
		Type = MessageType::TYPE_TVC;
		Temperature = 0.0;
		Vcc = 0;
		MessageCount = 0;
		DeviceId = 0;
	}
	MessageType Type;
	float Temperature;
	uint16_t Vcc;
	uint32_t MessageCount;
	byte DeviceId;
};

TvcMessage decodeTvcMessage(ZombieDatagram datagram);
ZombieDatagram encodeTvcDatagram(TvcMessage message);
void decodeTvcMessage(ZombieDatagram &datagram, TvcMessage &message);

// TYPE_LVC
struct LvcMessage
{
	LvcMessage()
	{
		Type = MessageType::TYPE_LVC;
		Lux = 0.0;
		Vcc = 0;
		MessageCount = 0;
		DeviceId = 0;
	}
	MessageType Type;
	uint16_t Lux;
	uint16_t Vcc;
	uint32_t MessageCount;
	byte DeviceId;
};

LvcMessage decodeLvcMessage(ZombieDatagram datagram);
ZombieDatagram encodeLvcDatagram(LvcMessage message);
void decodeLvcMessage(ZombieDatagram &datagram, LvcMessage &message);

// TYPE_DVC - Dhrystone per second
struct DvcMessage
{
	DvcMessage()
	{
		Type = MessageType::TYPE_DVC;
		DhrystoneMips = 0.0;
		Vcc = 0;
		MessageCount = 0;
		DeviceId = 0;
	}
	MessageType Type;
	float DhrystoneMips;
	uint16_t Vcc;
	uint32_t MessageCount;
	byte DeviceId;
};

DvcMessage decodeDvcMessage(ZombieDatagram datagram);
ZombieDatagram encodeDvcDatagram(DvcMessage message);
void decodeDvcMessage(ZombieDatagram &datagram, DvcMessage &message);


// TYPE_MNISTRESULT
struct MnistResultMessage
{
	MnistResultMessage()
	{
		Type = MessageType::TYPE_MNISTRESULT;
		PredictedDigit = 255;		// Not 0, because it is a prediction value
		Vcc = 0;
		MessageCount = 0;
		SleepLoops = 0;
		DeviceId = 0;
	}
	MessageType Type;
	uint8_t PredictedDigit;
	uint16_t Vcc;
	uint32_t MessageCount;
	byte SleepLoops;
	byte DeviceId;
};

MnistResultMessage decodeMnistResultMessage(ZombieDatagram datagram);
ZombieDatagram encodeDvcDatagram(MnistResultMessage message);
void decodeMnistResultMessage(ZombieDatagram &datagram, MnistResultMessage &message);

// TYPE_RELAYED_MNISTRESULT
struct MnistRelayedResultMessage
{
	MnistRelayedResultMessage()
	{
		Type = MessageType::TYPE_RELAYED_MNISTRESULT;
		PredictedDigit = 255;		// Not 0, because it is a prediction value
		Vcc = 0;
		MessageCount = 0;
		SleepLoops = 0;
		DeviceId = 0;
		RelayVcc = 0;
		RelaySleepLoops = 0;
		RelayDeviceId = 0;
	}
	MessageType Type;
	uint8_t PredictedDigit;
	uint16_t Vcc;
	uint32_t MessageCount;
	byte SleepLoops;
	byte DeviceId;
	uint16_t RelayVcc;
	byte RelaySleepLoops;
	byte RelayDeviceId;
};

MnistRelayedResultMessage decodeMnistRelayedResultMessage(ZombieDatagram datagram);
ZombieDatagram encodeDvcDatagram(MnistRelayedResultMessage message);
void decodeMnistRelayedResultMessage(ZombieDatagram &datagram, MnistRelayedResultMessage &message);

// TYPE_VC
struct VcMessage
{
	VcMessage()
	{
		Type = MessageType::TYPE_VC;
		Vcc = 0;
		MessageCount = 0;
		SleepLoops = 0;
		DeviceId = 0;
	}
	MessageType Type;
	uint16_t Vcc;
	uint32_t MessageCount;
	byte SleepLoops;
	byte DeviceId;
};

VcMessage decodeVcMessage(ZombieDatagram datagram);
ZombieDatagram encodeVcDatagram(VcMessage message);
void decodeVcMessage(ZombieDatagram &datagram, VcMessage &message);

// TYPE_ATMEGA328P_ID
struct Atmega328pIdMessage
{
	Atmega328pIdMessage()
	{
		Length = 12;
		Type = MessageType::TYPE_ATMEGA328P_ID;
	}
	byte Length;
	MessageType Type;
	uint8_t Id[9];
};

Atmega328pIdMessage decodeAtmega328pIdMessage(ZombieDatagram datagram);
ZombieDatagram encodeAtmega328pIdDatagram(Atmega328pIdMessage message);
void decodeAtmega328pIdMessage(ZombieDatagram &datagram, Atmega328pIdMessage &message);