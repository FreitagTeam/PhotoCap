#include <SystemStatus.h>
#include <PID_v2.h>
#include <bonsainet.h>
#include "mnist_100_test_images.h"

// #define debug

SystemStatus systemStatus;
uint8_t sleepLoops = 0;
int16_t vcc;
uint32_t messageCount = 0;

/* Device configuration */
#define DEVICE_ID 13
#define SEND_BURST 2

// PID Settings
double vcc_f, sleepLoops_f, targetVcc_f;
PID myPID(&vcc_f, &sleepLoops_f, &targetVcc_f, CONSKP, CONSKI, CONSKD, DIRECT);
// const uint8_t packageType = 206;

byte image[196];
uint8_t imageNumber = 0;
void receiveImage();

void setup() {
#ifdef debug
  Serial.begin(1000000);
  Serial.println("Pid Mnist Sleep...");
#endif

  setDeviceId(DEVICE_ID);
  // set target to (mnist)RELAY, (data)HOP, (final)FOG, depending on scenario
  setCommunicationTarget(RELAY);
  
  pinMode(LEDPIN, OUTPUT);
  digitalWrite(LEDPIN, LOW);
  pinMode(TX_POWER_PIN, OUTPUT);
  digitalWrite(TX_POWER_PIN, LOW);
  vcc = systemStatus.getVCC();
  vcc_f = vcc;
  targetVcc_f = SENDER_TARGET_VCC;        // We set the target VCC to the switch on vcc
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
  receiveImage(); // Receive first image in setup
}

void loop() {
  // digitalWrite(TX_POWER_PIN, HIGH);
  // initRadio();
  // initWriting();
  vcc = systemStatus.getVCC();
  vcc_f = vcc ;
  
  myPID.Compute();
  sleepLoops = (uint8_t) sleepLoops_f;
  // sleepLoops = 25;

  if (radio.failureDetected)
  {
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
    radio.powerUp();
    uint8_t imageRow = messageCount%14;

    // Guarantee message delivery
    for (int i=0; i<SEND_BURST; i++)
    {
      if (sendMnistImageRow(image, imageRow, vcc, messageCount, sleepLoops))
      {
#ifdef debug
  Serial.println(imageRow);
#endif
        messageCount++;
        if (imageRow == 13)
        {
          imageNumber++;
          receiveImage();   // Receive new image if this one has been transmitted successfully

        }
        break;
      }
#ifdef debug
      else
      {
        Serial.print(".");
      }
#endif




    }

    // Fire and forget
    // receiveImage();
    // sendMnistImage(image, vcc, messageCount, sleepLoops);
    // messageCount++;

    // digitalWrite(TX_POWER_PIN, LOW);
    radio.powerDown();
    delay(1);
    if (sleepLoops > 0)
    {
      simpleSleep.deeplyFor(sleepLoops*SINGLESLEEP, SleepCal);      
    }  
  }
}

void receiveImage()
{
#ifdef debug
  Serial.print("Image: ");
  Serial.println(imageNumber);
#endif
  for (int i=0; i<14; i++)
  {
    for (int j=0; j<14; j++)
    {
      int index = i*14 + j;
      image[index] = pgm_read_byte(&mnist_images[imageNumber%num_mnist_images][i][j]);
    }
  }
}