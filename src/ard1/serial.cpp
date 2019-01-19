#include "Arduino.h"
#include "ard1.h"
#include "serial.h"

void serial(void) {
  if (!Serial) return;
  Serial.begin(BAUD_RATE);
  while (!Serial)
    ;
}

