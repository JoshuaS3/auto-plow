CXX = gcc
CXXFLAGS = --std=c11 -Wall -Wextra -pedantic -O -g
FILES = src/rpi/main.c
OUT = autoplow

.PHONY: all

all:
	$(CXX) $(CXXFLAGS) -o $(OUT) $(FILES)
	stty -F /dev/ttyACM0 speed 9600 cs8 -cstopb -parenb raw