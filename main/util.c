// util.c
// Basic utilities
// Copyright (c) Joshua 'joshuas3' Stockin 2019

#include "util.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

int ticks_to_ms(int ticks) {
	return (int)(ticks * portTICK_PERIOD_MS);
}
int ms_to_ticks(int ms) {
	return (int)(ms / portTICK_PERIOD_MS);
}

void delay(int ms) {
	vTaskDelay(ms_to_ticks(ms));
}
