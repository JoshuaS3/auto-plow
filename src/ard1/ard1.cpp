#include "Arduino.h"
#include "ard1.h"
#include "serial.h"
#include "reset.h"


void oninput(String message);

// One time
void setup() {
  digitalWrite(PIN_RESET, HIGH);
  serial();
  Serial.println("ARD1: Motor Controller Active");
  pinMode(PIN_BRAKES, OUTPUT);
  pinMode(PIN_MR_D, OUTPUT);
  pinMode(PIN_MR_R, OUTPUT);
  pinMode(PIN_ML_D, OUTPUT);
  pinMode(PIN_ML_R, OUTPUT);
}

int brakes = 1;

void oninput (String message) {
  Serial.print("ARD1: Received information - ");
  Serial.println(message);
  if (message == String("Restart")) {
    Serial.println("ARD1: Restarting...");
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
    delay(CHANGE_DELAY);
    digitalWrite(PIN_BRAKES, LOW);
  }
  if (brakes == 1) return;
  if (message == String("mr0")) {
    Serial.println("ARD1: Turning MotorRight off");
    digitalWrite(PIN_MR_D, LOW);
    digitalWrite(PIN_MR_R, LOW);
  } else if (message == String("mr1")) {
    Serial.println("ARD1: Turning MotorRight on (drive)");
    digitalWrite(PIN_MR_R, LOW);
    delay(CHANGE_DELAY);
    digitalWrite(PIN_MR_D, HIGH);
  } else if (message == String("mr-1")) {
    Serial.println("ARD1: Turning MotorRight on (reverse)");
    digitalWrite(PIN_MR_D, LOW);
    delay(CHANGE_DELAY);
    digitalWrite(PIN_MR_R, HIGH);
  } else if (message == String("ml0")) {
    Serial.println("ARD1: Turning MotorLeft off");
    digitalWrite(PIN_ML_D, LOW);
    digitalWrite(PIN_ML_R, LOW);
  } else if (message == String("ml1")) {
    Serial.println("ARD1: Turning MotorLeft on (drive)");
    digitalWrite(PIN_ML_R, LOW);
    delay(CHANGE_DELAY);
    digitalWrite(PIN_ML_D, HIGH);
  } else if (message == String("ml-1")) {
    Serial.println("ARD1: Turning MotorLeft on (reverse)");
    digitalWrite(PIN_ML_D, LOW);
    delay(CHANGE_DELAY);
    digitalWrite(PIN_ML_R, HIGH);
  } else if (message == String("m0")) {
    Serial.println("ARD1: Turning both motors off");
    digitalWrite(PIN_MR_D, LOW);
    digitalWrite(PIN_MR_R, LOW);
    digitalWrite(PIN_ML_D, LOW);
    digitalWrite(PIN_ML_R, LOW);
  } else if (message == String("m1")) {
    Serial.println("ARD1: Turning both motors on (Drive)");
    digitalWrite(PIN_MR_R, LOW);
    digitalWrite(PIN_ML_R, LOW);
    delay(CHANGE_DELAY);
    digitalWrite(PIN_MR_D, HIGH);
    digitalWrite(PIN_ML_D, HIGH);
  } else if (message == String("m-1")) {
    Serial.println("ARD1: Turning both motors on (Reverse)");
    digitalWrite(PIN_MR_D, LOW);
    digitalWrite(PIN_ML_D, LOW);
    delay(CHANGE_DELAY);
    digitalWrite(PIN_MR_R, HIGH);
    digitalWrite(PIN_ML_R, HIGH);
  }
}

// Repeatedly
void loop() {
  if (Serial.available() > 0) oninput(Serial.readStringUntil(';'));
  delay(500);
}
