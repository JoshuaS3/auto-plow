.PHONY: all

all:
	~/esp/esp-idf/tools/idf.py -p /dev/ttyUSB0 build flash monitor
