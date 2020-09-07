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

int map(int input, int min_in, int max_in, int min_out, int max_out) {
    float range_in = max_in - min_in;
    float range_out = max_out - min_out;

    if (input < min_in) input = min_in;
    else if (input > max_in) input = max_in;

    float lerp = (input - min_in) / range_in;

    return (int)((lerp * range_out) + min_out);
}
