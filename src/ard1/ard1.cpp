#include "Arduino.h"
#include "ard1.h"

// One time
void setup() {
  pinMode(PIN, OUTPUT);
  digitalWrite(PIN, ON);
  Serial.begin(BAUD_RATE);
  while (!Serial)
    ;
  Serial.println("ARD1: Motor Controller Active");
}

// Repeatedly
void loop() {
  if (Serial.available() > 0) {
    Serial.print("ARD1: Received information - ");
    String message = Serial.readStringUntil(';');
    Serial.println(message);
    Serial.flush();
    if (message == String("Restart")) {
      Serial.println("ARD1: Restarting...");
      Serial.flush();
      reset();
    }
  }
}
