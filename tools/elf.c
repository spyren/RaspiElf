/**
 *  @brief
 *      Copies a Raspberry Pi binary file (stdin) to the Elf memory. 
 * 
 *      The Raspberry Pi GPIO is used as interface to the Cosmac Elf SBC 
 *      (e.g. Elf Membership Card parallel port).
 * 
 *      http://spyr.ch/twiki/bin/view/Cosmac/RaspiElf
 *
 *   	synopsis
 *		 $ elf [-i] [-v] [-s <number>] [load|run|wait|reset|write|get|put] [<switch>] 
 * 		
 * 		without command parameter get is executed.
 * 
 * 		-s start address in hex
 * 		-i increment
 * 		-n inverted command 
 * 		-v verbose
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

typedef enum {LOAD_CMD, RUN_CMD, WAIT_CMD, RESET_CMD, READ_CMD, 
	IN_CMD, GET_CMD, PUT_CMD} command_t;

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
    command_t cmd;
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
		if (argc - optind  == 1) {
			if (strcmp(argv[optind], "load") == 0) {
				cmd = LOAD_CMD;
			} else if (strcmp(argv[optind], "run") == 0) {
				cmd = RUN_CMD;
			} else if (strcmp(argv[optind], "wait") == 0) {
				cmd = WAIT_CMD;
			} else if ((strcmp(argv[optind], "reset") == 0) || 
						(strcmp(argv[optind], "clear") == 0)) {
				cmd = RESET_CMD;
			} else if (strcmp(argv[optind], "read") == 0 || 
						strcmp(argv[optind], "rd") == 0) {
				cmd = READ_CMD;
			} else if (strcmp(argv[optind], "in") == 0) {
				cmd = IN_CMD;
			} else if (strcmp(argv[optind], "get") == 0) {
				cmd = GET_CMD;
			} else if (strcmp(argv[optind], "put") == 0) {
				// argument for put is missing
				usage_exit(EXIT_FAILURE, argv[0]);
			} else {
				// unknown command
				usage_exit(EXIT_FAILURE, argv[0]);
			}
		} else {
			// no parameter -> get command
			cmd = GET_CMD;
		}
	} else if (argc - optind == 2) {
		// 2 parameters left -> put command and <switch>
		if (strcmp(argv[optind], "put") == 0) {
			switch_value = strtol(argv[argc-1], NULL, 16);
			cmd = PUT_CMD;
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

	if (start_mode) {
		for (i = 0; i < start_adr; i++) {
			// count up to the start address
			digitalWrite(IN_N, 0);
			digitalWrite(IN_N, 1);        
		}    
	}
	
	switch (cmd) {
		case LOAD_CMD:
		    digitalWrite(WAIT_N, 0);
		    digitalWrite(CLEAR_N, 0);
			break;
		case RUN_CMD:
		    digitalWrite(WAIT_N, 1);
		    digitalWrite(CLEAR_N, 1);
			break;
		case WAIT_CMD:
			if (inverted_mode) {
			    digitalWrite(WAIT_N, 1);
			} else {
			    digitalWrite(WAIT_N, 0);
			}		
			break;
		case RESET_CMD:
			if (inverted_mode) {
			    digitalWrite(CLEAR_N, 1);
			} else {
			    digitalWrite(CLEAR_N, 0);
			}
			break;
		case READ_CMD:
			if (inverted_mode) {
			    digitalWrite(READ_N, 1);
			} else {
			    digitalWrite(READ_N, 0);
			}
			break;
		case IN_CMD:
			if (inverted_mode) {
			    digitalWrite(IN_N, 1);
			} else {
			    digitalWrite(IN_N, 0);
			}
			break;
		case GET_CMD:
			// get is later executed
			break;
		case PUT_CMD:
			write_byte(switch_value);
			break;		
	}

	// always get
	if (verbose_mode) {
		printf("LED:%02x Q:%1x Rx:%1x IN:%1x WAIT:%1x CLR:%1x READ:%1x SWITCH:%02x\n", 
				read_byte(), 			// LED (Port Out)
				digitalRead(RX_Q), 		// Q, Tx
				digitalRead(TX_EF3),	// Rx (EF3)
				!digitalRead(IN_N),		// IN (EF4)
				!digitalRead(WAIT_N),	// WAIT
				!digitalRead(CLEAR_N),	// CLEAR
				!digitalRead(READ_N),	// READ
				read_switches() 		// SWITCH (Port In)		
		);
	} else {
		// LED Q Rx IN WAIT CLEAR WRITE SWITCH
		printf("%02x %1x %1x %1x %1x %1x %1x %02x\n", 
				read_byte(), 			// LED (Port Out)
				digitalRead(RX_Q), 		// Q, Tx
				digitalRead(TX_EF3),	// Rx (EF3)
				!digitalRead(IN_N),		// IN (EF4)
				!digitalRead(WAIT_N),	// WAIT
				!digitalRead(CLEAR_N),	// CLEAR
				!digitalRead(READ_N),	// READ
				read_switches() 		// SWITCH (Port In)
		); 
	}
	
    if (increment_mode) {
		// post increment
        digitalWrite(IN_N, 0);
        digitalWrite(IN_N, 1);        		
	}
     
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
