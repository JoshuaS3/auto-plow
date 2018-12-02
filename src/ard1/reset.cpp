#include "Arduino.h"
#include "ard1.h"
#include "reset.h"

void reset(void) {
  pinMode(PIN_RESET, OUTPUT);
}
