#include "Arduino.h"

// One time
void setup() {
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  Serial.begin(9600);
  while (!Serial)
    ;
  Serial.println("ARD1: Motor Controller Active");
  Serial.println("ARD1: Hello, World!");
}

void (* resetFunc) (void) = 0;


// Repeatedly
void loop() {
  if (Serial.available() > 0) {
    Serial.print("ARD1: Received information: ");
    String message = Serial.readStringUntil(';');
    Serial.println(message);
    Serial.flush();
    if (message == String("Restart")) {
      Serial.println("ARD1: Restarting");
      Serial.flush();
      resetFunc();
    }
  }
}
