# Driver settings
CXX = g++ --std=c++17
CXXFLAGS = -Wall -Wextra -pedantic -Os
SOURCES = $(shell find src/rpi -name '*.cpp')
OBJECTS = $(SOURCES:%.cpp=%.o)
OUT = autoplow

.PHONY: all ard1-build clean

all: $(OUT)

$(OUT): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o $@

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $^ -o $@


# Arduino settings
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

ARDUINO_CORE_C = $(ARDUINO_CORE_C_SRC:%.c=%.ard)
ARDUINO_CORE_CXX = $(ARDUINO_CORE_CXX_SRC:%.cpp=%.ard)
ARDUINO_CORE_ASM = $(ARDUINO_CORE_ASM_SRC:%.S=%.ard)

ARD1_OBJECTS = $(ARD1_SOURCES:%.cpp=%.ard)

ARD1_OUT = bin/ard1.hex


ard1-build: $(ARD1_OUT)

%.ard: %.c
	@echo
	@echo Compiling Arduino C file: $< to $@
	$(ARDUINO_CC) $(ARDUINO_CFLAGS) -I $(ARDUINO_INCLUDE) $< -o $@

%.ard: %.cpp
	@echo
	@echo Compiling Arduino C++ file: $< to $@
	$(ARDUINO_CXX) $(ARDUINO_CXXFLAGS) -I $(ARDUINO_INCLUDE) $< -o $@

%.ard: %.S
	@echo
	@echo Compiling Arduino Assembly file: $< to $@
	$(ARDUINO_CC) $(ARDUINO_CFLAGS) -I $(ARDUINO_INCLUDE) $< -o $@


$(ARD1_OUT): $(ARD1_OBJECTS) $(ARDUINO_CORE_CXX) $(ARDUINO_CORE_C) $(ARDUINO_CORE_ASM)
	@echo
	@echo
	@echo
	@echo
	@echo
	@echo Compiling Arduino objects to $(MCU) hex
	mkdir -p bin/
	$(ARDUINO_CC) -Os -Wl,-Map,bin/main.map -mmcu=$(MCU) $(ARD1_OBJECTS) $(ARDUINO_CORE_CXX) $(ARDUINO_CORE_C) $(ARDUINO_CORE_ASM) -o bin/main.elf
	$(ARDUINO_OBJCOPY) -O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0 bin/main.elf bin/main.eep
	$(ARDUINO_OBJCOPY) -O ihex -R .eeprom bin/main.elf $@
	@echo
	@echo
	@echo
	@echo
	@echo
	@echo Uploading $(ARD1_OUT) to /dev/ttyACM0
	$(ARDUINO_AVRDUDE) -C$(ARDUINO_AVRDUDE_CONF) -p$(MCU) -carduino -P/dev/ttyACM0 -b38400 -D -Uflash:w:$(ARD1_OUT):i

clean:
	rm -rf include/*.o
	rm -rf include/*.d
	rm -rf src/rpi/*.o
	rm -rf src/rpi/*.d
	rm -rf src/ard1/*.ard
	rm -rf src/ard1/*.d
	rm -rf bin/*.hex
	rm -rf bin/*.eep
	rm -rf bin/*.elf
	rm -rf bin/*.map
	rm -f autoplow
	rm -rf bin/
