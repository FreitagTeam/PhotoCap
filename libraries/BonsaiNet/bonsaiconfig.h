#ifndef BONSAICONFIG_H
#define BONSAICONFIG_H

/* Static defines - Those should not change, unless hardware is changed */
#define RFCE 7
#define RFCSN 8
#define LEDPIN 9
#define TX_POWER_PIN 10 

// #define SEND_TO_RELAY 1
#define SENDRETRIES 0       // radio.setretries; 0 to 15
#define RECEIVERETRIES 100   // number of retries for the radio receive loop; 1000 for larger single sleeps

/* Pid and sleep configuration parameters */
#define MIN_SLEEP 1             // Minimum sleep cycles
#define MAX_SLEEP 250           // Maximum sleep cycles
#define SINGLESLEEP 20          // single sleep time in ms, multiples of 10
#define SENDER_TARGET_VCC 3240  // target vcc for the PID loop; should be at least 60mV higher than switch off value
#define RELAY_TARGET_VCC 3350   // receiving packages requires higher VCC
#define CONSKP 2.0
#define CONSKI 0.05
#define CONSKD 0.05

/* Communication parameters */
#define BUFFERSIZE 32

#endif
