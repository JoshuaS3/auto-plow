#include "Arduino.h"

#ifndef __ARD1_H__
#define __ARD1_H__ 1

// Pins

// 0 and 1 are hardware RX and TX
#define PIN_RESET 2

#define ROBOCLAW_LOGIC 3

#define ROBOCLAW_RX 4
#define ROBOCLAW_TX 5

#define PIN_MOTOR_BRAKES 6
#define PIN_LIGHTS 7

#define PIN_CH_1 8
#define PIN_CH_2 9
#define PIN_CH_3 10
#define PIN_CH_4 11
#define PIN_CH_5 12
#define PIN_CH_6 13

#define PIN_PLOW_LIFT_UP A0
#define PIN_PLOW_LIFT_DOWN A1
#define PIN_PLOW_TILT_LEFT A2
#define PIN_PLOW_TILT_RIGHT A3

// Other settings

#define CONTROLLER_SENSITIVITY 4 // 1-5

#define RADIO_TIMEOUT 1000

#define BAUD_RATE 9600
#define ROBOCLAW_BAUD 38400

#endif
