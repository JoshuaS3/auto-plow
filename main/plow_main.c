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

static void radioTask(void * arg) {
	configASSERT(((bool)arg) == true);
	unsigned int R1;
	unsigned int R2;
	unsigned int R3;
	unsigned int R4;
	unsigned int R5;
	unsigned int R6;
	for (;;) {
		R1 = pulseIn(RADIO_1, true);
		R2 = pulseIn(RADIO_2, true);
		R3 = pulseIn(RADIO_3, true);
		R4 = pulseIn(RADIO_4, true);
		R5 = pulseIn(RADIO_5, true);
		R6 = pulseIn(RADIO_6, true);
		if ((R1 & R2 & R3 & R4 & R5 & R6) != 0) {
			ping_count = 2;
			unsigned int R1m = map(R1, 160000, 320000, 0, 1000);
			unsigned int R2m = map(R2, 160000, 320000, 0, 1000);
			unsigned int R3m = map(R3, 160000, 320000, 0, 1000);
			unsigned int R4m = map(R4, 160000, 320000, 0, 1000);
			unsigned int R5m = map(R5, 160000, 320000, 0, 1000)>500;
			unsigned int R6m = map(R6, 160000, 320000, 0, 1000)>500;
			printf("Radio RX:\t%i\t%i\t%i\t%i\t%i\t%i\n", R1m, R2m, R3m, R4m, R5m, R6m);
		} else {
			ping_count = 1;
			printf("Radio RX:\t-\t-\t-\t-\t-\t-\n");
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
	io_conf.pull_up_en = 0;
	io_conf.pull_down_en = 1;
	io_conf.intr_type = GPIO_INTR_DISABLE;
	gpio_config(&io_conf);

	printf("pulling STATUS_[GREEN|YELLOW|RED] HIGH\n");
	gpio_set_level(STATUS_GREEN, HIGH);
	gpio_set_level(STATUS_YELLOW, HIGH);
	gpio_set_level(STATUS_RED, HIGH);

	printf("configuring RADIO_[1-6] GPIO input\n");
	io_conf.pin_bit_mask = (1ULL<<RADIO_1) | (1ULL<<RADIO_2) | (1ULL<<RADIO_3) | (1ULL<<RADIO_4) | (1ULL<<RADIO_5) | (1ULL<<RADIO_6);
	io_conf.mode = GPIO_MODE_INPUT;
	io_conf.pull_up_en = 0;
	io_conf.pull_down_en = 0;
	io_conf.intr_type = GPIO_INTR_DISABLE;
	gpio_config(&io_conf);

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
