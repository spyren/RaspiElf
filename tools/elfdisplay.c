/**
 *  @brief
 *      Controls the mode, sets data switches and gets LED data of an 
 * 		Elf (Membership Card).
 * 
 *  @file
 *      elfdisplay.c
 *  @author
 *      Peter Schmid, peter@spyr.ch
 *  @date
 *      2018-05-10
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
#include "microdot_phat_hex.h"



void usage_exit(int err_number, const char *str);


int main(int argc, char *argv[]) {
    int i;
    int opt;
    int sw, led;
    uint8_t increment_mode = FALSE;
    uint8_t verbose_mode = FALSE;
	uint8_t start_mode = FALSE;
	uint8_t inverted_mode = FALSE;
    uint16_t start_adr = START_ADR;
    uint8_t switch_value;
    
    // parse command line options
    while ((opt = getopt(argc, argv, "s:inv")) != -1) {
        switch (opt) {
            case 's':
				start_mode = TRUE;
                start_adr = strtol(optarg, NULL, 16);
                break; 
            case 'i': 
                increment_mode = TRUE;
                break;
            case 'n':
                inverted_mode = TRUE;
                break;
            case 'v':
				verbose_mode = TRUE;
				break;
            default:
				usage_exit(EXIT_FAILURE, argv[0]);
				break;
        }
    }

	// parse command
	if (argc - optind <= 1) {
		// 1 or no parameter left -> command
	} else if (argc - optind == 2) {
		// 2 parameters left -> put command and <switch>
		if (strcmp(argv[optind], "put") == 0) {
			switch_value = strtol(argv[argc-1], NULL, 16);
		} else {
			usage_exit(EXIT_FAILURE, argv[0]);
		}
	} else if (argc - optind > 2) {
		// too many parameters
		usage_exit(EXIT_FAILURE, argv[0]);
	}

    if (init_port_mode() != 0) {
        // !! can't init ports
        exit(EXIT_FAILURE);
    }
    
    usleep(1000);

	write_hex_digits(0x01, TRUE, FALSE, 0);
	write_hex_digits(0xAB, FALSE, FALSE, 1);
	write_hex_digits(0xEF, FALSE, FALSE, 2);
	 
    exit(0);   
}

void usage_exit(int err_number, const char *str) {
	fprintf(stderr, "\
Usage: %s [-i] [-v] [-s <number>] [load|run|wait|reset|read|in|get|put] [<switch>]\n\
-i post increment IN\n\
-v verbose\n\
-n inverted command\n\
-s count to the <number> address (hex)\n\
<switch> data for the switches in hex\n\
LED Q Rx IN WAIT CLEAR WRITE SWITCH\n",
      str);
    exit(err_number);
}
