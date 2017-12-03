/**
 *  @brief
 *      Copies the Elf memory to a binary file on the Raspberry Pi. 
 * 
 *      The Raspberry Pi GPIO is used as interface to the Cosmac Elf SBC 
 *      (e.g. Elf Membership Card parallel port).
 * 
 *      http://spyr.ch/twiki/bin/view/Cosmac/RaspiElf
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
    uint16_t start_adr = START_ADR;
    uint16_t end_adr = END_ADR;
    
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
                fprintf(stderr, "Usage: %s [-s <adr>] [-e <adr>]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }
    
    if (init_ports() != 0) {
        // can't init ports
        exit(EXIT_FAILURE);
    }
    
    for(i = start_adr; i < end_adr + 1; i++) {
        // in clock
        digitalWrite(IN_N, 0);
        digitalWrite(IN_N, 1);
        
        data = read_byte();
        putchar(data);
    }
    
    // read
    //digitalWrite(WE_N, 1);
    
    // run  
    digitalWrite(WAIT_N, 1);
    digitalWrite(CLEAR_N, 1);
    
}


