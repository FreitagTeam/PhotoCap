/******************************************************************************
 * File:     Com_PidSleepRelay
 * Author:   Richard Freitag
 * Email:    richard.freitag@medoled.com
 * Created:  2022-08-11
 *
 * Revisions: ---
 *
 * Description: Layer 1 nRF relay. Wake up periodically and try to receive the Mnist result from Layer 2. Once a dataset
 *              is received, it will be forwarded to the fog node at Layer 0
 *              
 ******************************************************************************/

#include <SystemStatus.h>
#include <PID_v2.h>
#include "hmnet.h"

#define SEND_BURST 1
#define HBCOUNT 100   // Send VC heartbeat message every x loops

char data[BUFFERSIZE]; // = { 208, 0 };         // received data
// char data2[BUFFERSIZE]; // = { 225, 0 };        // TYPE_RELAYED_MNIST_RESULT - Sent to fog node

// int16_t sleepLoopsOffset = 160; // sleep loop synchronization offset in steps of 10ms 
// int16_t receivedSleepLoopsOffset = -100; // sleep offset 

#define DEFAULT_SLEEP_OFFSET 160
#define DEFAULT_RECEIVED_OFFSET -100

int16_t sleepLoopsOffset = DEFAULT_SLEEP_OFFSET; // sleep loop synchronization offset in steps of 10ms 
int16_t receivedSleepLoopsOffset = DEFAULT_RECEIVED_OFFSET; // sleep offset 

uint32_t messageCount = 0;
uint16_t loopCount = 0;
int16_t vcc;
uint8_t sleepLoops = 0;
uint8_t remoteSleepLoops = 0;

/* Other objects and variables */
SystemStatus systemStatus;
bool packageReceived = false;

// PID Settings
double vcc_f, sleepLoops_f, targetVcc_f;
PID myPID(&vcc_f, &sleepLoops_f, &targetVcc_f, CONSKP, CONSKI, CONSKD, DIRECT);
// const uint8_t comChannel = DEVICE_ID%6;

// #define debug

void setup() {
#ifdef debug
  Serial.begin(1000000);
  Serial.println("Com_PidSleepRelay...");
#endif 

  pinMode(LEDPIN, OUTPUT);
  digitalWrite(LEDPIN, LOW);
  pinMode(TX_POWER_PIN, OUTPUT);
  digitalWrite(TX_POWER_PIN, LOW);
  vcc = systemStatus.getVCC();
  vcc_f = vcc;
  targetVcc_f = RELAY_TARGET_VCC;        // We set the target VCC to the switch on vcc
  myPID.SetMode(AUTOMATIC);
  myPID.SetTunings(CONSKP, CONSKI, CONSKD);
  myPID.SetOutputLimits(MIN_SLEEP, MAX_SLEEP);
  SleepCal = simpleSleep.getCalibration();
  myPID.Compute();
  sleepLoops = (uint8_t) sleepLoops_f;
  
  digitalWrite(TX_POWER_PIN, HIGH);
  initRadio(RF24_PA_MIN);
  initWriting();
  sendStartupMessage(vcc, messageCount, sleepLoops);
  sendStartupMessage(vcc, messageCount, sleepLoops);
  radio.powerDown();
}

void loop() {
#ifdef debug
  Serial.println(".1");
#endif
  radio.powerUp();

  // Can be used for "visual" debugging
  // digitalWrite(LEDPIN, HIGH);
  // delay(10);
  // digitalWrite(LEDPIN, LOW);

  // digitalWrite(TX_POWER_PIN, HIGH);
  // initRadio();
  remoteSleepLoops = 0;
  vcc = systemStatus.getVCC();
  vcc_f = vcc ;

// For VCC averaging
//  vcc = 0;
//  for (int i = 0; i < VCC_CYCLES; ++i) {
//    vcc += systemStatus.getVCC();
//  }
//  vcc = vcc / VCC_CYCLES;
//  vcc_f = vcc ;

  myPID.Compute();
  sleepLoops = (uint8_t) sleepLoops_f;
  // sleepLoops = MAX_SLEEP;

  // Send relay resync message (or not)  
  // if (packageReceived == false)
  // {
  //   initWriting();
  //   sendRelayResyncMessage(vcc, messageCount, sleepLoops);
  //   // set offsets back to defaults to prevent drift
  //   receivedSleepLoopsOffset = DEFAULT_RECEIVED_OFFSET;
  //   sleepLoopsOffset = DEFAULT_SLEEP_OFFSET;
  // }

  if (radio.failureDetected)
  {
#ifdef debug
  Serial.println(".f");
#endif
    digitalWrite(TX_POWER_PIN, LOW);
    simpleSleep.deeplyFor(sleepLoops*SINGLESLEEP, SleepCal); 
    digitalWrite(TX_POWER_PIN, HIGH);
    delay(2);

    messageCount = 0;
    initRadio(RF24_PA_MIN);
    initWriting();
    radio.powerDown();
    radio.failureDetected = false;    
  }
  else
  {
    if (vcc > RELAY_TARGET_VCC) // We skip receiving data if VCC is too low
    {
      initReading();
      uint16_t rcvCnt = 0;
      for (rcvCnt=0; rcvCnt<RECEIVERETRIES; rcvCnt++)
      {
        if (radio.available())
        {
          radio.read(&data, sizeof(data));
          remoteSleepLoops = data[7];

          // We discard wrong/bogus packages where first byte is 0 or -1
          if ( (uint8_t)data[7] == 0 || (uint8_t)data[7] == 255 || (uint8_t)data[8] == 255 )
          {
            packageReceived = false;
            radio.failureDetected = true;
            break;
          }

          packageReceived = true;

          // Serial.print((uint8_t)data[0]);
          // Serial.print(",");
          // Serial.println((uint8_t)data[8]);

          if (sleepLoops > remoteSleepLoops)
          {
            //Serial.println(rcvCnt);

            // Adaptive sleep offset
            if (rcvCnt > 70){
              sleepLoopsOffset = sleepLoopsOffset + 20;
            } else if (rcvCnt < 30){
              sleepLoopsOffset = sleepLoopsOffset - 10;      
            }
            break;
          }
            // Serial.println(rcvCnt);
            // Adaptive sleep offset
            if (rcvCnt > 70 && remoteSleepLoops > 20){
              receivedSleepLoopsOffset = receivedSleepLoopsOffset + 20;
            } else if (rcvCnt < 30 && remoteSleepLoops > 20){
              receivedSleepLoopsOffset = receivedSleepLoopsOffset - 10;      
            }
            if (remoteSleepLoops == 1) // Reset potential drift if received messages come in quickly
              receivedSleepLoopsOffset = DEFAULT_RECEIVED_OFFSET;
            break;
        }
        else
        {
          // unsigned long time0 = millis();
          packageReceived = false;
          // radio.powerDown();
          // // simpleSleep.deeplyFor(1*SINGLESLEEP, SleepCal);      
          // simpleSleep.deeplyFor(SINGLESLEEP/2, SleepCal);      
          // radio.powerUp();
          // initReading();
          delay(1);
        }
      }
    }

  // Reset Heartbeat every
  if (messageCount%HBCOUNT==0)
  {
#ifdef debug
    Serial.println(".hb");
#endif

    // Can be used for a hard reset of the nRF chip
    // digitalWrite(TX_POWER_PIN, LOW);
    // simpleSleep.deeplyFor(sleepLoops*SINGLESLEEP, SleepCal); 
    // digitalWrite(TX_POWER_PIN, HIGH);
    // delay(2);
    // initRadio(RF24_PA_MIN);

    initWriting();
    sendVCMessage(vcc, messageCount, sleepLoops);
  }

  if (packageReceived)
  {
#ifdef debug
    Serial.println(".pr");
#endif
    packageReceived = false; // Prevent a resend of the package during the next loop
    initWriting();
    for (int i=0; i<SEND_BURST; i++)
    {
      relayVCMessage(data, vcc, sleepLoops);
    }
  }
  radio.powerDown();
  // digitalWrite(TX_POWER_PIN, LOW);
#ifdef debug
  Serial.println(messageCount);
  Serial.print("rs.");
  Serial.println(receivedSleepLoopsOffset);
#endif

  messageCount++;
  relaySleep1(packageReceived, sleepLoops, remoteSleepLoops, sleepLoopsOffset, receivedSleepLoopsOffset);
  }
}
