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

static uint8_t current_radio_channel = 0;
static uint16_t radio_channels[NUM_RADIO_CHANNELS + 1] = {0};
static bool updated = false;

QueueHandle_t xInputQueue;
gptimer_handle_t pulse_timer = NULL;

esp_err_t setup_radio_and_rotenc_input( void ) {

    ESP_LOGI( TAG, "creating input queue" );

    xInputQueue = xQueueCreate( 400, sizeof( xInputEvent ) ); // xInputEvent from input.h
    ESP_RETURN_ON_FALSE( xInputQueue != 0, ESP_FAIL, TAG, "xQueueCreate failed" );

    ESP_LOGI( TAG, "configuring RADIO_PPM pin for input, pull_down_en" );

    gpio_config_t radio_pin_conf = {
        .pin_bit_mask = (1ULL << RADIO_PPM),
        .mode = GPIO_MODE_INPUT,
        .pull_down_en = 1,
        .pull_up_en = 0,
    };
    ESP_RETURN_ON_ERROR( gpio_config( &radio_pin_conf ), TAG, "gpio_config failed" );

    ESP_LOGI( TAG, "configuring and starting hardware gptimer (1MHz)" );

    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1000000, // 1MHz, microsecond precision
    };
    ESP_RETURN_ON_ERROR( gptimer_new_timer( &timer_config, &pulse_timer ), TAG, "gptimer_new_timer failed" );
    ESP_RETURN_ON_ERROR( gptimer_enable( pulse_timer ), TAG, "gptimer_enable failed" );
    ESP_RETURN_ON_ERROR( gptimer_start( pulse_timer ), TAG, "gptimer_start failed" );

    ESP_LOGI( TAG, "creating positive edge RADIO_PPM interrupt for radio_pulse callback" );

    ESP_RETURN_ON_ERROR( gpio_set_intr_type( RADIO_PPM, GPIO_INTR_POSEDGE ), TAG, "gpio_set_intr_type failed" );
    ESP_RETURN_ON_ERROR( gpio_isr_handler_add( RADIO_PPM, radio_pulse, NULL ), TAG, "gpio_isr_handler_add failed" );
    ESP_RETURN_ON_ERROR( gpio_intr_enable( RADIO_PPM ), TAG, "gpio_intr_enable failed" );

    // wakeup_enable not allowed for rising edge interrupts
    // TODO: move to sleep handler
    // ESP_RETURN_ON_ERROR( gpio_wakeup_enable( RADIO_PPM, GPIO_INTR_POSEDGE ), TAG, "gpio_wakeup_enable failed" );

    ESP_LOGI( TAG, "creating input handler task" );

    TaskHandle_t input_task = NULL;
    xTaskCreate( input_handler_task, "input_handler_task", 2048, NULL, 20, &input_task ); // stack size 400 bytes, priority 2

    ESP_LOGI( TAG, "input setup done" );

    return ESP_OK;

}

void IRAM_ATTR radio_pulse( void* arg ) {

    // get pulse timer value
    uint64_t offset = 0;
    gptimer_get_raw_count( pulse_timer, &offset );

    if (offset >= RADIO_PULSE_STOPGAP_US || current_radio_channel == NUM_RADIO_CHANNELS) {
        radio_channels[NUM_RADIO_CHANNELS] = offset;
        current_radio_channel = 0;
    } else {
        radio_channels[current_radio_channel++] = offset;
    }
    updated = true;

    // reset timer
    gptimer_set_raw_count( pulse_timer, 0 );

}

void input_handler_task( void* arg ) {
    // clears input event queue

    //xInputEvent event;
    for ( ;; ) {
        /*
        while ( xQueueReceive( xInputQueue, &event, 2 ) ) {

            if ( event.type == INPUT_EVENT_RADIOPULSE ) {
                // ppm radio receiver handling

            } else if ( event.type == INPUT_EVENT_ROTENCSW ) {

                // TODO

            } else if ( event.type == INPUT_EVENT_ROTENCTURN ) {

                // TODO

            }

        }*/
        if (updated) {
            ESP_LOGD( TAG, "\nCHANNELS\nch1:  %u\nch2:  %u\nch3:  %u\nch4:  %u\nch5:  %u\nch6:  %u\nch7:  %u\nch8:  %u\nch9:  %u\nch10: %u\ngap dist: %u\n", radio_channels[0], radio_channels[1], radio_channels[2], radio_channels[3], radio_channels[4], radio_channels[5], radio_channels[6], radio_channels[7], radio_channels[8], radio_channels[9], radio_channels[10] );
            updated = false;
        }

        vTaskDelay(10);
        taskYIELD(); // queue is empty, don't waste anymore processor time
    }

}
