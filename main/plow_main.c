// plow_main.c
// Main logic controller for the snow plow
// Copyright (c) Joshua 'joshuas3' Stockin 2020

#include "plow_main.h"
#include "pulseIn.h"
#include "util.h"

#include <stdio.h>

#include "esp_system.h"
#include "esp_task_wdt.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/uart.h"

static void onrestart(void) {
	gpio_set_level(STATUS_RED, HIGH);
	delay(250);
}

static int ping_count = 1;
static void pingTask(void * pingOK) {
	configASSERT(((bool)pingOK) == true);
	static int beeps;
	static int i;
	for (;;) {
		delay(3000);
		printf("ping #%dx%d: %i ticks\n", ++beeps, ping_count, xTaskGetTickCount());
		for (i = 0; i < ping_count; i++) {
			gpio_set_level(STATUS_YELLOW, HIGH);
			delay(0200);
			gpio_set_level(STATUS_YELLOW, LOW);
			delay(0200);
		}
	}
}

const int uart_blade_control = UART_NUM_1;
const int uart_motor_control = UART_NUM_2;
static void radioTask(void * arg) {
	configASSERT(((bool)arg) == true);
	unsigned int R1;
	unsigned int R2;
	unsigned int R3;
	unsigned int R4;
	unsigned int R5;
	unsigned int R6;
	unsigned char SWITCH_MOTORS = 0;
	unsigned char SWITCH_SALT = 0;
	for (;;) {
		R1 = pulseIn(RADIO_1, true);
		R2 = pulseIn(RADIO_2, true);
		R3 = pulseIn(RADIO_3, true);
		R4 = pulseIn(RADIO_4, true);
		R5 = pulseIn(RADIO_5, true);
		R6 = pulseIn(RADIO_6, true);
		if (R1 || R2 || R3 || R4 || R5 || R6) {
			ping_count = 2;
			unsigned int R1m = map(R1, 160000, 320000, 0, 1000); // right horizontal
			unsigned int R2m = map(R2, 160000, 320000, 0, 1000); // right vertical
			unsigned int R3m = map(R3, 160000, 320000, 0, 1000); // left vertical
			unsigned int R4m = map(R4, 160000, 320000, 0, 1000); // left horizontal
			unsigned char R5m = map(R5, 160000, 320000, 0, 1000)>500;
			unsigned char R6m = map(R6, 160000, 320000, 0, 1000)>500;

			// Switches
			if (SWITCH_SALT != R6m) {
				printf("salt spreader switch flip\n");
				SWITCH_SALT = R6m;
				gpio_set_level(SOLENOID_SALT, !R6m);
			}
			if (SWITCH_MOTORS != R5m) {
				printf("motors switch flip\n");
				SWITCH_MOTORS = R5m;
				if (R5m) {
					gpio_set_level(BATTERY_MOTORCONTROL, !R5m);
					gpio_set_level(BATTERY_BLADECONTROL, !R5m);
					delay(500);
					gpio_set_level(SOLENOID_MOTORS, !R5m);
				} else {
					gpio_set_level(SOLENOID_MOTORS, !R5m);
					delay(500);
					gpio_set_level(BATTERY_MOTORCONTROL, !R5m);
					gpio_set_level(BATTERY_BLADECONTROL, !R5m);
				}
			}

			// Motor controller configuration
			if (R5m) {
				// Blade control
				unsigned int blade_pitch = 192;
				unsigned int blade_yaw = 64;
				if (R3m > 600) {
					blade_pitch -= map(R3m, 600, 1000, 1, 63);
				} else if (R3m < 400) {
					blade_pitch += (64 - map(R3m, 0, 400, 1, 63));
				}
				if (R4m > 600) {
					blade_yaw = map(R4m, 600, 1000, 64, 127);
				} else if (R4m < 400) {
					blade_yaw = map(R4m, 0, 400, 1, 64);
				}

				// Motor control
				short motor_left = 64;
				short motor_right = 192;
				if (R2m > 600) {
					motor_left = map(R2m, 600, 1000, 64, 127);
					motor_right = map(R2m, 600, 1000, 192, 255);
				} else if (R2m < 400) {
					motor_left = map(R2m, 0, 400, 1, 64);
					motor_right = map(R2m, 0, 400, 128, 192);
				}
				if (R1m > 600) { // right turn
					motor_left += map(R1m, 600, 1000, 1, 63);
					motor_right -= map(R1m, 600, 1000, 1, 63);
				} else if (R1m < 400) { // left turn
					motor_left -= (64 - map(R1m, 0, 400, 1, 63));
					motor_right += (64 - map(R1m, 0, 400, 1, 63));
				}

				// Constrain motor signals
				if (motor_left < 1) motor_left = 1;
				if (motor_left > 127) motor_left = 127;
				if (motor_right < 128) motor_right = 128;
				if (motor_right > 255) motor_right = 255;

				printf("Motors:\tpitch\tyaw\tleft\tright\n");
				printf("Motors:\t%i\t%i\t%i\t%i\n", blade_pitch, blade_yaw, motor_left, motor_right);

				char motor_bytes[] = {motor_left, motor_right};
				uart_write_bytes(uart_motor_control, (const char*)motor_bytes, 2);

				char blade_bytes[] = {blade_pitch, blade_yaw};
				uart_write_bytes(uart_blade_control, (const char*)blade_bytes, 2);
			} else {
				printf("Radio RX:\t%i\t%i\t%i\t%i\t%i\t%i\n", R1m, R2m, R3m, R4m, R5m, R6m);
			}
		} else {
			ping_count = 1;
			printf("Radio RX:\t-\t-\t-\t-\t-\t-\n");
			if (SWITCH_SALT != 0) {
				printf("salt spreader switch flip\n");
				SWITCH_SALT = 0;
				gpio_set_level(SOLENOID_SALT, HIGH);
			}
			if (SWITCH_MOTORS != 0) {
				printf("motors switch flip\n");
				SWITCH_MOTORS = 0;
				gpio_set_level(SOLENOID_MOTORS, HIGH);
				delay(200);
				gpio_set_level(BATTERY_MOTORCONTROL, HIGH);
				gpio_set_level(BATTERY_BLADECONTROL, HIGH);
			}
		}
		esp_task_wdt_reset();
		delay(10);
	}
}

void app_main(void)
{
	printf("entering app_main\n");

	printf("configuring STATUS_[GREEN|YELLOW|RED] GPIO output\n");
	gpio_config_t io_conf;
	io_conf.pin_bit_mask = (1ULL<<STATUS_GREEN) | (1ULL<<STATUS_YELLOW) | (1ULL<<STATUS_RED);
	io_conf.mode = GPIO_MODE_OUTPUT;
	io_conf.intr_type = GPIO_INTR_DISABLE;
	gpio_config(&io_conf);

	printf("pulling STATUS_[GREEN|YELLOW|RED] HIGH\n");
	gpio_set_level(STATUS_GREEN, HIGH);
	gpio_set_level(STATUS_YELLOW, HIGH);
	gpio_set_level(STATUS_RED, HIGH);

	printf("configuring BATTERY_[MOTOR|BLADE]CONTROL, SOLENOID_[SALT|MOTORS], and RELAY_BOARD GPIO output\n");
	io_conf.pin_bit_mask = (1ULL<<SOLENOID_SALT) | (1ULL<<SOLENOID_MOTORS) | (1ULL<<BATTERY_MOTORCONTROL) | (1ULL<<BATTERY_BLADECONTROL) | (1ULL<<RELAY_BOARD);
	io_conf.mode = GPIO_MODE_OUTPUT;
	gpio_config(&io_conf);

	printf("pulling all high\n");
	gpio_set_level(SOLENOID_SALT, HIGH);
	gpio_set_level(SOLENOID_MOTORS, HIGH);
	gpio_set_level(BATTERY_MOTORCONTROL, HIGH);
	gpio_set_level(BATTERY_BLADECONTROL, HIGH);
	gpio_set_level(RELAY_BOARD, HIGH);

	printf("configuring RADIO_[1-6] GPIO input\n");
	io_conf.pin_bit_mask = (1ULL<<RADIO_1) | (1ULL<<RADIO_2) | (1ULL<<RADIO_3) | (1ULL<<RADIO_4) | (1ULL<<RADIO_5) | (1ULL<<RADIO_6);
	io_conf.mode = GPIO_MODE_INPUT;
	gpio_config(&io_conf);

	printf("configuring UART\n");
	uart_config_t uart_config = {
		.baud_rate = CONTROL_BAUDRATE,
		.data_bits = UART_DATA_8_BITS,
		.parity = UART_PARITY_DISABLE,
		.stop_bits = UART_STOP_BITS_1,
		.flow_ctrl = UART_HW_FLOWCTRL_DISABLE
	};
	ESP_ERROR_CHECK(uart_param_config(uart_motor_control, &uart_config));
	ESP_ERROR_CHECK(uart_param_config(uart_blade_control, &uart_config));
	ESP_ERROR_CHECK(uart_set_pin(uart_motor_control, UART_MOTOR_CONTROL, NULL_PIN, NULL_PIN, NULL_PIN));
	ESP_ERROR_CHECK(uart_set_pin(uart_blade_control, UART_BLADE_CONTROL, NULL_PIN, NULL_PIN, NULL_PIN));

	printf("installing UART\n");
	const int uart_buffer_size = 256;
	ESP_ERROR_CHECK(uart_driver_install(uart_motor_control, uart_buffer_size, uart_buffer_size, 0, NULL, 0));
	ESP_ERROR_CHECK(uart_driver_install(uart_blade_control, uart_buffer_size, uart_buffer_size, 0, NULL, 0));
	ESP_ERROR_CHECK(uart_set_mode(uart_motor_control, UART_MODE_UART));
	ESP_ERROR_CHECK(uart_set_mode(uart_blade_control, UART_MODE_UART));

	printf("registering shutdown function\n");
	esp_register_shutdown_handler(onrestart);

	printf("creating ping task\n");
	TaskHandle_t pingTaskHandle = NULL;
	xTaskCreate(pingTask, "pingTask", 2000, (void *)true, tskIDLE_PRIORITY, &pingTaskHandle);

	printf("creating radio task\n");
	TaskHandle_t radioTaskHandle = NULL;
	xTaskCreate(radioTask, "radioTask", 10000, (void *)true, 1, &radioTaskHandle);

	printf("pulling STATUS_[YELLOW|RED] LOW\n");
	gpio_set_level(STATUS_YELLOW, LOW);
	gpio_set_level(STATUS_RED, LOW);
}
