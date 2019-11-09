// plow_main.c
// Main logic controller for the snow plow
// Copyright (c) Joshua 'joshuas3' Stockin 2019

#include "plow_main.h"
#include "pulseIn.h"
#include "util.h"

#include <stdio.h>

#include "esp_system.h"
#include "esp_task_wdt.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

static void pingTask(void * pingOK) {
	configASSERT(((bool)pingOK) == true);
	static int beeps;
	for (;;) {
		delay(2400);
		printf("ping x%d - %i ticks\n", ++beeps, xTaskGetTickCount());
		gpio_set_level(STATUS_YELLOW, HIGH);
		delay(0200);
		gpio_set_level(STATUS_YELLOW, LOW);
		delay(0200);
		gpio_set_level(STATUS_YELLOW, HIGH);
		delay(0200);
		gpio_set_level(STATUS_YELLOW, LOW);
	}
}

static void radioTask(void * arg) {
	configASSERT(((bool)arg) == true);
	int R1;
	int R2;
	int R3;
	int R4;
	int R5;
	int R6;
	for (;;) {
		R1 = pulseIn(RADIO_1, true);
		R2 = pulseIn(RADIO_2, true);
		R3 = pulseIn(RADIO_3, true);
		R4 = pulseIn(RADIO_4, true);
		R5 = pulseIn(RADIO_5, true);
		R6 = pulseIn(RADIO_6, true);
		printf("%i\t%i\t%i\t%i\t%i\t%i\n", R1, R2, R3, R4, R5, R6);
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

	printf("configuring RADIO_[1-6] GPIO input\n");
	io_conf.pin_bit_mask = (1ULL<<RADIO_1) | (1ULL<<RADIO_2) | (1ULL<<RADIO_3) | (1ULL<<RADIO_4) | (1ULL<<RADIO_5) | (1ULL<<RADIO_6);
	io_conf.mode = GPIO_MODE_INPUT;
	io_conf.pull_up_en = 0;
	io_conf.pull_down_en = 0;
	io_conf.intr_type = GPIO_INTR_DISABLE;
	gpio_config(&io_conf);

	printf("creating ping task\n");
	TaskHandle_t pingTaskHandle = NULL;
	xTaskCreate(pingTask, "pingTask", 2000, (void *)true, tskIDLE_PRIORITY, &pingTaskHandle);

	printf("creating radio task\n");
	TaskHandle_t radioTaskHandle = NULL;
	xTaskCreate(radioTask, "radioTask", 10000, (void *)true, 1, &radioTaskHandle);

	printf("setting STATUS_GREEN HIGH\n");
	gpio_set_level(STATUS_GREEN, HIGH);
}
