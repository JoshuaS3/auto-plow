// espy Copyright (c) 2023 Josh Stockin <josh@joshstock.in>
// [https://joshstock.in] [https://github.com/joshuas3]
//
// This software is licensed and distributed under the terms of the MIT License.
// See the MIT License in the LICENSE file of this project's root folder.
//
// This comment block and its contents, including this disclaimer, MUST be
// preserved in all copies or distributions of this software's source.


// Filename: main.c
// Description: Program entry point, runs setups functions


#include "esp_err.h"
#include "esp_log.h"
#include "driver/gpio.h"

#include "input.h"
#include "pins.h"


static const char* TAG = "main.c";


void indicate_error ( void ) {

    gpio_reset_pin( LED_RED );

    gpio_config_t led_pin_conf = {
        .pin_bit_mask = (1ULL << LED_RED),
        .mode = GPIO_MODE_OUTPUT,
    };
    gpio_config( &led_pin_conf );

    gpio_set_level( LED_RED, 1 );

}


void app_main( void ) {

#ifdef DEBUG_MODE
#warning DEBUG_MODE active!
    esp_log_level_set( "*", ESP_LOG_DEBUG );
#else
    esp_log_level_set( "*", ESP_LOG_INFO );
#endif

    esp_err_t ret = ESP_OK;

    ESP_LOGI( TAG, "main thread enter" );

    ESP_LOGI( TAG, "installing GPIO ISR service" );
    gpio_install_isr_service( 0 );

    ESP_LOGI( TAG, "running input setup" );
    if ( (ret = setup_radio_and_rotenc_input()) != ESP_OK ) {
        ESP_ERROR_CHECK_WITHOUT_ABORT( ret );
        indicate_error();
        return;
    }
    /*

    ESP_LOGI( TAG, "running control hardware setup" );
    if ( (ret = setup_control_hardware()) != ESP_OK ) {
        ESP_ERROR_CHECK_WITHOUT_ABORT( ret );
        indicate_error();
        return;
    }

    ESP_LOGI( TAG, "running control loop setup" );
    if ( (ret = setup_control_loop()) != ESP_OK ) {
        ESP_ERROR_CHECK_WITHOUT_ABORT( ret );
        indicate_error();
        return;
    }
    */

    ESP_LOGI( TAG, "main thread exit" );
    return;

}
