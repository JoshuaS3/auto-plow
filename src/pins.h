// espy Copyright (c) 2023 Josh Stockin <josh@joshstock.in>
// [https://joshstock.in] [https://github.com/joshuas3]
//
// This software is licensed and distributed under the terms of the MIT License.
// See the MIT License in the LICENSE file of this project's root folder.
//
// This comment block and its contents, including this disclaimer, MUST be
// preserved in all copies or distributions of this software's source.


// Filename: pins.h
// Description: GPIO pin definition header.
//
// Values >=40 are on expander board:
// 40-47 are GPA0-GPA7, 48-55 are GPB0-GPB8
//
// The expander board is controlled via I2C interface. PIN_LED_RED is a
// reserved controller pin to indicate errors in case the I2C controller
// malfunctions.


#pragma once


// LEDs

#define LED_BLU 40
#define LED_GRN 41
#define LED_YEL 55
#define LED_ORG 54
#define LED_RED 12


// Inputs/outputs/buses

#define SPI_DATA 13
#define SPI_CLK 14
#define I2C_DATA 21
#define I2C_CLK 22

#define CS_DIGIPOT 15
#define CS_DISPLAY 26

#define EPD_DC 4
#define EPD_BUSY 16

#define RADIO_PPM 25

#define ROTARYENC_CLK 32
#define ROTARYENC_DT  33
#define ROTARYENC_SW  23


// Sensors

#define SENS_HUMIDITY 5
#define SENS_TEMP1 18
#define SENS_TEMP2 27
#define SENS_VOLT 34


// Relays

#define RELAY_MOTOR_DRV_PWR  42 // U3-IN8 (SYM-PIN from schematic)
#define RELAY_MOTOR_DRV_RDIR 45 // U3-IN3
#define RELAY_MOTOR_DRV_LDIR 46 // U3-IN2
#define RELAY_MOTOR_DRV_SW   47 // U3-IN1

#define RELAY_SALTSPREAD 43 // U3-IN7
#define RELAY_HEADLIGHTS 44 // U3-IN5

#define RELAY_MOTOR_UD_PWR  52 // U11-IN5
#define RELAY_MOTOR_UD_CTL1 48 // U11-IN1
#define RELAY_MOTOR_UD_CTL2 49 // U11-IN2

#define RELAY_MOTOR_LR_PWR  53 // U11-IN6
#define RELAY_MOTOR_LR_CTL1 50 // U11-IN3
#define RELAY_MOTOR_LR_CTL2 51 // U11-IN4
