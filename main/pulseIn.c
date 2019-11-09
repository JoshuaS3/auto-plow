// pulseIn.c
// Reads pulse width on input GPIO pin
// Copyright (c) Joshua 'joshuas3' Stockin 2019

#include "driver/gpio.h"

#define WAIT_FOR_PIN_STATE(state) \
	while (gpio_get_level(pin) != (state)) { \
		if (xthal_get_ccount() - func_start > 5000000) return 0; \
	}

unsigned int pulseIn(unsigned char pin, unsigned char state) {
	const unsigned int func_start = xthal_get_ccount();
	WAIT_FOR_PIN_STATE(!state);
	WAIT_FOR_PIN_STATE(state); // wait until pin reaches state
	const unsigned int pulse_start = xthal_get_ccount();
	WAIT_FOR_PIN_STATE(!state); // pin leaves state, record clock time elapsed
	return xthal_get_ccount() - pulse_start;
}
