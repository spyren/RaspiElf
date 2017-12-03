elf2bin: elf2bin.o raspi_gpio.o
	cc -o elf2bin -lwiringPi elf2bin.o raspi_gpio.o

main.o: main.c
	cc -c elf2bin.c

raspi_gpio.o: raspi_gpio.c
	cc -c raspi_gpio.c
