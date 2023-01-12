// espy Copyright (c) 2023 Josh Stockin <josh@joshstock.in>
// [https://joshstock.in] [https://github.com/joshuas3]
//
// This software is licensed and distributed under the terms of the MIT License.
// See the MIT License in the LICENSE file of this project's root folder.
//
// This comment block and its contents, including this disclaimer, MUST be
// preserved in all copies or distributions of this software's source.


// Filename: input.c
// Description: Handlers for radio receiver, rotary encoder


#include "esp_attr.h"
#include "esp_err.h"
#include "esp_check.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "driver/gptimer.h"

#include "input.h"
#include "pins.h"


static const char* TAG = "input.c";

static uint32_t current_radio_channel = 0;
static uint32_t radio_channels[NUM_RADIO_CHANNELS] = {0};
static QueueHandle_t xInputQueue;
gptimer_handle_t pulse_timer = NULL;

esp_err_t setup_radio_and_rotenc_input( void ) {

    ESP_LOGI( TAG, "creating input queue" );

    xInputQueue = xQueueCreate( 10, sizeof( xInputEvent* ) ); // xInputEvent from input.h
    ESP_RETURN_ON_FALSE( xInputQueue != 0, ESP_FAIL, TAG, "xQueueCreate failed" );

    ESP_LOGI( TAG, "configuring RADIO_PPM pin for input, pull_down_en" );

    gpio_config_t radio_pin_conf = {
        .pin_bit_mask = (1ULL << RADIO_PPM),
        .mode = GPIO_MODE_INPUT,
        .pull_down_en = 1,
        .pull_up_en = 0,
    };
    ESP_RETURN_ON_ERROR( gpio_config( &radio_pin_conf ), TAG, "gpio_config failed" );

    ESP_LOGI( TAG, "creating positive edge RADIO_PPM interrupt for radio_pulse callback" );

    ESP_RETURN_ON_ERROR( gpio_set_intr_type( RADIO_PPM, GPIO_INTR_POSEDGE ), TAG, "gpio_set_intr_type failed" );
    ESP_RETURN_ON_ERROR( gpio_isr_handler_add( RADIO_PPM, radio_pulse, NULL ), TAG, "gpio_isr_handler_add failed" );
    ESP_RETURN_ON_ERROR( gpio_intr_enable( RADIO_PPM ), TAG, "gpio_intr_enable failed" );

    ESP_RETURN_ON_ERROR( gpio_wakeup_enable( RADIO_PPM, GPIO_INTR_POSEDGE ), TAG, "gpio_wakeup_enable failed" );

    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1000000, // 1MHz, microsecond precision
    };
    ESP_RETURN_ON_ERROR( gptimer_new_timer( &timer_config, &pulse_timer ), TAG, "gpitimer_new_timer failed" );
    ESP_RETURN_ON_ERROR( gptimer_enable( pulse_timer ), TAG, "gptimer_enable failed" );
    ESP_RETURN_ON_ERROR( gptimer_start( pulse_timer ), TAG, "gptimer_enable failed" );

    TaskHandle_t input_task = NULL;
    xTaskCreate( input_handler_task, "input_handler_task", 400, NULL, 2, &input_task ); // stack size 400 bytes, priority 2

    return ESP_OK;

}

void IRAM_ATTR radio_pulse( void* arg ) {

    // get pulse timer value
    uint64_t offset = 0;
    gptimer_get_raw_count( pulse_timer, &offset );

    // insert timer value into queue
    xInputEvent pulse_event = {
        .type = INPUT_EVENT_RADIOPULSE,
        .data = offset,
    };
    xQueueSendFromISR( xInputQueue, &pulse_event, NULL );

    // reset timer
    gptimer_set_raw_count( pulse_timer, 0 );

    // debug log event
    ESP_DRAM_LOGD( TAG, "INTR: received radio pulse (offset %lu), queued", offset );

}

void input_handler_task( void* arg ) {
    // clears input event queue

    xInputEvent event;
    for ( ;; ) {
        while ( xQueueReceive( xInputQueue, &event, 0 ) ) {

            if ( event.type == INPUT_EVENT_RADIOPULSE ) {
                // ppm radio receiver handling

                if ( event.data >= RADIO_PULSE_STOPGAP_US ) {

                    // pulse gap is larger than designated stopgap time,
                    // indicates end of a stopgap (beginning of transmission);
                    // reset data
                    current_radio_channel = 0;
                    for ( int i = 0; i < NUM_RADIO_CHANNELS; i++ ) {
                        radio_channels[i] = 0;
                    }

                } else {

                    if ( ++current_radio_channel >= NUM_RADIO_CHANNELS ) {
                        // surpassed number of expected pulses without a stopgap;
                        // something's wrong, continue until next stopgap
                        continue;
                    }

                    // valid data; since the time between the Nth pulse and the
                    // Nth-1 pulse indicates the value of channel N-1, set it
                    // accordingly
                    radio_channels[ current_radio_channel - 1 ] = event.data;

                    ESP_LOGD( TAG, "radio channel %lu: %lu", current_radio_channel, event.data );
                }

            } else if ( event.type == INPUT_EVENT_ROTENCSW ) {

                // TODO

            } else if ( event.type == INPUT_EVENT_ROTENCTURN ) {

                // TODO

            }

        }

        taskYIELD(); // queue is empty, don't waste anymore processor time
    }

}
