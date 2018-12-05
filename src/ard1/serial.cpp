#include "Arduino.h"
#include "ard1.h"
#include "serial.h"

void serial(void) {
  if (!Serial) return;
  Serial.begin(BAUD_RATE);
  while (!Serial)
    ;
}

int input_callback_count = 0;

callback input_callbacks = (callback) malloc( sizeof( callback ) );

void input(callback callback_function) {
  input_callback_count ++;
  callback input_callbacks_tmp = (callback) realloc(&input_callbacks, input_callback_count * sizeof( callback ));
  if (!input_callbacks_tmp) {
    exit(1);
  }
  input_callbacks = input_callbacks_tmp;
}

void input_update(void) {
  if (Serial.available() > 0) {
    String line = Serial.readStringUntil(';');
    for (int x = 0; x < input_callback_count; x++) {
      input_callbacks[x](line);
    }
  }
}
