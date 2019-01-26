// Copyright (c) Joshua 'joshuas3' Stockin 2019

#define DEBUG 1

#include "Arduino.h"
#include "ard1.h"
#include "serial.h"
#include "reset.h"

#include "SoftwareSerial.h"
SoftwareSerial roboclaw(ROBOCLAW_RX, ROBOCLAW_TX);

// One time
void setup() {
  pinMode(PIN_RESET, INPUT);
  digitalWrite(PIN_RESET, LOW);
  serial();

  Serial.println("ARD1: ARD1 serial communications active");

  pinMode(ROBOCLAW_LOGIC, OUTPUT);
  digitalWrite(ROBOCLAW_LOGIC, HIGH);

  Serial.print("ARD1: Beginning RoboClaw Motor Controller serial at baud rate ");
  Serial.println(ROBOCLAW_BAUD);
  roboclaw.begin(ROBOCLAW_BAUD);

  pinMode(PIN_MOTOR_BRAKES, OUTPUT);
  pinMode(PIN_LIGHTS, OUTPUT);

  pinMode(PIN_PLOW_LIFT_UP, OUTPUT);
  pinMode(PIN_PLOW_LIFT_DOWN, OUTPUT);
  pinMode(PIN_PLOW_TILT_LEFT, OUTPUT);
  pinMode(PIN_PLOW_TILT_RIGHT, OUTPUT);

  pinMode(PIN_CH_1, INPUT);
  pinMode(PIN_CH_2, INPUT);
  pinMode(PIN_CH_3, INPUT);
  pinMode(PIN_CH_4, INPUT);
  pinMode(PIN_CH_5, INPUT);
  pinMode(PIN_CH_6, INPUT);
}

// Calculate sensitivity threshold
int controller_threshold = (5 - CONTROLLER_SENSITIVITY) * 10;
int joystick_max = 64 + controller_threshold;
int joystick_min = 64 - controller_threshold;

// Repeatedly
void loop() {
  int start = millis();

  // Read from radio receiver
  int ch1 = pulseIn(PIN_CH_1, RADIO_TIMEOUT);
  int ch2 = pulseIn(PIN_CH_2, RADIO_TIMEOUT);
  int ch3 = pulseIn(PIN_CH_3, RADIO_TIMEOUT);
  int ch4 = pulseIn(PIN_CH_4, RADIO_TIMEOUT);
  int ch5 = pulseIn(PIN_CH_5, RADIO_TIMEOUT);
  int ch6 = pulseIn(PIN_CH_6, RADIO_TIMEOUT);

  // ch1: right joystick horizontal, 1099, 1978
  // ch2: right joystick vertical,   1003, 1986
  // ch3: left joystick vertical,    993,  1885
  // ch4: left joystick horizontal,  1023, 1986
  // ch5: switch d,                  993,  1986
  // ch6: switch a,                  993,  1986

  // Constrain radio information
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

  // Map receiver information
  int ch1_fixed = map(ch1, 1099, 1978, 1, 127);
  int ch2_fixed = map(ch2, 1003, 1986, 1, 127);
  int ch3_fixed = map(ch3, 993, 1885, 1, 127);
  int ch4_fixed = map(ch4, 1023, 1986, 1, 127);
  int ch5_fixed = (ch5 > 1500); // 1 or 0
  int ch6_fixed = (ch6 > 1500); // 1 or 0

  // Calculate tilt direction
  int tilt = 0;
  if (ch4_fixed < joystick_min) tilt = -1;
  if (ch4_fixed > joystick_max) tilt = 1;

  // Calculate lift direction
  int lift = 0;
  if (ch3_fixed < joystick_min) lift = -1;
  if (ch3_fixed > joystick_max) lift = 1;

  // Calculate motor directions
  if (ch1_fixed < joystick_max && ch1_fixed > joystick_min) ch1_fixed = 64;
  if (ch2_fixed < joystick_max && ch2_fixed > joystick_min) ch2_fixed = 64;

  int motor_left = 64;
  int motor_right = 64;

  if (ch1_fixed < 64) {
    motor_right = 64 + (64 - ch1_fixed);
    motor_left = 64 - (64 - ch1_fixed);
  } else if (ch1_fixed > 64) {
    motor_left = 64 + (ch1_fixed - 64);
    motor_right = 64 - (ch1_fixed - 64);
  }

  if (ch2_fixed > 64) {
    motor_right += (ch2_fixed - 64);
    motor_left += (ch2_fixed - 64);
  } else if (ch2_fixed < 64) {
    motor_right -= (64 - ch2_fixed);
    motor_left -= (64 - ch2_fixed);
  }

  // Constrain motor directions
  if (motor_right > 127) motor_right = 127;
  if (motor_right < 1) motor_right = 1;
  if (motor_left > 127) motor_left = 127;
  if (motor_left < 1) motor_left = 1;

  // Offset motor_right number for RoboClaw serial format
  motor_right += 128;

  #ifdef DEBUG
  Serial.println("\e[1;1H\e[2J");
  Serial.println("Debug Information");
  Serial.print("Left");
  Serial.print("\t");
  Serial.print("Right");
  Serial.print("\t");
  Serial.print("Lights");
  Serial.print("\t");
  Serial.print("Brakes + Motors");
  Serial.print("\t\t");
  Serial.print("Plow Tilt");
  Serial.print("\t");
  Serial.print("Plow Lift");
  Serial.println();

  Serial.print(motor_left);
  Serial.print("\t");
  Serial.print(motor_right);
  Serial.print("\t");
  Serial.print(ch6_fixed);
  Serial.print("\t");
  Serial.print(ch5_fixed);
  Serial.print("\t\t\t");
  Serial.print(tilt);
  Serial.print("\t\t");
  Serial.println(lift);
  #endif

  // Write motor information to RoboClaw
  roboclaw.write((byte)motor_left);
  roboclaw.write((byte)motor_right);

  // Plow tilt
  if (tilt == -1) {
    digitalWrite(PIN_PLOW_TILT_RIGHT, LOW);
    digitalWrite(PIN_PLOW_TILT_LEFT, HIGH);
  } else if (tilt == 1) {
    digitalWrite(PIN_PLOW_TILT_LEFT, LOW);
    digitalWrite(PIN_PLOW_TILT_RIGHT, HIGH);
  } else {
    digitalWrite(PIN_PLOW_TILT_RIGHT, LOW);
    digitalWrite(PIN_PLOW_TILT_LEFT, LOW);
  }

  // Plow lift
  if (lift == -1) {
    digitalWrite(PIN_PLOW_LIFT_UP, LOW);
    digitalWrite(PIN_PLOW_LIFT_DOWN, HIGH);
  } else if (tilt == 1) {
    digitalWrite(PIN_PLOW_LIFT_DOWN, LOW);
    digitalWrite(PIN_PLOW_LIFT_UP, HIGH);
  } else {
    digitalWrite(PIN_PLOW_LIFT_UP, LOW);
    digitalWrite(PIN_PLOW_LIFT_DOWN, LOW);
  }

  // Lights
  if (ch6_fixed) {
    digitalWrite(PIN_LIGHTS, HIGH);
  } else {
    digitalWrite(PIN_LIGHTS, LOW);
  }

  // Power to motor controller
  if (ch5_fixed) {
    digitalWrite(PIN_MOTOR_BRAKES, HIGH);
  } else {
    digitalWrite(PIN_MOTOR_BRAKES, LOW);
  }

  int end = millis();
  #ifdef DEBUG
  Serial.print("Cycle time: ");
  Serial.println(end - start);
  #endif
}
