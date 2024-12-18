#include "bonsainet.h"

RF24 radio(RFCE, RFCSN); // CE, CSN
SimpleSleep simpleSleep;
SimpleSleep_Cal SleepCal;
uint8_t _deviceId = 0;
uint8_t _comChannel = 0;
enum DeviceType _comSource;
enum DeviceType _comTarget;
enum DeviceType _deviceType;
bool _writeInit = false;
bool _readInit = false;

uint64_t relayAddress[6] = { 0x6878787878LL,
                        0xA3B4B5B6F1LL, // 241
                        0xA3B4B5B6CDLL, // 205
                        0xA3B4B5B6A3LL, // 163
                        0xA3B4B5B60FLL, // 15
                        0xA3B4B5B605LL }; // 5

uint64_t fogAddress[6] = { 0x7878787878LL,
                        0xB3B4B5B6F1LL, // 241
                        0xB3B4B5B6CDLL, // 205
                        0xB3B4B5B6A3LL, // 163
                        0xB3B4B5B60FLL, // 15
                        0xB3B4B5B605LL }; // 5

uint64_t hopAddress[6] = { 0x5878787878LL,
                        0x03B4B5B6F1LL, // 241
                        0x03B4B5B6CDLL, // 205
                        0x03B4B5B6A3LL, // 163
                        0x03B4B5B60FLL, // 15
                        0x03B4B5B605LL }; // 5

void setCommunicationSource(DeviceType deviceType)
{
  _comSource = deviceType;
}

void setCommunicationTarget(DeviceType deviceType)
{
  _comTarget = deviceType;
}

void setDeviceType(DeviceType type)
{
  _deviceType = type;
}

void setAsRelayFromSourceToFog()
{
    // RELAY_FROM_SOURCE_TO_FOG
  setCommunicationSource(SOURCE);
  setDeviceType(RELAY);
  setCommunicationTarget(FOG);
}

void setAsRelayFromSourceToHop()
{
  // RELAY_FROM_SOURCE_TO_HOP
  setCommunicationSource(SOURCE);
  setDeviceType(RELAY);
  setCommunicationTarget(HOP);
}

void setAsHopFromRelayToFog()
{
  // HOP_FROM_RELAY_TO_FOG
  setCommunicationSource(RELAY);
  setDeviceType(HOP);
  setCommunicationTarget(FOG);
}

void setDeviceId(uint8_t deviceId)
{
  _deviceId = deviceId;
  _comChannel = _deviceId%6;
}

void initRadio()
{
  initRadio(RF24_PA_MIN);
}

void initRadio(uint8_t level)
{
  while (!radio.begin()) {
    // simpleSleep.idle();
    digitalWrite(LEDPIN, HIGH);
    delay(30);
    digitalWrite(LEDPIN, LOW);
  }
  radio.setPALevel(level);
}


void initWriting()
{
  if (_writeInit)
  {
    return;
  }

  radio.stopListening();
  switch (_comTarget)
  {
    case SOURCE: // This should not happen; will write to fog in case it is called by accident
      radio.openWritingPipe(fogAddress[_comChannel]);
      break;
    case RELAY:
      radio.openWritingPipe(relayAddress[_comChannel]);
      break;
    case HOP:
      radio.openWritingPipe(hopAddress[_comChannel]);
      break;
    case FOG:
      radio.openWritingPipe(fogAddress[_comChannel]);
      break;
  }
  _writeInit = true;
  _readInit = false;

  // if (SEND_TO_RELAY)
  // {
  //   radio.openWritingPipe(relayAddress[comChannel]);
  //   // Serial.println("Send to relay!");
  // }
  // else  
  // {
  //   radio.openWritingPipe(fogAddress[comChannel]);
  // }
  radio.setRetries(((_deviceId * 3) % 12) + 3, SENDRETRIES);  // maximum value is 15 for both args
}

void initReading()
{
  if (_readInit)
  {
    // Reading pipe is already open, we just need to start listening again    
    radio.startListening();
    return;
  }

  switch (_deviceType)
  {
    case SOURCE:
      // Source devices should not need a reading pipe; in case this is called by accident, we listen to messages from the fog
      radio.openReadingPipe(0, fogAddress[_comChannel]);
      break;
    case RELAY:
      radio.openReadingPipe(0, relayAddress[_comChannel]);
      break;
    case HOP:
      radio.openReadingPipe(0, hopAddress[_comChannel]);
      break;
    case FOG:
      radio.openReadingPipe(0, fogAddress[_comChannel]);
      break;
  }
  radio.startListening();
  _writeInit = false;
  _readInit = true;
  // Serial.println("Init Reading done");
}

bool relayMessage(char relayData[BUFFERSIZE])
{
  char data[BUFFERSIZE];
  memcpy(data, relayData, sizeof(data));
  return radio.write(data, sizeof(data));
}

bool sendVCMessage(int16_t vcc, uint32_t messageCount, uint8_t sleepLoops)
{
  char data[BUFFERSIZE];
  memset(data, 0, sizeof(data));
  data[0] = MessageType::TYPE_VC;

  setVcc(data, vcc);
  setMessageCount(data, messageCount);

  data[7] = sleepLoops;
  data[8] = _deviceId;
  return radio.write(data, sizeof(data));
}

bool sendStartupMessage(int16_t vcc, uint32_t messageCount, uint8_t sleepLoops)
{
  char data[BUFFERSIZE];
  memset(data, 0, sizeof(data));
  data[0] = MessageType::TYPE_STARTUP;

  setVcc(data, vcc);
  setMessageCount(data, messageCount);

  data[7] = sleepLoops;
  data[8] = _deviceId;
  return radio.write(data, sizeof(data));
}

bool sendRelayResyncMessage(int16_t vcc, uint32_t messageCount, uint8_t sleepLoops)
{
  char data[BUFFERSIZE];
  memset(data, 0, sizeof(data));
  data[0] = MessageType::TYPE_RELAY_RESYNC;

  setVcc(data, vcc);
  setMessageCount(data, messageCount);

  data[7] = sleepLoops;
  data[8] = _deviceId;
  return radio.write(data, sizeof(data));
}

bool relayVCMessage(char data[BUFFERSIZE], int16_t vcc, uint8_t sleepLoops)
{
  char data2[BUFFERSIZE];
  memset(data2,0,sizeof(data2));
  data2[0] = MessageType::TYPE_RELAYED_VC;

  for (uint8_t x = 1; x < 9; x++)
  {
    data2[x] = data[x];
  }

  // Write relay vcc from sender at byte 10-11
  for (uint8_t x = 0; x < 2; x++) {
    data2[x + 9] = (uint8_t)(vcc >> (x * 8));
  }

  data2[11] = sleepLoops;
  data2[12] = _deviceId;

  return radio.write(&data2, sizeof(data2));; 
}

void sendMnistImage(byte image[196], int16_t vcc, uint32_t messageCount, uint8_t sleepLoops)
{
  char data[BUFFERSIZE];
  memset(data, 0, sizeof(data));
  data[0] = MessageType::TYPE_MNISTIMAGEROW;

  setVcc(data, vcc);
  setMessageCount(data, messageCount);

  // Write sleepLoops into byte 8 and deviceID into byte 9
  data[7] = sleepLoops;
  data[8] = _deviceId;

  for (uint8_t imageRow = 0; imageRow<14; imageRow++)
  {
    data[9] = imageRow;
    for (uint8_t imageByte = 0; imageByte<14; imageByte++)
    {
      data[10+imageByte] = image[imageRow*14+imageByte];
    }
    // Send image row package
    Serial.print(radio.write(&data, sizeof(data)));
  }
  Serial.println();
}

bool sendMnistImageRow(byte image[196], uint8_t imageRow, int16_t vcc, uint32_t messageCount, uint8_t sleepLoops)
{
  char data[BUFFERSIZE];
  memset(data, 0, sizeof(data));
  data[0] = MessageType::TYPE_MNISTIMAGEROW;

  setVcc(data, vcc);
  setMessageCount(data, messageCount);

  data[7] = sleepLoops;
  data[8] = _deviceId;

  data[9] = imageRow;
  for (uint8_t imageByte = 0; imageByte<14; imageByte++)
  {
    data[10+imageByte] = image[imageRow*14+imageByte];
  }

  return radio.write(data, sizeof(data));
}

bool sendMnistResult(byte result, int16_t vcc, uint32_t messageCount, uint8_t sleepLoops, uint8_t deviceId)
{
  char data[BUFFERSIZE];
  memset(data, 0, sizeof(data));
  data[0] = MessageType::TYPE_MNISTRESULT;

  setVcc(data, vcc);
  setMessageCount(data, messageCount);

  data[7] = sleepLoops;
  data[8] = deviceId;

  data[9] = result;

  bool rslt = radio.write(data, sizeof(data));
  Serial.print(rslt);

  return rslt;
}

// TBD: check data positions for result!
bool relayMnistResult(char data[BUFFERSIZE], int16_t vcc, uint8_t sleepLoops)
{
  char data2[BUFFERSIZE];
  memset(data2,0,sizeof(data2));
  data2[0] = MessageType::TYPE_RELAYED_MNISTRESULT;

  for (uint8_t x = 1; x < 9; x++)
  {
    data2[x] = data[x];
  }

  // Write relay vcc from sender at byte 10-11
  for (uint8_t x = 0; x < 2; x++) {
    data2[x + 9] = (uint8_t)(vcc >> (x * 8));
  }

  data2[11] = sleepLoops;
  data2[12] = _deviceId;

  return radio.write(&data2, sizeof(data2));; 
}

void setVcc(char data[BUFFERSIZE], int16_t vcc)
{
  // Write vcc into buffer at byte 3-4
  for (uint8_t x = 0; x < 2; x++) {
    data[x + 1] = (uint8_t)(vcc >> (x * 8));
  }
}

void setMessageCount(char data[BUFFERSIZE], uint32_t messageCount)
{
  // Write messageCount count into buffer at byte 5-8
  for (uint8_t x = 0; x < 4; x++) {
    data[x + 3] = (uint8_t)(messageCount >> (x * 8));
  }
}

/// @brief 
/// @param packageReceived 
/// @param sleepLoops 
/// @param remoteSleepLoops 
/// @param sleepLoopsOffset 
/// @param receivedSleepLoopsOffset 
void relaySleep1(bool packageReceived, uint8_t sleepLoops, uint8_t remoteSleepLoops, int16_t sleepLoopsOffset, int16_t receivedSleepLoopsOffset)
{
  // We synchronize our sleep with the sender
  if (packageReceived == true)
  {
    if (sleepLoops > remoteSleepLoops) // We synchronize our sleep if we have enough power
    {
      simpleSleep.deeplyFor(sleepLoops*SINGLESLEEP, SleepCal);      
    }
    else
    {
      if (remoteSleepLoops > 20)
      {
        // We make sure that the sleep time always is positive
        if (remoteSleepLoops*SINGLESLEEP + receivedSleepLoopsOffset <= 0)
        {
          simpleSleep.deeplyFor(remoteSleepLoops*SINGLESLEEP, SleepCal);
        }
        else
        {
          simpleSleep.deeplyFor(remoteSleepLoops*SINGLESLEEP + receivedSleepLoopsOffset, SleepCal);      
        }
      }
      else // no sleep offset for times that are shorter than the receive retries window
      {
        simpleSleep.deeplyFor(remoteSleepLoops*SINGLESLEEP, SleepCal);      
      }      
    }
  }
  else // we have not received anything and will just go ahead with internal sleep recommendation
  {
    if (sleepLoops > 0)
      simpleSleep.deeplyFor(sleepLoops*SINGLESLEEP, SleepCal);      
  }
}
