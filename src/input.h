// espy Copyright (c) 2023 Josh Stockin <josh@joshstock.in>
// [https://joshstock.in] [https://github.com/joshuas3]
//
// This software is licensed and distributed under the terms of the MIT License.
// See the MIT License in the LICENSE file of this project's root folder.
//
// This comment block and its contents, including this disclaimer, MUST be
// preserved in all copies or distributions of this software's source.


// Filename: input.h
// Description: Input config/ISR/task functions and utilities


#pragma once


#include "esp_attr.h"
#include "esp_err.h"

// input utilities
#define NUM_RADIO_CHANNELS     10
#define RADIO_PULSE_STOPGAP_US 3500 // minimum length of transmission block separation in microseconds

// input functions
esp_err_t setup_radio_and_rotenc_input( void ); // config, ISR creation, task creation
void radio_pulse( void* arg );                  // ISR
void rotenc_switch( void* arg );                // ISR
void rotenc_rotate( void* arg );                // ISR
void input_handler_task( void* arg );

// queue input event structure
typedef struct xInputEvent {
    uint8_t type;
    uint16_t data;
} xInputEvent;

// input event types
#define INPUT_EVENT_RADIOPULSE 0
#define INPUT_EVENT_ROTENCSW   1
#define INPUT_EVENT_ROTENCTURN 2

// move to separate headers
esp_err_t setup_epd( void );
esp_err_t setup_control_hardware( void );
esp_err_t setup_control_loop( void );
