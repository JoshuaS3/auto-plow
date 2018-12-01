# Raspberry Pi settings
CC = gcc --std=c11
CCFLAGS = -Wall -Wextra -pedantic -O3 -g
SOURCES = $(shell find src/rpi -name '*.c')
OBJECTS = $(SOURCES:%.c=%.o)
OUT = autoplow


# Arduino Settings
F_CPU = 16000000L
MCU = atmega328p

ARDUINO_CC = avr-gcc
ARDUINO_CXX = avr-g++
ARDUINO_AR = avr-ar
ARDUINO_OBJCOPY = avr-objcopy

ARDUINO_FLAGS = -c -g -Os -Wall -Wextra -pedantic -MMD -ffunction-sections -fdata-sections -mmcu=$(MCU) -DF_CPU=$(F_CPU) -DUSB_VID=null -DUSB_PID=null -DARDUINO=106
ARDUINO_CC_FLAGS = $(ARDUINO_FLAGS)
ARDUINO_CXX_FLAGS = $(ARDUINO_FLAGS) -fno-exceptions

ARDUINO_INCLUDE = "include/arduino/"

ARD1_SOURCES = $(shell find src/ard1 -name '*.cpp')
ARD1_CORE_CXX = $(shell find include/arduino -name '*.cpp')
ARD1_CORE_CC = $(shell find include/arduino -name '*.c')
ARD1_OUT = bin/ard1.hex
# ARD2_SOURCES = $(shell find src/ard2 -name '*.cpp')


.PHONY: all ard1-build ard1-upload

all: $(OUT) $(OBJECTS)

$(OUT): $(OBJECTS)
	$(CC) $(CCFLAGS) $(OBJECTS) -o $(OUT)

ard1-build: $(ARD1_OUT) $(ARD1_SOURCES) $(ARD1_CORE_CXX) $(ARD1_CORE_CC)
	$(ARDUINO_CXX) $(ARDUINO_CXX_FLAGS) -I$(ARDUINO_INCLUDE) $(ARD1_CORE_CC) -o $(ARD1_CORE_CC:%.c=%.o)
	$(ARDUINO_CXX) $(ARDUINO_CXX_FLAGS) -I$(ARDUINO_INCLUDE) $(ARD1_CORE_CXX) -o $(ARD1_CORE_CXX:%.cpp=%.o)
	$(ARDUINO_CXX) $(ARDUINO_CXX_FLAGS) -I$(ARDUINO_INCLUDE) $(ARD1_SOURCES) -o $(ARD1_SOURCES:%.cpp=%.o)
	$(ARDUINO_AR) rcs core.a $(shell find include/arduino -name '*.o')
	$(ARDUINO_AR) rcs core.a $(ARD1_SOURCES:%.cpp:%.o)
	$(ARDUINO_CC) -Os -Wl,--gc-sections -mmcu=$(MCU) -o $(ARD1_SOURCES:%.cpp:%.elf) $(ARD1_SOURCES:%.cpp:%.o) core.a -lm
	$(ARDUINO_OBJCOPY) -O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0 $(ARD1_SOURCES:%.cpp:%.elf) $(ARD1_SOURCES:%.cpp:%.eep)
	$(ARDUINO_OBJCOPY) -O ihex -R .eeprom $(ARD1_SOURCES:%.cpp:%.elf) $(ARD1_OUT)
	rm core.a