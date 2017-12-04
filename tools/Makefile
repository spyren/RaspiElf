elf2bin: elf2bin.o raspi_gpio.o
	cc -o elf2bin -lwiringPi elf2bin.o raspi_gpio.o

bin2elf: bin2elf.o raspi_gpio.o
	cc -o bin2elf -lwiringPi bin2elf.o raspi_gpio.o

bin2elf.o: bin2elf.c
	cc -c bin2elf.c

elf2bin.o: elf2bin.c
	cc -c elf2bin.c

raspi_gpio.o: raspi_gpio.c
	cc -c raspi_gpio.c
	
install: elf2bin bin2elf
	install -m 557 elf2bin bin2elf /usr/local/bin
