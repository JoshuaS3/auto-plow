# Raspberry Pi settings
CC = gcc --std=c11
CCFLAGS = -Wall -Wextra -pedantic -Os
SOURCES = $(shell find src/rpi -name '*.c')
OBJECTS = $(SOURCES:c=o)
OUT = autoplow


# Arduino Settings
F_CPU = 16000000L
MCU = atmega328p

ARDUINO_CC = avr-gcc
ARDUINO_CXX = avr-g++
ARDUINO_AR = avr-ar
ARDUINO_OBJCOPY = avr-objcopy

ARDUINO_FLAGS = -c -w -Os -MMD -ffunction-sections -fdata-sections -DF_CPU=$(F_CPU) -DUSB_VID=null -DUSB_PID=null -DARDUINO=106 -mmcu=$(MCU)
ARDUINO_CC_FLAGS = $(ARDUINO_FLAGS)
ARDUINO_CXX_FLAGS = $(ARDUINO_FLAGS) -fno-exceptions

ARDUINO_INCLUDE = "include/arduino/"
ARDUINO_CORE_CXX_SRC = $(shell find include/arduino -name '*.cpp')
ARDUINO_CORE_CXX = $(ARDUINO_CORE_CXX_SRC:cpp=o)
ARDUINO_CORE_CC_SRC = $(shell find include/arduino -name '*.c')
ARDUINO_CORE_CC = $(ARDUINO_CORE_CC_SRC:c=o)

ARD1_SOURCES = $(shell find src/ard1 -name '*.cpp')
ARD1_OBJECTS = $(ARD1_SOURCES:%.cpp=%.o)
ARD1_OUT = bin/ard1.hex
# ARD2_SOURCES = $(shell find src/ard2 -name '*.cpp')

ARDUINO_ALL_OBJECTS := $(ARD1_OBJECTS)
ARDUINO_ALL_OBJECTS += $(ARDUINO_CORE_CXX)
ARDUINO_ALL_OBJECTS += $(ARDUINO_CORE_CC)

.PHONY: all ard1-build

all: $(OUT)

$(OUT): $(OBJECTS)
	$(CC) $(CCFLAGS) $(OBJECTS) -o $(OUT)


ard1-build: $(ARD1_OUT)

$(ARDUINO_CORE_CC): $(ARDUINO_CORE_CC_SRC)
	$(ARDUINO_CC) $(ARDUINO_CC_FLAGS) -I$(ARDUINO_INCLUDE) $< -o $@

$(ARDUINO_CORE_CXX): $(ARDUINO_CORE_CXX_SRC)
	$(ARDUINO_CXX) $(ARDUINO_CXX_FLAGS) -I$(ARDUINO_INCLUDE) $< -o $@

$(ARD1_OBJECTS): $(ARD1_SOURCES) $(ARDUINO_CORE_CXX) $(ARDUINO_CORE_CC)
	$(ARDUINO_CXX) $(ARDUINO_CXX_FLAGS) -I$(ARDUINO_INCLUDE) $< -o $@


$(ARD1_OUT): $(ARDUINO_ALL_OBJECTS) $(ARD1_OBJECTS) $(ARDUINO_CORE_CXX) $(ARDUINO_CORE_CC)
	$(ARDUINO_AR) rcs core.a $<
	$(ARDUINO_CC) -Os -Wl,--gc-sections -mmcu=$(MCU) -o $(ARD1_SOURCES:cpp=elf) $(ARD1_OBJECTS) core.a -lm
	$(ARDUINO_OBJCOPY) -O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0 $(ARD1_SOURCES:cpp=elf) $(ARD1_SOURCES:cpp=eep)
	$(ARDUINO_OBJCOPY) -O ihex -R .eeprom $(ARD1_SOURCES:cpp=elf) $@
	rm core.a