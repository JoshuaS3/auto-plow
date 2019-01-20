#include "Arduino.h"
#include "ard1.h"
#include "serial.h"
#include "reset.h"

#include "SoftwareSerial.h"
SoftwareSerial roboclaw(4, 5);

// One time
void setup() {
  pinMode(PIN_RESET, INPUT);
  digitalWrite(PIN_RESET, LOW);
  serial();

  Serial.println("ARD1: ARD1 serial communications active");
  Serial.print("ARD1: Beginning RoboClaw Motor Controller serial at baud rate ");
  Serial.println(ROBOCLAW_BAUD);
  roboclaw.begin(ROBOCLAW_BAUD);

  pinMode(PIN_BRAKES, OUTPUT);
  pinMode(PIN_CH_1, INPUT);
  pinMode(PIN_CH_2, INPUT);
  pinMode(PIN_CH_3, INPUT);
  pinMode(PIN_CH_4, INPUT);
  pinMode(PIN_CH_5, INPUT);
  pinMode(PIN_CH_6, INPUT);
}

int brakes = 1;

byte full_stop = 0;
// Repeatedly
void loop() {
  int ch1 = pulseIn(PIN_CH_1, 1000);
  int ch2 = pulseIn(PIN_CH_2, 1000);
  int ch3 = pulseIn(PIN_CH_3, 1000);
  int ch4 = pulseIn(PIN_CH_4, 1000);
  int ch5 = pulseIn(PIN_CH_5, 1000);
  int ch6 = pulseIn(PIN_CH_6, 1000);

  // ch1: right joystick horizontal, 1099, 1978
  // ch2: right joystick vertical,   1003, 1986
  // ch3: left joystick vertical,    993,  1885
  // ch4: left joystick horizontal,  1023, 1986
  // ch5: switch d,                  993,  1986
  // ch6: switch a,                  993,  1986

  if (ch1 < 1099) ch1 = 1099;
  if (ch1 > 1978) ch1 = 1978;

  if (ch2 < 1003) ch2 = 1003;
  if (ch2 > 1986) ch2 = 1986;

  if (ch3 < 993)  ch3 = 993;
  if (ch3 > 1885) ch3 = 1885;

  if (ch4 < 1023) ch4 = 1023;
  if (ch4 > 1986) ch4 = 1986;

  if (ch5 < 993)  ch5 = 993;
  if (ch5 > 1986) ch5 = 1986;

  if (ch6 < 993)  ch6 = 993;
  if (ch6 > 1986) ch6 = 1986;

  int ch1_fixed = map(ch1, 1099, 1978, 1, 127);
  int ch2_fixed = map(ch2, 1003, 1986, 1, 127);

  if (ch1_fixed < 84 && ch1_fixed > 44) ch1_fixed = 64;
  if (ch2_fixed < 84 && ch2_fixed > 44) ch2_fixed = 64;

  int motor_left = 64;
  int motor_right = 64;

  if (ch1_fixed > 64 && ch2_fixed > 64) {
    motor_left = ch2_fixed;
    motor_right = 127 - (ch1_fixed - 64);
  } else if (ch1_fixed < 64 && ch2_fixed > 64) {
    motor_right = ch2_fixed;
    motor_left = 127 - (64 - ch1_fixed);
  } else if (ch2_fixed > 64) {
    motor_right = ch2_fixed;
    motor_left = ch2_fixed;
  } else if (ch1_fixed > 64 && ch2_fixed < 64) {
    motor_left = ch2_fixed;
    motor_right = 64 - (64 - ch1_fixed);
  } else if (ch1_fixed < 64 && ch2_fixed < 64) {
    motor_right = ch2_fixed;
    motor_left = 64 - (ch1_fixed + 64);
  } else if (ch2_fixed < 64) {
    motor_left = ch2_fixed;
    motor_right = ch2_fixed;
  }

  Serial.print(motor_left - 64);
  Serial.print("\t");
  Serial.println(motor_right - 64);

//  roboclaw.write(127);
//  roboclaw.write(255);
}

