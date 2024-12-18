/******************************************************************************
 * File:     Com_PidSleepRelay
 * Author:   Richard Freitag
 * Email:    richard.freitag@medoled.com
 * Created:  2022-08-11
 *
 * Revisions: ---
 *
 * Description: Layer 2 Mnist Compute Relay: Receive data from L3. Once a full dataset has been received, 
 *              the Mnist result is calculated and forwarded to the hop node on Layer 1 of the network
 *              
 ******************************************************************************/

#include <SystemStatus.h>
#include <PID_v2.h>
#include "bonsainet.h"
#include "network.h"

// #define debug

/* Device configuration */
#define DEVICE_ID 7

#define RELAY_SLEEP_FACTOR 12
#define HOP_SLEEP_FACTOR 3

// Device type; only use one
// #define RELAY_FROM_SOURCE_TO_FOG
#define RELAY_FROM_SOURCE_TO_HOP
// #define HOP_FROM_RELAY_TO_FOG

#define SEND_BURST 1
#define HBCOUNT 100   // Send VC heartbeat message every x loops

char data[BUFFERSIZE]; // = { 208, 0 };         // received data
char lastData[BUFFERSIZE]; // = { 208, 0 };     // last received dataset as buffer
// char data2[BUFFERSIZE]; // = { 225, 0 };     // TYPE_RELAYED_MNIST_RESULT - Sent to fog node

// int16_t sleepLoopsOffset = 160; // sleep loop synchronization offset in steps of 10ms 
// int16_t receivedSleepLoopsOffset = -100; // sleep offset 

#define DEFAULT_SLEEP_OFFSET 160
#define DEFAULT_RECEIVED_OFFSET -100
#define LONG_SLEEP_TOLERANCE 0       // Tolerance for long deep sleep

int16_t sleepLoopsOffset = DEFAULT_SLEEP_OFFSET; // sleep loop synchronization offset in steps of 10ms 
int16_t receivedSleepLoopsOffset = DEFAULT_RECEIVED_OFFSET; // sleep offset 

uint32_t messageCount = 0;
uint16_t loopCount = 0;
int16_t vcc;
uint8_t sleepLoops = 0;
uint8_t remoteSleepLoops = 0;

// sleep charge variables
uint8_t sleepFactor; // sleep multiplier for sleep charge
uint32_t slp = 1;       // calculated (variable) sleep time

// mnist variables
byte image[img_size];           // array to store input image
long layer1[l1_size];           // array to store layer 1 results
long layer2[l2_size];           // array to store layer 2 results
bool imageReceived = false;
byte max_idx = 255;               // prediction result; 255 indicates no result

#define l1_shift (l1w_bits + img_bits - l1b_bits)       // bit-shift distance of layer 1
#define l2_shift (l2w_bits + l1b_bits - l2b_bits)       // bit-shift distance of layer 2

/* Other objects and variables */
SystemStatus systemStatus;
bool packageReceived = false;
bool resultRelayed = false;
uint8_t sourceId = 255;

// PID Settings
double vcc_f, sleepLoops_f, targetVcc_f;
PID myPID(&vcc_f, &sleepLoops_f, &targetVcc_f, CONSKP, CONSKI, CONSKD, DIRECT);
// const uint8_t comChannel = DEVICE_ID%6;

void compute_network();
void get_result();

void setup() {
#ifdef debug
  Serial.begin(1000000);
  Serial.println("Com_PidSleepChargeMnistComputeRelay...");
#endif 

  setDeviceId(DEVICE_ID);
  // Use one function here to define device type

#ifdef  RELAY_FROM_SOURCE_TO_FOG
  setAsRelayFromSourceToFog();
  sleepFactor = RELAY_SLEEP_FACTOR;
#endif
#ifdef RELAY_FROM_SOURCE_TO_HOP
  setAsRelayFromSourceToHop();
  sleepFactor = RELAY_SLEEP_FACTOR;
#endif
#ifdef HOP_FROM_RELAY_TO_FOG
 setAsHopFromRelayToFog();
  sleepFactor = HOP_SLEEP_FACTOR;
#endif

  memset(lastData, 0, sizeof(data));
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
// #ifdef debug
//   Serial.println(".1");
// #endif
  imageReceived = false;

  // Sleep charge every HBCOUNT message
  if (messageCount%HBCOUNT==0)
  {
// #ifdef debug
//     Serial.println(".hb");
// #endif

    // Can be used for a hard reset of the nRF chip
    // digitalWrite(TX_POWER_PIN, LOW);
    // simpleSleep.deeplyFor(sleepLoops*SINGLESLEEP, SleepCal); 
    // digitalWrite(TX_POWER_PIN, HIGH);
    // delay(2);
    // initRadio(RF24_PA_MIN);

    slp = (uint32_t)sleepFactor*MAX_SLEEP*SINGLESLEEP;

// #ifdef debug
//     Serial.print(".slp.");
//     Serial.println(slp);
// #endif

    if (vcc < (RELAY_TARGET_VCC+LONG_SLEEP_TOLERANCE))
    {
      simpleSleep.deeplyFor(slp, SleepCal);
    }
    radio.powerUp();

    // 2023-01-28: Do not send VC message to hop
    // initWriting();
    // sendVCMessage(vcc, messageCount, sleepLoops);
  }

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
// #ifdef debug
//   Serial.println(".f");
// #endif
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
      packageReceived = false;
      uint16_t rcvCnt = 0;
      for (rcvCnt=0; rcvCnt<RECEIVERETRIES; rcvCnt++)
      {
         if (radio.available())
         {          
            radio.read(&data, sizeof(data));
            remoteSleepLoops = data[7];
            uint8_t row = data[9];
          // We discard wrong/bogus packages where first byte is 0 or -1
          if ( (uint8_t)data[7] == 0 || (uint8_t)data[7] == 255 || (uint8_t)data[8] == 255 )
          {
            radio.failureDetected = true;
            break;
          }
          packageReceived = true;
#ifdef debug
  Serial.print("rsl ");
  Serial.println(remoteSleepLoops);

  Serial.print("rcvCnt ");
  Serial.println(rcvCnt);

  Serial.print("os ");
  Serial.println(receivedSleepLoopsOffset);
#endif
          memcpy(&image[row*14], &data[11], 14);
          if (row == 13)
          {
            imageReceived = true;
            sourceId = data[8];     // When we receive an image, we forward the source ID
          }

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
          delay(1);
         }

        //  if (imageReceived)
        //  {
        //   Serial.println("Full image received");
        //  }





//         if (radio.available())
//         {
//           radio.read(&data, sizeof(data));
//           remoteSleepLoops = data[7];

//           // We discard wrong/bogus packages where first byte is 0 or -1
//           if ( (uint8_t)data[7] == 0 || (uint8_t)data[7] == 255 || (uint8_t)data[8] == 255 )
//           {
//             packageReceived = false;
//             radio.failureDetected = true;
//             break;
//           }

//           uint8_t row = data[9];
// #ifdef debug
//           Serial.println(row);
// #endif
//           memcpy(&image[row*14], &data[11], 14);
//           packageReceived = true;

//           // Serial.print((uint8_t)data[0]);
//           // Serial.print(",");
//           // Serial.println((uint8_t)data[8]);

//           if (sleepLoops > remoteSleepLoops)
//           {
//             //Serial.println(rcvCnt);

//             // Adaptive sleep offset
//             if (rcvCnt > 70){
//               sleepLoopsOffset = sleepLoopsOffset + 20;
//             } else if (rcvCnt < 30){
//               sleepLoopsOffset = sleepLoopsOffset - 10;      
//             }
//             break;
//           }
//             // Serial.println(rcvCnt);
//             // Adaptive sleep offset
//             if (rcvCnt > 70 && remoteSleepLoops > 20){
//               receivedSleepLoopsOffset = receivedSleepLoopsOffset + 20;
//             } else if (rcvCnt < 30 && remoteSleepLoops > 20){
//               receivedSleepLoopsOffset = receivedSleepLoopsOffset - 10;      
//             }
//             if (remoteSleepLoops == 1) // Reset potential drift if received messages come in quickly
//               receivedSleepLoopsOffset = DEFAULT_RECEIVED_OFFSET;
//             break;
//         }
//         else
//         {
//           // unsigned long time0 = millis();
//           packageReceived = false;
//           // radio.powerDown();
//           // // simpleSleep.deeplyFor(1*SINGLESLEEP, SleepCal);      
//           // simpleSleep.deeplyFor(SINGLESLEEP/2, SleepCal);      
//           // radio.powerUp();
//           // initReading();
//           delay(1);
//         }
       }
    }


  if (imageReceived)
  {
    compute_network();
    get_result();
    // Reset image array to 0
    memset(image,0,sizeof(image));
    resultRelayed = false;
  }

  // We always send the last received data package; this can also be adapted to a ringbuffer
  if (resultRelayed == false && max_idx != 255)
  {
    initWriting();
    for (int i=0; i<SEND_BURST; i++)
    {
#ifdef debug
      Serial.println("Trying to relay...");
      Serial.println(max_idx);
#endif
      if (sendMnistResult(max_idx, vcc, messageCount, sleepLoops, sourceId))
      {
        resultRelayed = true;
        imageReceived = false;
#ifdef debug
  Serial.println("Relayed...");
#endif 
        break;
      }
    }
  }

  radio.powerDown();
  // digitalWrite(TX_POWER_PIN, LOW);
// #ifdef debug
//   Serial.println(messageCount);
//   Serial.print("rs.");
//   Serial.println(receivedSleepLoopsOffset);
// #endif

  messageCount++;
  relaySleep1(packageReceived, sleepLoops, remoteSleepLoops, sleepLoopsOffset, receivedSleepLoopsOffset);
  }
}


/**
 * Function to compute the 2 layer neural network.
 * Weights and Biases are loaded from PROGMEM
 */
void compute_network() {
    // COMPUTE LAYER 1
    for (byte i=0; i<l1_size; ++i) {
        layer1[i] = 0;

        // multiply matrix row i and vector
        for (byte j=0; j<img_size; ++j) {
            // layer1[i] += (long)image[j] * (long)(char)pgm_read_byte(&l1_weights[(int)j + (int)i * img_size]);
            layer1[i] += (long)image[j] * (long)(char)pgm_read_byte(&l1_weights[i][j]);
        }

        // shift bits to have same precision as layer 1 bias
        layer1[i] = layer1[i] >> l1_shift;

        // add layer 1 bias
        layer1[i] += (long)(char)pgm_read_byte(&l1_bias[i]);

        // relu activation
        if (layer1[i] < 0) {
            layer1[i] = 0;
        }
    }

    // COMPUTE LAYER 2
    for (byte i=0; i<l2_size; ++i) {
        layer2[i] = 0;

        // multiply matrix row i and vector
        for (byte j=0; j<l1_size; ++j) {
            //Serial.print("L2 before: ");
            //Serial.print(layer2[i]);
            //Serial.println();
            // layer2[i] += layer1[j] * (long)(char)pgm_read_byte(&l2_weights[(int)j + (int)i * l1_size]);

            //Serial.print("Layer 1, j: ");
            //Serial.print(layer1[j]);
            //Serial.println();

            //Serial.print("l2_weights[i][j]: ");
            //Serial.print((long)(char)pgm_read_byte(&l2_weights[i][j]));
            //Serial.println();
            
            
            layer2[i] += layer1[j] * (long)(char)pgm_read_byte(&l2_weights[i][j]);
            //Serial.print("L2 after: ");
            //Serial.print(layer2[i]);
            //Serial.println();
        }

        // shift bits to have same precision as layer 2 bias
        layer2[i] = layer2[i] >> l2_shift;

        // add layer 2 bias
        layer2[i] += (long)(char)pgm_read_byte(&l2_bias[i]);
    }
}

/**
 * Function to send the predicted value back to the serial port
 */
void get_result() {
    max_idx = 0;
    long max_val = 0;

    for (byte i = 0; i < l2_size; ++i) {
        //Serial.print(layer2[i]);
        //Serial.print(",");
        if (layer2[i] > max_val) {
            max_val = layer2[i];
            max_idx = i;
        }
        //Serial.println();
    }
}