/**
 *  @brief
 *      Copies a Raspberry Pi binary file (stdin) to the Elf memory. 
 * 
 *      The Raspberry Pi GPIO is used as interface to the Cosmac Elf SBC 
 *      (e.g. Elf Membership Card parallel port).
 * 
 *      http://spyr.ch/twiki/bin/view/Cosmac/RaspiElf
 *
 *  @file
 *      bin2elf.c
 *  @author
 *      Peter Schmid, peter@spyr.ch
 *  @date
 *      2017-11-30
 *  @remark
 *      Language: gcc version 4.9.2 on Raspberry Pi 3, Raspbian
 *  @copyright
 *      Peter Schmid, Switzerland
 *
 *      This file is part of "RaspiElf" software.
 *
 *      "RaspiElf" software is free software: you can redistribute it
 *      and/or modify it under the terms of the GNU General Public License as
 *      published by the Free Software Foundation, either version 3 of the
 *      License, or (at your option) any later version.
 *
 *      "RaspiElf" is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License along
 *      with "RaspiElf". If not, see <http://www.gnu.org/licenses/>.
 */


#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wiringPi.h>
#include "raspi_gpio.h"


int main(int argc, char *argv[]) {
    int i;
    int opt;
    int data;
    uint16_t start_adr = START_ADR;
    uint16_t end_adr = END_ADR;
    FILE *fp;
    
    // parse command line options
    while ((opt = getopt(argc, argv, "s:e:")) != -1) {
        switch (opt) {
            case 's': 
                start_adr = strtol(optarg, NULL, 16);
                break; 
            case 'e': 
                end_adr = strtol(optarg, NULL, 16);
                break;
            default:
                fprintf(stderr, 
                  "Usage: %s [-s <adr>] [-e <adr>] [<filename>]\n", 
                  argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    fp = stdin;
    if (optind < argc) {
		// there is a filename parameter, use it instead of stdin
		fp = fopen(argv[optind], "r");
		if (fp == NULL) {
			fprintf(stderr, 
			  "Cannot open file \"%s\"\n", 
			  argv[optind]);
			exit(EXIT_FAILURE);
		}
	}
    
    if (init_ports() != 0) {
        // can't init ports
        exit(EXIT_FAILURE);
    }
    
    for (i = 0; i < start_adr; i++) {
        // count up to the start address
        digitalWrite(IN_N, 0);
        digitalWrite(IN_N, 1);        
    }
    
    // write
    digitalWrite(WE_N, 0);
   
    int j = 0;
    while ((data = fgetc(fp)) != EOF) {
		write_byte(data);
		j++;
        
        // in clock
        digitalWrite(IN_N, 0);
        digitalWrite(IN_N, 1);
   
        if (++i > end_adr) {
            break;
        }
    }
    
    // read
    digitalWrite(WE_N, 1);
    
    // run  
    digitalWrite(WAIT_N, 1);
    digitalWrite(CLEAR_N, 1);
    
    fprintf(stderr, "0x%04x bytes written\n", j);
    
    fclose(fp);
}


