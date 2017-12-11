# @brief
#	Makefile for the RaspiElf tools
# 
# @file
#	Makefile
# @author
#	Peter Schmid peter@spyr.ch
# @date
# 	2017-12-09
all: elf2bin bin2elf elf

elf: elf.o raspi_gpio.o
	cc -g -o elf -lwiringPi elf.o raspi_gpio.o
 
elf2bin: elf2bin.o raspi_gpio.o
	cc -g -o elf2bin -lwiringPi elf2bin.o raspi_gpio.o

bin2elf: bin2elf.o raspi_gpio.o
	cc -g -o bin2elf -lwiringPi bin2elf.o raspi_gpio.o

elf.o: elf.c
	cc -g -c elf.c

bin2elf.o: bin2elf.c
	cc -g -c bin2elf.c

elf2bin.o: elf2bin.c
	cc -g -c elf2bin.c

raspi_gpio.o: raspi_gpio.c raspi_gpio.h
	cc -g -c raspi_gpio.c
	
install: elf2bin bin2elf elf
	install -m 557 elf2bin bin2elf elf /usr/local/bin

docs:
	doxygen ./Doxyfile
	
