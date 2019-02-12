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

  Serial.println("ARD1: Sending power to RoboClaw logic side");
  pinMode(ROBOCLAW_LOGIC, OUTPUT);
  digitalWrite(ROBOCLAW_LOGIC, HIGH);

  Serial.print("ARD1: Beginning RoboClaw serial at baud rate ");
  Serial.println(ROBOCLAW_BAUD);
  roboclaw.begin(ROBOCLAW_BAUD);

<<<<<<< HEAD
  Serial.println("Enabling switch pins");
  pinMode(PIN_MOTOR_BRAKES, OUTPUT);
  pinMode(PIN_LIGHTS, OUTPUT);

  Serial.println("Enabling relay pins");
  pinMode(PIN_RELAY_1, OUTPUT);
  pinMode(PIN_RELAY_2, OUTPUT);
  pinMode(PIN_RELAY_3, OUTPUT);
  pinMode(PIN_RELAY_4, OUTPUT);
  pinMode(PIN_RELAY_5, OUTPUT);
  pinMode(PIN_RELAY_6, OUTPUT);

  Serial.println("Enabling radio receiver pins");
=======
  pinMode(PIN_MOTOR_BRAKES, OUTPUT);
  pinMode(PIN_LIGHTS, OUTPUT);

  pinMode(PIN_PLOW_LIFT_UP, OUTPUT);
  pinMode(PIN_PLOW_LIFT_DOWN, OUTPUT);
  pinMode(PIN_PLOW_TILT_LEFT, OUTPUT);
  pinMode(PIN_PLOW_TILT_RIGHT, OUTPUT);

>>>>>>> a85aa7c2aa323a809d7357d17b4ce3a517dd965f
  pinMode(PIN_CH_1, INPUT);
  pinMode(PIN_CH_2, INPUT);
  pinMode(PIN_CH_3, INPUT);
  pinMode(PIN_CH_4, INPUT);
  pinMode(PIN_CH_5, INPUT);
  pinMode(PIN_CH_6, INPUT);
}

<<<<<<< HEAD
// Calculate sensitivity threshold
int left_joystick_threshold = (5 - LEFT_JOYSTICK_SENSITIVITY) * 10;
int right_joystick_threshold = (5 - RIGHT_JOYSTICK_SENSITIVITY) * 10;

int left_joystick_max = 64 + left_joystick_threshold;
int left_joystick_min = 64 - left_joystick_threshold;

int right_joystick_max = 64 + right_joystick_threshold;
int right_joystick_min = 64 - right_joystick_threshold;

=======
int controller_threshold = (5 - CONTROLLER_SENSITIVITY) * 10;
int joystick_max = 64 + controller_threshold;
int joystick_min = 64 - controller_threshold;
int brakes = 1;
>>>>>>> a85aa7c2aa323a809d7357d17b4ce3a517dd965f

// Repeatedly
void loop() {
  int start = millis();

<<<<<<< HEAD
  // Read from radio receiver
  int ch1 = pulseIn(PIN_CH_1, RADIO_TIMEOUT);
  int ch2 = pulseIn(PIN_CH_2, RADIO_TIMEOUT);
  int ch3 = pulseIn(PIN_CH_3, RADIO_TIMEOUT);
  int ch4 = pulseIn(PIN_CH_4, RADIO_TIMEOUT);
  int ch5 = pulseIn(PIN_CH_5, RADIO_TIMEOUT);
  int ch6 = pulseIn(PIN_CH_6, RADIO_TIMEOUT);
=======
  int ch1 = pulseIn(PIN_CH_1, 1000);
  int ch2 = pulseIn(PIN_CH_2, 1000);
  int ch3 = pulseIn(PIN_CH_3, 1000);
  int ch4 = pulseIn(PIN_CH_4, 1000);
  int ch5 = pulseIn(PIN_CH_5, 1000);
  int ch6 = pulseIn(PIN_CH_6, 1000);
>>>>>>> a85aa7c2aa323a809d7357d17b4ce3a517dd965f

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
<<<<<<< HEAD
  int ch5_fixed = (ch5 > 1500); // 1 or 0
  int ch6_fixed = (ch6 > 1500); // 1 or 0

  // Calculate tilt direction
  int tilt = 0;
  if (ch4_fixed < left_joystick_min) tilt = -1;
  if (ch4_fixed > left_joystick_max) tilt = 1;

  // Calculate lift direction
  int lift = 0;
  if (ch3_fixed < left_joystick_min) lift = -1;
  if (ch3_fixed > left_joystick_max) lift = 1;

  // Calculate motor directions
  if (ch1_fixed < right_joystick_max && ch1_fixed > right_joystick_min) ch1_fixed = 64;
  if (ch2_fixed < right_joystick_max && ch2_fixed > right_joystick_min) ch2_fixed = 64;
=======
  int ch5_fixed = (ch5 > 1500);
  int ch6_fixed = (ch6 > 1500);

  int tilt = 0;
  if (ch4_fixed < joystick_min) tilt = -1;
  if (ch4_fixed > joystick_max) tilt = 1;

  int lift = 0;
  if (ch3_fixed < joystick_min) lift = -1;
  if (ch3_fixed > joystick_max) lift = 1;

  if (ch1_fixed < joystick_max && ch1_fixed > joystick_min) ch1_fixed = 64;
  if (ch2_fixed < joystick_max && ch2_fixed > joystick_min) ch2_fixed = 64;
>>>>>>> a85aa7c2aa323a809d7357d17b4ce3a517dd965f

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

<<<<<<< HEAD
  // Constrain motor directions
=======
>>>>>>> a85aa7c2aa323a809d7357d17b4ce3a517dd965f
  if (motor_right > 127) motor_right = 127;
  if (motor_right < 1) motor_right = 1;
  if (motor_left > 127) motor_left = 127;
  if (motor_left < 1) motor_left = 1;

<<<<<<< HEAD
  // Offset motor_right number for RoboClaw serial format
  motor_right += 128;

  #ifdef DEBUG
=======
  motor_right += 128;

  int end = millis();

>>>>>>> a85aa7c2aa323a809d7357d17b4ce3a517dd965f
  Serial.print("\e[1;1H\e[2J");
  Serial.print("Left");
  Serial.print("\t");
  Serial.print("Right");
<<<<<<< HEAD
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

  // Plow tilt (HIGH = LOW, LOW = HIGH)
  if (tilt == -1) {
    digitalWrite(PIN_RELAY_4, HIGH);
    digitalWrite(PIN_RELAY_5, LOW);
    digitalWrite(PIN_RELAY_6, LOW);
  } else if (tilt == 1) {
    digitalWrite(PIN_RELAY_5, HIGH);
    digitalWrite(PIN_RELAY_6, HIGH);
    digitalWrite(PIN_RELAY_4, LOW);
  } else {
    digitalWrite(PIN_RELAY_4, HIGH);
    digitalWrite(PIN_RELAY_5, HIGH);
    digitalWrite(PIN_RELAY_6, HIGH);
  }

  // Plow lift
  if (lift == -1) {
    digitalWrite(PIN_RELAY_1, HIGH);
    digitalWrite(PIN_RELAY_2, LOW);
    digitalWrite(PIN_RELAY_3, LOW);
  } else if (lift == 1) {
    digitalWrite(PIN_RELAY_2, HIGH);
    digitalWrite(PIN_RELAY_3, HIGH);
    digitalWrite(PIN_RELAY_1, LOW);
  } else {
    digitalWrite(PIN_RELAY_1, HIGH);
    digitalWrite(PIN_RELAY_2, HIGH);
    digitalWrite(PIN_RELAY_3, HIGH);
  }

  // Lights
=======
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

  Serial.print("Cycle time: ");
  Serial.println(end - start);

  roboclaw.write((byte)motor_left);
  roboclaw.write((byte)motor_right);

  if (tilt == -1) {
    digitalWrite(PIN_PLOW_TILT_RIGHT, LOW);
    digitalWrite(PIN_PLOW_TILT_LEFT, HIGH);
  } else if (tilt == 0) {
    digitalWrite(PIN_PLOW_TILT_RIGHT, LOW);
    digitalWrite(PIN_PLOW_TILT_LEFT, LOW);
  } else if (tilt == 1) {
    digitalWrite(PIN_PLOW_TILT_LEFT, LOW);
    digitalWrite(PIN_PLOW_TILT_RIGHT, HIGH);
  }
  if (lift == -1) {
    digitalWrite(PIN_PLOW_LIFT_UP, LOW);
    digitalWrite(PIN_PLOW_LIFT_DOWN, HIGH);
  } else if (lift == 0) {
    digitalWrite(PIN_PLOW_LIFT_UP, LOW);
    digitalWrite(PIN_PLOW_LIFT_DOWN, LOW);
  } else if (tilt == 1) {
    digitalWrite(PIN_PLOW_LIFT_DOWN, LOW);
    digitalWrite(PIN_PLOW_LIFT_UP, HIGH);
  }
>>>>>>> a85aa7c2aa323a809d7357d17b4ce3a517dd965f
  if (ch6_fixed) {
    digitalWrite(PIN_LIGHTS, HIGH);
  } else {
    digitalWrite(PIN_LIGHTS, LOW);
  }
<<<<<<< HEAD

  // Power to motor controller
=======
>>>>>>> a85aa7c2aa323a809d7357d17b4ce3a517dd965f
  if (ch5_fixed) {
    digitalWrite(PIN_MOTOR_BRAKES, HIGH);
  } else {
    digitalWrite(PIN_MOTOR_BRAKES, LOW);
  }
<<<<<<< HEAD

  int end = millis();
  #ifdef DEBUG
  Serial.print("Cycle time: ");
  Serial.println(end - start);
  #endif
=======
>>>>>>> a85aa7c2aa323a809d7357d17b4ce3a517dd965f
}
