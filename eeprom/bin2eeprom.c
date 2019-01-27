/**
 *  @brief
 *      bin2eeprom - Copies the content of binary file on the Raspberry Pi to EEPROM. 
 * 
 *      Copies the content of binary file on the Raspberry Pi to EEPROM 
 *      memory. The Raspberry Pi GPIO SPI0.1 is used as interface to the 
 *      SPI EEPROM (24 bit address, at least a 1024 Kibit type, 256 byte 
 *      page). Use < for redirecting or | for piping from another command. 
 *      (e.g. Elf Membership Card parallel port).
 * 
 *      http://spyr.ch/twiki/bin/view/Cosmac/MassStorage
 *
 *      synopsis
 *       $ bbin2eeprom [-s hexadr] [-e hexadr] [file] 
 *      The file is read from stdin in or <filename>.
 *      -s start address in hex (0 is default) 
 *      -e end adress in hex (0x1FFFF is default) 
 *  @file
 *      bin2eeprom.c
 *  @author
 *      Peter Schmid, peter@spyr.ch
 *  @date
 *      2019-01-26
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
#include <wiringPiSPI.h>
#include "eeprom.h"

#define START_ADR (0x00000)
#define END_ADR   (0x1FFFF)

static FILE *fp;
static uint8_t end_of_file = FALSE;

// function prototypes
int write_page(uint32_t start, uint16_t count);

int main(int argc, char *argv[]) {
    int opt;
    uint32_t start_adr = START_ADR;
    uint32_t end_adr = END_ADR;
    int eeprom_fd;
    uint32_t written_bytes = 0;
    uint32_t start_remainder;
    uint32_t end_remainder;
    uint32_t pages;
    uint32_t page;
  
    
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

    eeprom_fd = wiringPiSPISetup (CHANNEL, SPEED);
    if (eeprom_fd < 0) {
        fprintf(stderr, "Cannot open SPI channel.\n"); 
        exit(EXIT_FAILURE);
    }
    
    // pages required for EEPROM write operation
    start_remainder = PAGE_SIZE - (start_adr % PAGE_SIZE);
    if (start_remainder == PAGE_SIZE) {
        start_remainder = 0;
    }
    end_remainder = (end_adr+1) % PAGE_SIZE;
    
    if (start_remainder != 0) {
        // write start remainder page
        written_bytes = write_page(start_adr, start_remainder);
    }

    pages = ((end_adr+1) - start_adr - start_remainder - end_remainder) / PAGE_SIZE;
    for (page = 0; page < pages; page++) {
        // write whole pages
        if (end_of_file) {
            break;
        }
        written_bytes += write_page(start_adr + start_remainder + page * PAGE_SIZE, PAGE_SIZE);
    }

    if (end_remainder != 0 && !end_of_file) {
        // write end remainder page
        written_bytes += write_page(end_adr - end_remainder + 1, end_remainder);
    }
            
    
    fprintf(stderr, "0x%05x bytes written\n", written_bytes);
    
    fclose(fp);
}

int write_page(uint32_t start, uint16_t count) {
    uint16_t i;
    uint8_t data[PAGE_SIZE+4];
    
    // write enable
    data[0] = WREN_CMD;
    wiringPiSPIDataRW(CHANNEL, &data[0], 1) ;      
    
    // prepare for write
    data[0] = WRITE_CMD;
        
    // 24 bit address
    data[1] = start >> 16;
    data[2] = start >> 8 & 0x0000FF;
    data[3] = start & 0x0000FF;
    
    // read data
    for (i = 0; i < count; i++) {
        data[i+4] = fgetc(fp);
		if( feof(fp) ) {
            // reached EOF
            end_of_file = TRUE;
            break;
        }
    }

    if (i == 0) {
        return 0;
    }
    
    // write page
    wiringPiSPIDataRW(CHANNEL, &data[0], i + 4) ;     
    
    // wait til operation finished
    do {
        data[0] = RDSR_CMD; 
        wiringPiSPIDataRW(CHANNEL, &data[0], 2);
    } while ((data[1] & WRITE_IN_PROCESS) == WRITE_IN_PROCESS) ;  
    
    return i;   
    
}
