#include <Arduino.h>
#include <ZombieMessage.h>
#include <RF24.h>
#include <ZombieMessage.h>
#include <WiFi.h>
#include <WiFiUdp.h>

#define LED 2
#define BUFFER_SIZE 32 // bytes for the Tvc array
RF24 radio(D7, D8); // CE, CSN
// const byte address[6] = "solar";

uint64_t address[6] = { 0x7878787878LL,
                        0xB3B4B5B6F1LL,
                        0xB3B4B5B6CDLL,
                        0xB3B4B5B6A3LL,
                        0xB3B4B5B60FLL,
                        0xB3B4B5B605LL };

// To receive data directly 
// uint64_t address[6] = { 0x6878787878LL,
//                         0xA3B4B5B6F1LL,
//                         0xA3B4B5B6CDLL,
//                         0xA3B4B5B6A3LL,
//                         0xA3B4B5B60FLL,
//                         0xA3B4B5B605LL };

WiFiUDP UDP;
// #define TARGET_IP "192.168.1.109" // Yoga
#define TARGET_IP "192.168.1.108" // PN50
#define UDP_PORT 17583
const char* ssid = "ObiLanKenobi2g"; // Replace with your own SSID
const char* password = "Tripod-2020"; //Replace with your own password
// const char* ssid = "Freitaglab2"; // Replace with your own SSID
// const char* password = "Tripod-2022"; //Replace with your own password

uint8_t buffer[BUFFER_SIZE];
bool conn;
ZombieDatagram datagram;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(1000000);
  Serial.println(F("nRF24l01+ Fog Relay"));
  pinMode(LED, OUTPUT);

  // if (!radio.begin()) {
  //   Serial.println(F("radio hardware is not responding!!"));
  //   while (1) {}  // hold in infinite loop
  // }

  while(!radio.begin()) {
    Serial.println(F("radio hardware is not responding!!"));
    delay(100);
  }

  // radio.openReadingPipe(0, address);
  // Set the addresses for all pipes to TX nodes
  for (uint8_t i = 0; i < 6; ++i)
    radio.openReadingPipe(i, address[i]);
  radio.setPALevel(RF24_PA_MAX);
  radio.startListening();

  conn = radio.isChipConnected();
  if (conn)
    Serial.println("Transceiver detected");
  else
    Serial.println("No transceiver connected");

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  Serial.println();
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());

  UDP.beginPacket(TARGET_IP, UDP_PORT);
  UDP.print("Hello from Fog Relay at ");
  UDP.print(WiFi.localIP().toString().c_str());
  UDP.endPacket();
  delay(250);
}

void loop() {
  // Serial.println(F("Main loop"));
  digitalWrite(LED, HIGH);
	if (radio.available()) {
    // Serial.println(F("Received..."));
    radio.read(&buffer, sizeof(buffer));
		MessageType type;
		type = (MessageType)buffer[0];
    
    UDP.beginPacket(TARGET_IP, UDP_PORT);

    if (type != 0)
    {
      Serial.print(0);      // Print 0 for compatibility with analysis script
      Serial.print(",");
      Serial.print(type);
      Serial.print(",");

      UDP.print(0);      // Print 0 for compatibility with analysis script
      UDP.print(",");
      UDP.print(type);
      UDP.print(",");

			if (type == MessageType::TYPE_TVC)
			{
        TvcMessage message;
				datagram.setBytes(buffer);
				decodeTvcMessage(datagram, message);
				//Serial.print("Temperature: ");
				Serial.print(message.Temperature);
				Serial.print(",");
				Serial.print(message.Vcc);
				Serial.print(",");
				Serial.print(message.MessageCount);
				Serial.print(",");
				Serial.println(message.DeviceId);

				UDP.print(message.Temperature);
				UDP.print(",");
				UDP.print(message.Vcc);
				UDP.print(",");
				UDP.print(message.MessageCount);
				UDP.print(",");
				UDP.print(message.DeviceId);
			}
      if (type == MessageType::TYPE_LVC)
      {
        LvcMessage message;
        datagram.setBytes(buffer);
        decodeLvcMessage(datagram, message);
        //Serial.print("Lux: ");
        Serial.print(message.Lux);
        Serial.print(",");
        Serial.print(message.Vcc);
        Serial.print(",");
        Serial.print(message.MessageCount);
        Serial.print(",");
        Serial.println(message.DeviceId);

        UDP.print(message.Lux);
        UDP.print(",");
        UDP.print(message.Vcc);
        UDP.print(",");
        UDP.print(message.MessageCount);
        UDP.print(",");
        UDP.print(message.DeviceId);
      }
      else if (type == MessageType::TYPE_DVC 
            || type == MessageType::TYPE_HEARTBEAT)
      {
        datagram.setBytes(buffer);
        DvcMessage message;
        decodeDvcMessage(datagram, message);
        Serial.print(message.DhrystoneMips,6);
        Serial.print(",");
        Serial.print(message.Vcc);
        Serial.print(",");
        Serial.print(message.MessageCount);
        Serial.print(",");
        Serial.println(message.DeviceId);

        UDP.print(message.DhrystoneMips,6);
        UDP.print(",");
        UDP.print(message.Vcc);
        UDP.print(",");
        UDP.print(message.MessageCount);
        UDP.print(",");
        UDP.print(message.DeviceId);
      }
      else if (type == MessageType::TYPE_MNISTRESULT)
      {
        MnistResultMessage message;
        datagram.setBytes(buffer);
        decodeMnistResultMessage(datagram, message);
        Serial.print(message.Vcc);
        Serial.print(",");
        Serial.print(message.SleepLoops);
        Serial.print(",");
        Serial.print(message.DeviceId);        
        Serial.print(",");
        Serial.print(message.MessageCount);
        Serial.print(",");
        Serial.println(message.PredictedDigit);

        UDP.print(message.Vcc);
        UDP.print(",");
        UDP.print(message.SleepLoops);
        UDP.print(",");
        UDP.print(message.DeviceId);        
        UDP.print(",");
        UDP.print(message.MessageCount);
        UDP.print(",");
        UDP.print(message.PredictedDigit);
      }
      else if (type == MessageType::TYPE_RELAYED_MNISTRESULT)
      {
        MnistRelayedResultMessage message;
        datagram.setBytes(buffer);
        decodeMnistRelayedResultMessage(datagram, message);
        Serial.print(message.Vcc);
        Serial.print(",");
        Serial.print(message.SleepLoops);
        Serial.print(",");
        Serial.print(message.DeviceId);        
        Serial.print(",");
        Serial.print(message.MessageCount);
        Serial.print(",");
        Serial.print(message.PredictedDigit);
        Serial.print(",");
        Serial.print(message.RelayVcc);
        Serial.print(",");
        Serial.print(message.RelaySleepLoops);
        Serial.print(",");
        Serial.println(message.RelayDeviceId);        

        UDP.print(message.Vcc);
        UDP.print(",");
        UDP.print(message.SleepLoops);
        UDP.print(",");
        UDP.print(message.DeviceId);        
        UDP.print(",");
        UDP.print(message.MessageCount);
        UDP.print(",");
        UDP.print(message.PredictedDigit);
        UDP.print(",");
        UDP.print(message.RelayVcc);
        UDP.print(",");
        UDP.print(message.RelaySleepLoops);
        UDP.print(",");
        UDP.print(message.RelayDeviceId);        
      }
      else if (type == MessageType::TYPE_VC || 
               type == MessageType::TYPE_STARTUP ||
               type == MessageType::TYPE_RELAY_RESYNC)
      {
        datagram.setBytes(buffer);
        StandardMessage message;
        decodeStandardMessage(datagram, message);
        Serial.print(message.Vcc);
        Serial.print(",");
        Serial.print(message.SleepLoops);
        Serial.print(",");
        Serial.print(message.DeviceId);
        Serial.print(",");
        Serial.println(message.MessageCount);

        UDP.print(message.Vcc);
        UDP.print(",");
        UDP.print(message.SleepLoops);
        UDP.print(",");
        UDP.print(message.DeviceId);
        UDP.print(",");
        UDP.print(message.MessageCount);
      }
      else if (type == MessageType::TYPE_RELAYED_VC)
      {
        VcRelayedMessage message;
        datagram.setBytes(buffer);
        decodeVcRelayedMessage(datagram, message);
        Serial.print(message.Vcc);
        Serial.print(",");
        Serial.print(message.SleepLoops);
        Serial.print(",");
        Serial.print(message.DeviceId);        
        Serial.print(",");
        Serial.print(message.MessageCount);
        Serial.print(",");
        Serial.print(message.RelayVcc);
        Serial.print(",");
        Serial.print(message.RelaySleepLoops);
        Serial.print(",");
        Serial.println(message.RelayDeviceId);        

        UDP.print(message.Vcc);
        UDP.print(",");
        UDP.print(message.SleepLoops);
        UDP.print(",");
        UDP.print(message.DeviceId);        
        UDP.print(",");
        UDP.print(message.MessageCount);
        UDP.print(",");
        UDP.print(message.RelayVcc);
        UDP.print(",");
        UDP.print(message.RelaySleepLoops);
        UDP.print(",");
        UDP.print(message.RelayDeviceId);        
      }
      else if (type == MessageType::TYPE_ATMEGA328P_ID)
      {
        Serial.print("ATMEGA328P_ID,");
        UDP.print("ATMEGA328P_ID,");
        datagram.setBytes(buffer);
        Atmega328pIdMessage message;
        decodeAtmega328pIdMessage(datagram, message);
        for (int i=0; i<sizeof(message.Id); i++)
        {
          if (message.Id[i] < 0x10)
          {
            Serial.print("0");
            UDP.print("0");
          }
            
          Serial.print(message.Id[i], HEX);
          Serial.print(" ");
          UDP.print(message.Id[i], HEX);
          UDP.print(" ");
        }
        Serial.println();
      }
      else if (type == MessageType::TYPE_MNISTIMAGEROW)
      {
        // Serial.print(buffer[8]); // Sleep loops 
        // Serial.print(",");

          datagram.setBytes(buffer);
          MnistImageRowMessage message;
          decodeMnistImageRowMessage(datagram, message);
          Serial.print(message.Vcc);
          Serial.print(",");
          Serial.printf("%3d",message.SleepLoops);
          Serial.print(",");
          Serial.printf("%2d", message.DeviceId);
          Serial.print(",");
          Serial.print(message.MessageCount);
          Serial.print(",");
          Serial.printf("%2d",message.ImageRow);
          Serial.print(" - ");

          UDP.print(message.Vcc);
          UDP.print(",");
          UDP.printf("%3d",message.SleepLoops);
          UDP.print(",");
          UDP.printf("%2d", message.DeviceId);
          UDP.print(",");
          UDP.print(message.MessageCount);
          UDP.print(",");
          UDP.printf("%2d",message.ImageRow);
          UDP.print(" - ");

          for (int i=0; i<14; i++)
          {
            Serial.printf("%3d",buffer[11+i]);
            Serial.print(" ");

            UDP.printf("%3d",buffer[11+i]);
            UDP.print(" ");
          }
          Serial.println();
      }
      else
      {
          Serial.println(" - unkown package type");
          UDP.print(" - unkown package type");
          // delay(50);
          // char raw[BUFFER_SIZE];
          // memcpy(raw, buffer, sizeof(buffer));
          // Serial.println(raw);
      }
      UDP.endPacket();
		}
  }
  digitalWrite(LED, LOW);
}
