#ifndef CONFIG0_H
#define CONFIG0_H

/* Static defines - Those should not change, unless hardware is changed */
#define RFCE 7
#define RFCSN 8
#define LEDPIN 9
#define TX_POWER_PIN 10 

/* Device configuration */
#define DEVICE_ID 8

/* Pid and sleep configuration parameters */
#define MIN_SLEEP 1         // Minimum sleep cycles
#define MAX_SLEEP 250       // Maximum sleep cycles
#define SINGLESLEEP 20      // single sleep time in ms, multiples of 10
#define TARGET_VCC 3140  // target vcc for the PID loop; usual switch off is around 3120
#define CONSKP 2.0
#define CONSKI 0.05
#define CONSKD 0.05

/* Communication parameters */
#define BUFFERSIZE 32
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

#endif // CONFIG0_H
