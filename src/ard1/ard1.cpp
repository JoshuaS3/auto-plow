#include "Arduino.h"
#include "ard1.h"
#include "serial.h"
#include "reset.h"

#include "RoboClaw.h"


void oninput(String message);

// One time
void setup() {
  pinMode(PIN_RESET, INPUT);
  digitalWrite(PIN_RESET, LOW);
  serial();
  Serial.println("ARD1: Motor Controller Active");
  pinMode(PIN_BRAKES, OUTPUT);
  pinMode(PIN_MR_D, OUTPUT);
  pinMode(PIN_MR_R, OUTPUT);
  pinMode(PIN_ML_D, OUTPUT);
  pinMode(PIN_ML_R, OUTPUT);
  pinMode(7, INPUT);
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);
  pinMode(PIN_CH_1, INPUT);
  pinMode(PIN_CH_2, INPUT);
  pinMode(PIN_CH_3, INPUT);
}

int brakes = 1;
int ml = 0;
int mr = 0;

void oninput (String message) {
  Serial.print("ARD1: Received information - ");
  Serial.println(message);
  if (message == String("Restart")) {
    Serial.println("ARD1: Restarting...");
    delay(500);
    reset();
  } else if (message == String("b0")) {
    Serial.println("ARD1: Turning brakes off");
    brakes = 0;
    digitalWrite(PIN_BRAKES, HIGH);
  } else if (message == String("b1")) {
    Serial.println("ARD1: Turning brakes on");
    brakes = 1;
    digitalWrite(PIN_MR_D, LOW);
    digitalWrite(PIN_MR_R, LOW);
    digitalWrite(PIN_ML_D, LOW);
    digitalWrite(PIN_ML_R, LOW);
    if (mr != 0 || ml != 0) delay(CHANGE_DELAY);
    mr = 0;
    ml = 0;
    digitalWrite(PIN_BRAKES, LOW);
  }
  if (brakes == 1) return;
  if (message == String("mr0")) {
    Serial.println("ARD1: Turning MotorRight off");
    digitalWrite(PIN_MR_D, LOW);
    digitalWrite(PIN_MR_R, LOW);
    mr = 0;
  } else if (message == String("mr1")) {
    Serial.println("ARD1: Turning MotorRight on (drive)");
    digitalWrite(PIN_MR_R, LOW);
    if (mr == -1) delay(CHANGE_DELAY);
    mr = 1;
    digitalWrite(PIN_MR_D, HIGH);
  } else if (message == String("mr-1")) {
    Serial.println("ARD1: Turning MotorRight on (reverse)");
    digitalWrite(PIN_MR_D, LOW);
    if (mr == 1) delay(CHANGE_DELAY);
    mr = -1;
    digitalWrite(PIN_MR_R, HIGH);
  } else if (message == String("ml0")) {
    Serial.println("ARD1: Turning MotorLeft off");
    ml = 0;
    digitalWrite(PIN_ML_D, LOW);
    digitalWrite(PIN_ML_R, LOW);
  } else if (message == String("ml1")) {
    Serial.println("ARD1: Turning MotorLeft on (drive)");
    digitalWrite(PIN_ML_R, LOW);
    if (ml == -1) delay(CHANGE_DELAY);
    ml = 1;
    digitalWrite(PIN_ML_D, HIGH);
  } else if (message == String("ml-1")) {
    Serial.println("ARD1: Turning MotorLeft on (reverse)");
    digitalWrite(PIN_ML_D, LOW);
    if (ml == 1) delay(CHANGE_DELAY);
    ml = -1;
    digitalWrite(PIN_ML_R, HIGH);
  } else if (message == String("m0")) {
    Serial.println("ARD1: Turning both motors off");
    digitalWrite(PIN_MR_D, LOW);
    digitalWrite(PIN_MR_R, LOW);
    digitalWrite(PIN_ML_D, LOW);
    digitalWrite(PIN_ML_R, LOW);
    mr = 0;
    ml = 0;
  } else if (message == String("m1")) {
    Serial.println("ARD1: Turning both motors on (Drive)");
    digitalWrite(PIN_MR_R, LOW);
    digitalWrite(PIN_ML_R, LOW);
    if (mr == -1 || ml == -1) delay(CHANGE_DELAY);
    mr = 1;
    ml = 1;
    digitalWrite(PIN_MR_D, HIGH);
    digitalWrite(PIN_ML_D, HIGH);
  } else if (message == String("m-1")) {
    Serial.println("ARD1: Turning both motors on (Reverse)");
    digitalWrite(PIN_MR_D, LOW);
    digitalWrite(PIN_ML_D, LOW);
    if (mr == 1 || ml == 1) delay(CHANGE_DELAY);
    mr = -1;
    ml = -1;
    digitalWrite(PIN_MR_R, HIGH);
    digitalWrite(PIN_ML_R, HIGH);
  }
}

// Repeatedly
void loop() {
  int sensorValue2 = pulseIn(7, HIGH);
  if (sensorValue2 > 100) Serial.println(sensorValue2);
}

void laoop() {
  if (Serial.available() > 0) oninput(Serial.readStringUntil(';'));
  int ch1 = pulseIn(PIN_CH_1, HIGH);
  int ch2 = pulseIn(PIN_CH_2, HIGH);
  int ch3 = pulseIn(PIN_CH_3, HIGH);

  int ch1_fixed = map(ch1, 1106, 1992, -500, 500);
  int ch2_fixed = map(ch2, 997,  1992, -500, 500);
  int ch3_fixed = map(ch3, 993,  1876, -500, 500);

  Serial.print(ch1_fixed);
  Serial.print("\t");
  Serial.print(ch2_fixed);
  Serial.print("\t");
  Serial.print(ch3_fixed);
  Serial.println();

  if (ch3_fixed < -300) {
    oninput("b0");
  } else oninput("b1");

  if (ch1_fixed > 300) {
    oninput("mr-1");
    oninput("ml1");
    return;
  } else if (ch1_fixed < -300) {
    oninput("ml-1");
    oninput("mr1");
    return;
  }
  if (ch2_fixed > 300) {
    oninput("m1");
  } else if (ch2_fixed < -300) {
    oninput("m-1");
  } else oninput("m0");
}
