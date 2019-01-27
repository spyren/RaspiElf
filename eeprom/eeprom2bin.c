/**
 *  @brief
 *      eeprom2bin - Copies the EEPROM memory to a binary file on the Raspberry Pi. 
 * 
 *      Copies the EEPROM memory to a binary file (or stdout) on the 
 *      Raspberry Pi. The Raspberry Pi GPIO SPI0.1 is used as interface 
 *      to the SPI EEPROM (24 bit address, at least a 1024 Kibit type, 
 *      256 byte page). The generated data is written to the standard 
 *      output stream or to a file. Caution: Overwrite file if it exists. 
 *      Use > for redirecting (save the file) or | for piping to another 
 *      command (e.g. hexdump). 
 * 
 *      http://spyr.ch/twiki/bin/view/Cosmac/MassStorage
 *
 *      synopsis
 *       $ eeprom2bin [-s hexadr] [-e hexadr] [file] 
 *      -s start address in hex (0 is default) 
 *      -e end adress in hex (0x1FFFF is default) 
 *  
 *  @file 
 *      eeprom2bin.c
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

// function prototypes
int read_page(uint32_t start, uint16_t count);

static FILE *fp;


int main(int argc, char *argv[]) {
    int opt;
    uint32_t start_adr = START_ADR;
    uint32_t end_adr = END_ADR;
    int eeprom_fd;
    uint32_t read_bytes = 0;
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
    
    eeprom_fd = wiringPiSPISetup (CHANNEL, SPEED);
    if (eeprom_fd < 0) {
        fprintf(stderr, "Cannot open SPI channel\n"); 
        exit(EXIT_FAILURE);
    }
    
    // pages required for EEPROM read operation
    start_remainder = PAGE_SIZE - (start_adr % PAGE_SIZE);
    if (start_remainder == PAGE_SIZE) {
        start_remainder = 0;
    }
    end_remainder = (end_adr+1) % PAGE_SIZE;
    
    if (start_remainder != 0) {
        // read start remainder page
        read_bytes = read_page(start_adr, start_remainder);
    }

    pages = ((end_adr+1) - start_adr - start_remainder - end_remainder) / PAGE_SIZE;
    for (page = 0; page < pages; page++) {
        // read whole pages
        read_bytes += read_page(start_adr + start_remainder + page * PAGE_SIZE, PAGE_SIZE);
    }

    if (end_remainder != 0) {
        // read end remainder page
        read_bytes +=read_page(end_adr - end_remainder + 1, end_remainder);
    }
    

    fprintf(stderr, "0x%05x bytes read\n", read_bytes);
    
    fclose(fp);
    
    exit (0);

}

int read_page(uint32_t start, uint16_t count) {
    uint16_t i;
    uint8_t data[PAGE_SIZE+4];

    // prepare for read
    data[0] = READ_CMD;
    
    // 24 bit address
    data[1] = start >> 16;
    data[2] = start >> 8 & 0x0000FF;
    data[3] = start & 0x0000FF;
    
    // read the eeprom
    wiringPiSPIDataRW (CHANNEL, &data[0], count + 4) ;     
    
    // write data to the file
    for(i = 4; i < count + 4; i++) {
        fputc(data[i], fp);
    }
    
    return count;
}

