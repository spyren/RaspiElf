/**
 *  @brief
 *      Copies the Elf memory to a binary file (or stdout) on the Raspberry Pi. 
 * 
 *      The Raspberry Pi GPIO is used as interface to the Cosmac Elf SBC 
 *      (e.g. Elf Membership Card parallel port).
 * 
 *      http://spyr.ch/twiki/bin/view/Cosmac/RaspiElf
 *
 *   	synopsis
 *		 $ elf2bin [-s <hexadr>] [-e <hexadr>] [-w] [-r] [<filename>]
 * 		The generated data is written to the standard output stream or to
 * 		<filename>. Caution: Overwrite file if it exists.  
 * 		Use  > for redirecting (save the file) or | for piping to 
 * 		another command (e.g. hexdump)
 * 		-s start address in hex
 * 		-e end adress in hex
 * 		-w read enable
 * 		-r run mode
 *  
 *  @file 
 *      elf2bin.c
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
    char data;
    uint8_t run_mode = FALSE;
    uint8_t write_mode = FALSE;
    uint16_t start_adr = START_ADR;
    uint16_t end_adr = END_ADR;
    FILE *fp;
    
    // parse command line options
    while ((opt = getopt(argc, argv, "s:e:wr")) != -1) {
        switch (opt) {
            case 's': 
                start_adr = strtol(optarg, NULL, 16);
                break; 
            case 'e': 
                end_adr = strtol(optarg, NULL, 16);
                break;
            case 'w':
				write_mode = TRUE;
				break;
            case 'r':
				run_mode = TRUE;
				break;
            default:
                fprintf(stderr, 
                  "Usage: %s [-s <adr>] [-e <adr>] [-w] [-r] [<filename>]\n", 
                  argv[0]);
                exit(EXIT_FAILURE);
        }
    }
    
    fp = stdout;
    if (optind < argc) {
		// there is a filename parameter, use it instead of stdout
		fp = fopen(argv[optind], "w");
		if (fp == NULL) {
			fprintf(stderr, 
			  "Cannot open file \"%s\"\n", 
			  argv[optind]);
			exit(EXIT_FAILURE);
		}

	}
	
    if (init_port_mode() != 0) {
        // can't init ports
        exit(EXIT_FAILURE);
    }

    if (init_port_level() != 0) {
        // can't init ports
        exit(EXIT_FAILURE);
    }
    
    // read
    digitalWrite(WRITE_N, 1);
    int j = 0;
    for(i = 0; i <= end_adr; i++) {
        // in clock
        digitalWrite(IN_N, 0);
        usleep(100);
        digitalWrite(IN_N, 1);
        usleep(100);        
        if (i >= start_adr) {
            data = read_byte();
            fputc(data, fp);
            j++;
        }
    }
    
    if (write_mode) {
		// write enable
		digitalWrite(WRITE_N, 0);
	} else {
		// read 
		digitalWrite(WRITE_N, 1);
	}
	
	if (run_mode) {
		// run  
		digitalWrite(WAIT_N, 1);
		digitalWrite(CLEAR_N, 1);    
	}
    
	fprintf(stderr, "0x%04x bytes read\n", j);
	
	fclose(fp);
	
	exit (0);

}


