# @brief
#	Makefile for the RaspiEEPROM tools
# 
# @file
#	Makefile
# @author
#	Peter Schmid peter@spyr.ch
# @date
# 	2019-01-25
all: eeprom2bin bin2eeprom bootloader.bin bootloader-db25.bin

bootloader.bin: bootloader.hex
	hex2bin bootloader.hex

bootloader-db25.bin: bootloader-db25.hex
	hex2bin bootloader-db25.hex

bootloader.hex: bootloader.asm
	a18 bootloader.asm -Lb1 bootloader.lst -o bootloader.hex 

bootloader-db25.hex: bootloader-db25.asm
	a18 bootloader-db25.asm -Lb1 bootloader-db25.lst -o bootloader-db25.hex 

eeprom2bin: eeprom2bin.o 
	cc -g -o eeprom2bin -lwiringPi eeprom2bin.o

bin2eeprom: bin2eeprom.o 
	cc -g -o bin2eeprom -lwiringPi bin2eeprom.o

eeprom2bin.o: eeprom2bin.c
	cc -g -c eeprom2bin.c

bin2eeprom.o: bin2eeprom.c
	cc -g -c bin2eeprom.c

install: eeprom2bin bin2eeprom 
	install -m 557 eeprom2bin bin2eeprom /usr/local/bin

docs:
	doxygen ./Doxyfile

