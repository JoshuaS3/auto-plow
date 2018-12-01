# Raspberry Pi settings
CC = gcc --std=c18
CFLAGS = -Wall -Wextra -pedantic -Os
SOURCES = $(shell find src/rpi -name '*.c')
OBJECTS = $(SOURCES:c=o)
OUT = autoplow


# Arduino Settings
F_CPU = 16000000L
MCU = atmega328p

ARDUINO_CC = avr-gcc
ARDUINO_CXX = avr-g++
ARDUINO_AVRDUDE = avrdude
ARDUINO_OBJCOPY = avr-objcopy

ARDUINO_FLAGS = -c -w -Os -MMD -ffunction-sections -fdata-sections -DF_CPU=$(F_CPU) -DUSB_VID=null -DUSB_PID=null -DARDUINO=106 -mmcu=$(MCU)
ARDUINO_CFLAGS = $(ARDUINO_FLAGS)
ARDUINO_CXXFLAGS = $(ARDUINO_FLAGS) -fno-exceptions

ARDUINO_AVRDUDE_CONF = "include/arduino/avrdude.conf"
ARDUINO_INCLUDE = "include/arduino/"

ARDUINO_CORE_CXX_SRC = $(shell find include/arduino -name '*.cpp')
ARDUINO_CORE_C_SRC = $(shell find include/arduino -name '*.c')
ARDUINO_CORE_ASM_SRC = $(shell find include/arduino -name '*.S')

ARD1_SOURCES = $(shell find src/ard1 -name '*.cpp')

ARDUINO_CORE_C = $(ARDUINO_CORE_C_SRC:%.c=%.o)
ARDUINO_CORE_CXX = $(ARDUINO_CORE_CXX_SRC:%.cpp=%.o)
ARDUINO_CORE_ASM = $(ARDUINO_CORE_ASM_SRC:%.S=%.o)

ARD1_OBJECTS = $(ARD1_SOURCES:%.cpp=%.o)

ARD1_OUT = bin/ard1.hex



.PHONY: all ard1-build

all: $(OUT)

$(OUT): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(OUT)

$(OBJECTS): $(SOURCES)
	$(CC) -c $(CFLAGS) $(SOURCES)


ard1-build: $(ARD1_OUT)

%.o: %.c
	@echo
	@echo Compiling Arduino C file: $< to $@
	$(ARDUINO_CC) $(ARDUINO_CFLAGS) -I $(ARDUINO_INCLUDE) $< -o $@

%.o: %.cpp
	@echo
	@echo Compiling Arduino C++ file: $< to $@
	$(ARDUINO_CXX) $(ARDUINO_CXXFLAGS) -I $(ARDUINO_INCLUDE) $< -o $@

%.o: %.S
	@echo
	@echo Compiling Arduino Assembly file: $< to $@
	$(ARDUINO_CC) $(ARDUINO_CFLAGS) -I $(ARDUINO_INCLUDE) $< -o $@


$(ARD1_OUT): $(ARD1_OBJECTS) $(ARDUINO_CORE_CXX) $(ARDUINO_CORE_C) $(ARDUINO_CORE_ASM)
	@echo
	@echo Compiling Arduino objects to hex file
	$(ARDUINO_CC) -Os -Wl,-Map,bin/main.map -mmcu=$(MCU) $(ARD1_OBJECTS) $(ARDUINO_CORE_CXX) $(ARDUINO_CORE_C) $(ARDUINO_CORE_ASM) -o bin/main.elf
	$(ARDUINO_OBJCOPY) -O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0 bin/main.elf bin/main.eep
	$(ARDUINO_OBJCOPY) -O ihex -R .eeprom bin/main.elf $@
	$(ARDUINO_AVRDUDE) -C$(ARDUINO_AVRDUDE_CONF) -v -p$(MCU) -carduino -P/dev/ttyACM0 -b115200 -D -Uflash:w:$(ARD1_OUT):i