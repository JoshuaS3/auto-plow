# Raspberry Pi settings
CC = gcc --std=c11
CCFLAGS = -Wall -Wextra -pedantic -O3 -g
SOURCES = $(shell find src/rpi -name '*.c')
OBJECTS = $(SOURCES:%.c=bin/%.o)
OUT = autoplow


# Arduino Settings
F_CPU = 16000000L
MCU = atmega328p
ARDUINO_CC = avr-gcc
ARDUINO_CXX = avr-g++
ARDUINO_AR = avr-ar
ARDUINO_FLAGS = -c -g -Os -Wall -Wextra -pedantic -MMD -ffunction-sections -fdata-sections -mmcu=$(MCU) -DF_CPU=$(F_CPU) -DUSB_VID=null -DUSB_PID=null -DARDUINO=106
ARDUINO_CC_FLAGS = $(ARDUINO_FLAGS)
ARDUINO_CPP_FLAGS = $(ARDUINO_FLAGS) -fno-exceptions
ARDUINO_INCLUDE = "include/arduino/"
ARD1_SOURCES = $(shell find src/ard1 -name '*.c')
# ARD2_SOURCES = $(shell find src/ard2 -name '*.c')


.PHONY: all ard1-build ard1-upload

all: $(EXEC) $(OBJECTS)

$(EXEC): $(OBJECTS)
	$(CC) $(CXXFLAGS) $(OBJECTS) -o $(OUT)

ard1-build: $(ARD1_SOURCES)
	$(ARDUINO_CXX) $(ARDUINO_CC_FLAGS) -I$(ARDUINO_INCLUDE) $(ARD1_SOURCES) -o bin/$(ARD1_SOURCES:%.c=%.o)