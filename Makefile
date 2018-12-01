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
OBJ_COPY = avr-objcopy
ARDUINO_FLAGS = -c -g -Os -Wall -Wextra -pedantic -MMD -ffunction-sections -fdata-sections -mmcu=$(MCU) -DF_CPU=$(F_CPU) -DUSB_VID=null -DUSB_PID=null -DARDUINO=106
ARDUINO_CC_FLAGS = $(ARDUINO_FLAGS)
ARDUINO_CXX_FLAGS = $(ARDUINO_FLAGS) -fno-exceptions
ARDUINO_INCLUDE = "include/arduino/"
ARD1_SOURCES = $(shell find src/ard1 -name '*.cpp')
ARD1_OUT = bin/ard1.hex
# ARD2_SOURCES = $(shell find src/ard2 -name '*.cpp')


.PHONY: all ard1-build ard1-upload

all: $(OUT) $(OBJECTS)

$(OUT): $(OBJECTS)
	$(CC) $(CCFLAGS) $(OBJECTS) -o $(OUT)

ard1-build: $(ARD1_OUT) $(ARD1_SOURCES)

$(ARD1_OUT): $(ARD1_SOURCES)
	$(ARDUINO_CXX) $(ARDUINO_CXX_FLAGS) -I$(ARDUINO_INCLUDE) $(ARD1_SOURCES) -o $(ARD1_SOURCES:%.cpp=%.o)
	$(ARDUINO_CXX) $(ARDUINO_CXX_FLAGS) -I$(ARDUINO_INCLUDE) ${ARDUINO_INCLUDE}abi.cpp -o $(ARDUINO_INCLUDE)abi.o
	$(ARDUINO_CXX) $(ARDUINO_CXX_FLAGS) -I$(ARDUINO_INCLUDE) ${ARDUINO_INCLUDE}CDC.cpp -o $(ARDUINO_INCLUDE)CDC.o
	$(ARDUINO_CXX) $(ARDUINO_CXX_FLAGS) -I$(ARDUINO_INCLUDE) ${ARDUINO_INCLUDE}HardwareSerial.cpp -o $(ARDUINO_INCLUDE)HardwareSerial.o
	$(ARDUINO_CXX) $(ARDUINO_CXX_FLAGS) -I$(ARDUINO_INCLUDE) ${ARDUINO_INCLUDE}HardwareSerial0.cpp -o $(ARDUINO_INCLUDE)HardwareSerial0.o
	$(ARDUINO_CXX) $(ARDUINO_CXX_FLAGS) -I$(ARDUINO_INCLUDE) ${ARDUINO_INCLUDE}HardwareSerial1.cpp -o $(ARDUINO_INCLUDE)HardwareSerial1.o
	$(ARDUINO_CXX) $(ARDUINO_CXX_FLAGS) -I$(ARDUINO_INCLUDE) ${ARDUINO_INCLUDE}HardwareSerial2.cpp -o $(ARDUINO_INCLUDE)HardwareSerial2.o
	$(ARDUINO_CXX) $(ARDUINO_CXX_FLAGS) -I$(ARDUINO_INCLUDE) ${ARDUINO_INCLUDE}HardwareSerial3.cpp -o $(ARDUINO_INCLUDE)HardwareSerial3.o
	$(ARDUINO_CXX) $(ARDUINO_CXX_FLAGS) -I$(ARDUINO_INCLUDE) ${ARDUINO_INCLUDE}IPAddress.cpp -o $(ARDUINO_INCLUDE)IPAddress.o
	$(ARDUINO_CXX) $(ARDUINO_CXX_FLAGS) -I$(ARDUINO_INCLUDE) ${ARDUINO_INCLUDE}main.cpp -o $(ARDUINO_INCLUDE)main.o
	$(ARDUINO_CXX) $(ARDUINO_CXX_FLAGS) -I$(ARDUINO_INCLUDE) ${ARDUINO_INCLUDE}new.cpp -o $(ARDUINO_INCLUDE)new.o
	$(ARDUINO_CXX) $(ARDUINO_CXX_FLAGS) -I$(ARDUINO_INCLUDE) ${ARDUINO_INCLUDE}PluggableUSB.cpp -o $(ARDUINO_INCLUDE)PluggableUSB.o
	$(ARDUINO_CXX) $(ARDUINO_CXX_FLAGS) -I$(ARDUINO_INCLUDE) ${ARDUINO_INCLUDE}Print.cpp -o $(ARDUINO_INCLUDE)Print.o
	$(ARDUINO_CXX) $(ARDUINO_CXX_FLAGS) -I$(ARDUINO_INCLUDE) ${ARDUINO_INCLUDE}Stream.cpp -o $(ARDUINO_INCLUDE)Stream.o
	$(ARDUINO_CXX) $(ARDUINO_CXX_FLAGS) -I$(ARDUINO_INCLUDE) ${ARDUINO_INCLUDE}Tone.cpp -o $(ARDUINO_INCLUDE)Tone.o
	$(ARDUINO_CXX) $(ARDUINO_CXX_FLAGS) -I$(ARDUINO_INCLUDE) ${ARDUINO_INCLUDE}USBCore.cpp -o $(ARDUINO_INCLUDE)USBCore.o
	$(ARDUINO_CXX) $(ARDUINO_CXX_FLAGS) -I$(ARDUINO_INCLUDE) ${ARDUINO_INCLUDE}WMath.cpp -o $(ARDUINO_INCLUDE)WMath.o
	$(ARDUINO_CXX) $(ARDUINO_CXX_FLAGS) -I$(ARDUINO_INCLUDE) ${ARDUINO_INCLUDE}WString.cpp -o $(ARDUINO_INCLUDE)WString.o
	$(ARDUINO_CC) $(ARDUINO_CC_FLAGS) -I$(ARDUINO_INCLUDE) ${ARDUINO_INCLUDE}hooks.c -o $(ARDUINO_INCLUDE)hooks.o
	$(ARDUINO_CC) $(ARDUINO_CC_FLAGS) -I$(ARDUINO_INCLUDE) ${ARDUINO_INCLUDE}WInterrupts.c -o $(ARDUINO_INCLUDE)WInterrupts.o
	$(ARDUINO_CC) $(ARDUINO_CC_FLAGS) -I$(ARDUINO_INCLUDE) ${ARDUINO_INCLUDE}wiring.c -o $(ARDUINO_INCLUDE)wiring.o
	$(ARDUINO_CC) $(ARDUINO_CC_FLAGS) -I$(ARDUINO_INCLUDE) ${ARDUINO_INCLUDE}wiring_analog.c -o $(ARDUINO_INCLUDE)wiring_analog.o
	$(ARDUINO_CC) $(ARDUINO_CC_FLAGS) -I$(ARDUINO_INCLUDE) ${ARDUINO_INCLUDE}wiring_digital.c -o $(ARDUINO_INCLUDE)wiring_digital.o
	$(ARDUINO_CC) $(ARDUINO_CC_FLAGS) -I$(ARDUINO_INCLUDE) ${ARDUINO_INCLUDE}wiring_pulse.c -o $(ARDUINO_INCLUDE)wiring_pulse.o
	$(ARDUINO_CC) $(ARDUINO_CC_FLAGS) -I$(ARDUINO_INCLUDE) ${ARDUINO_INCLUDE}wiring_shift.c -o $(ARDUINO_INCLUDE)wiring_shift.o
	$(AR) rcs core.a WInterrupts.c.o
	$(AR) rcs core.a wiring.c.o
	$(AR) rcs core.a wiring_analog.c.o
	$(AR) rcs core.a wiring_digital.c.o
	$(AR) rcs core.a wiring_pulse.c.o
	$(AR) rcs core.a wiring_shift.c.o
	$(AR) rcs core.a CDC.cpp.o
	$(AR) rcs core.a HardwareSerial.cpp.o
	$(AR) rcs core.a HID.cpp.o
	$(AR) rcs core.a IPAddress.cpp.o
	$(AR) rcs core.a main.cpp.o
	$(AR) rcs core.a new.cpp.o
	$(AR) rcs core.a Print.cpp.o
	$(AR) rcs core.a Stream.cpp.o
	$(AR) rcs core.a Tone.cpp.o
	$(AR) rcs core.a USBCore.cpp.o
	$(AR) rcs core.a WMath.cpp.o
	$(AR) rcs core.a WString.cpp.o
	$(ARDUINO_CC) -Os -Wl,--gc-sections -mmcu=$(MCU) -o $(ARD1_SOURCES).elf $(ARD1_SOURCES).o core.a -lm
	$(OBJ_COPY) -O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0 $(ARD1_SOURCES).elf $(ARD1_SOURCES).eep
	$(OBJ_COPY) -O ihex -R .eeprom $(ARD1_SOURCES).elf $(ARD1_OUT).hex