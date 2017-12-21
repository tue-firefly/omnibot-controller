ARDUINO_DIR  = /usr/share/arduino
BOARD_TAG    = uno
ARDUINO_PORT = /dev/ttyACM0

ARD_ID_1=9523834333435180C1C1
ARD_ID_2=9523834333435160F142
ARD_ID_3=95238343334351304222
SERIAL_ID=$(shell udevadm info --name=${ARDUINO_PORT} | grep -oP 'ID_SERIAL_SHORT=\K\w+')

ifeq ($(SERIAL_ID), $(ARD_ID_1))
	PIN_FLAG= -DPINS_1
	ARD_NUM = 1
else ifeq ($(SERIAL_ID), $(ARD_ID_2))
	PIN_FLAG= -DPINS_2
	ARD_NUM = 2
else ifeq ($(SERIAL_ID), $(ARD_ID_3))
	PIN_FLAG= -DPINS_3
	ARD_NUM = 3
endif
CPPFLAGS += ${PIN_FLAG}
#TODO other arduinos

include /usr/share/arduino/Arduino.mk

select:
	@echo ""
	@echo "================="
	@echo "Flashed arduino ${ARD_NUM}"

burn: upload select



