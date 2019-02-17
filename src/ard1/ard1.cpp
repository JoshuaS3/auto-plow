// Copyright (c) Joshua 'joshuas3' Stockin 2019

#define DEBUG 1

#include "Arduino.h"
#include "ard1.h"
#include "serial.h"
#include "reset.h"

#include "SoftwareSerial.h"
SoftwareSerial roboclaw(ROBOCLAW_RX, ROBOCLAW_TX);
SoftwareSerial roboclaw2(ROBOCLAW2_RX, ROBOCLAW2_TX);

// One time
void setup() {
  pinMode(PIN_RESET, INPUT);
  digitalWrite(PIN_RESET, LOW);
  serial();

  Serial.println("ARD1: ARD1 serial communications active");

  Serial.println("ARD1: Powering main RoboClaw logic side");
  pinMode(ROBOCLAW_LOGIC, OUTPUT);
  digitalWrite(ROBOCLAW_LOGIC, HIGH);

  Serial.println("ARD1: Powering secondary RoboClaw logic side");
  pinMode(ROBOCLAW2_LOGIC, OUTPUT);
  digitalWrite(ROBOCLAW2_LOGIC, HIGH);

  Serial.print("ARD1: Beginning RoboClaw serials at baud rate ");
  Serial.println(ROBOCLAW_BAUD);
  roboclaw.begin(ROBOCLAW_BAUD);
  roboclaw2.begin(ROBOCLAW_BAUD);

  Serial.println("ARD1: Configuring switch pins");
  pinMode(PIN_MOTOR_BRAKES, OUTPUT);
  pinMode(PIN_LIGHTS, OUTPUT);

  Serial.println("ARD1: Configuring radio receiver pins");
  pinMode(PIN_CH_1, INPUT);
  pinMode(PIN_CH_2, INPUT);
  pinMode(PIN_CH_3, INPUT);
  pinMode(PIN_CH_4, INPUT);
  pinMode(PIN_CH_5, INPUT);
  pinMode(PIN_CH_6, INPUT);
}

// Calculate sensitivity threshold
int left_joystick_threshold = (5 - LEFT_JOYSTICK_SENSITIVITY) * 10;
int right_joystick_threshold = (5 - RIGHT_JOYSTICK_SENSITIVITY) * 10;

int left_joystick_max = 64 + left_joystick_threshold;
int left_joystick_min = 64 - left_joystick_threshold;

int right_joystick_max = 64 + right_joystick_threshold;
int right_joystick_min = 64 - right_joystick_threshold;

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
  ch1 = constrain(ch1, 1099, 1978);
  ch2 = constrain(ch2, 1003, 1986);
  ch3 = constrain(ch3, 993, 1885);
  ch4 = constrain(ch4, 1023, 1986);
  ch5 = constrain(ch5, 993, 1986);
  ch6 = constrain(ch6, 993, 1986);

  // Map receiver information
  int ch1_fixed = map(ch1, 1099, 1978, 1, 127);
  int ch2_fixed = map(ch2, 1003, 1986, 1, 127);
  int ch3_fixed = map(ch3, 993, 1885, 1, 127);
  int ch4_fixed = map(ch4, 1023, 1986, 1, 127);
  int ch5_fixed = (ch5 > 1500); // 1 or 0
  int ch6_fixed = (ch6 > 1500); // 1 or 0

  // Calculate tilt direction
  if (ch3_fixed < left_joystick_max && ch3_fixed > left_joystick_min) ch3_fixed = 64;
  if (ch4_fixed < left_joystick_max && ch4_fixed > left_joystick_min) ch4_fixed = 64;

  int tilt = 64; // ch4_fixed
  int lift = 64; // ch3_fixed

  if (ch3_fixed > 64) {
    lift += (ch3_fixed - 64);
  } else if (ch3_fixed < 64) {
    lift -= (64 - ch3_fixed);
  }

  if (ch4_fixed > 64) {
    tilt += (ch4_fixed - 64);
  } else if (ch4_fixed < 64) {
    tilt -= (64 - ch4_fixed);
  }

  tilt = constrain(tilt, 1, 127);
  lift = constrain(lift, 1, 127);

  // Calculate motor directions
  if (ch1_fixed < right_joystick_max && ch1_fixed > right_joystick_min) ch1_fixed = 64;
  if (ch2_fixed < right_joystick_max && ch2_fixed > right_joystick_min) ch2_fixed = 64;

  int motor_left = 64;
  int motor_right = 64;

  if (ch1_fixed < 64) {
    motor_right += (64 - ch1_fixed);
    motor_left -= (64 - ch1_fixed);
  } else if (ch1_fixed > 64) {
    motor_left += (ch1_fixed - 64);
    motor_right -= (ch1_fixed - 64);
  }

  if (ch2_fixed > 64) {
    motor_right += (ch2_fixed - 64);
    motor_left += (ch2_fixed - 64);
  } else if (ch2_fixed < 64) {
    motor_right -= (64 - ch2_fixed);
    motor_left -= (64 - ch2_fixed);
  }

  // Constrain motor numbers
  motor_right = constrain(motor_right, 1, 127);
  motor_left = constrain(motor_left, 1, 127);

  // Offset numbers for RoboClaw serial format
  motor_right += 128;
  lift += 128;

  #ifdef DEBUG
  Serial.print("\e[1;1H\e[2J");
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

  // Write motor information to RoboClaws
  roboclaw.write((byte)motor_left);
  roboclaw.write((byte)motor_right);

  roboclaw2.write((byte)tilt);
  roboclaw2.write((byte)lift);

  // Lights
  if (ch6_fixed) {
    digitalWrite(PIN_LIGHTS, HIGH);
  } else {
    digitalWrite(PIN_LIGHTS, LOW);
  }

  // Power to motor controller
  if (ch5_fixed) {
    digitalWrite(PIN_MOTOR_BRAKES, HIGH);
    digitalWrite(ROBOCLAW_LOGIC, HIGH);
    digitalWrite(ROBOCLAW2_LOGIC, HIGH);
  } else {
    digitalWrite(PIN_MOTOR_BRAKES, LOW);
    digitalWrite(ROBOCLAW_LOGIC, LOW);
    digitalWrite(ROBOCLAW2_LOGIC, LOW);
  }

  int end = millis();
  #ifdef DEBUG
  Serial.print("Cycle time: ");
  Serial.println(end - start);
  #endif
}
