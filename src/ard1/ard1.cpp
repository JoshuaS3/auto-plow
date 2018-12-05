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
  Serial.print("ARD1: Setting PIN ");
  Serial.print(PIN);
  Serial.println("'s mode to OUTPUT");
  pinMode(PIN, OUTPUT);
  Serial.print("ARD1: Turning PIN ");
  Serial.print(PIN);
  Serial.println(" ON");
  digitalWrite(PIN, ON);
  input(oninput);
  Serial.println(sizeof(&input));
  Serial.println(sizeof(callback));
}

void oninput (String message) {
  Serial.print("ARD1: Received information - ");
  Serial.println(message);
  Serial.flush();
  if (message == String("Restart")) {
    Serial.println("ARD1: Restarting...");
    Serial.flush();
    reset();
  }
}

// Repeatedly
void loop() {
  input_update();
}
