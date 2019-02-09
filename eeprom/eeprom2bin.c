/**
 *  @brief
 *      eeprom2bin - Copies the EEPROM memory to a binary file on the Raspberry Pi. 
 * 
 *      Copies the EEPROM memory to a binary file (or stdout) on the 
 *      Raspberry Pi. The Raspberry Pi GPIO SPI0.1 is used as interface 
 *      to the SPI EEPROM (e.g. 25LC1024 has 24 bit address and 256 byte page). 
 *      The generated data is written to the standard output stream or to a file. 
 *      Caution: Overwrite file if it exists. 
 *      Use > for redirecting (save the file) or | for piping to another 
 *      command (e.g. hexdump). 
 * 
 *      http://spyr.ch/twiki/bin/view/Cosmac/MassStorage
 *
 *      synopsis
 *       $ eeprom2bin [-s hexadr] [-e hexadr] [file] 
 *      -s start address in hex (0 is default) 
 *      -e end adress in hex (0x1FFFF is default) 
 *      -p <number>  page size in bytes (256 is default) 
 *      -a <number>  address bits (8, 16, or 24; 24 is default) 
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

// global variables
static FILE *fp;

uint16_t page_size = PAGE_SIZE;   
uint8_t address_bits = ADDRESS_BITS;


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
    uint16_t size = 0;
    
    // parse command line options
    while ((opt = getopt(argc, argv, "s:e:p:a:k:")) != -1) {
        switch (opt) {
            case 's': 
                start_adr = strtol(optarg, NULL, 16);
                break; 
            case 'e': 
                end_adr = strtol(optarg, NULL, 16);
                break;
            case 'p': 
                page_size = strtol(optarg, NULL, 16);
                break;
            case 'a': 
                address_bits = strtol(optarg, NULL, 16);
                break;
            case 'k': 
                size = strtol(optarg, NULL, 16);
                break;
            default:
                fprintf(stderr, 
                  "Usage: %s [-s <adr>] [-e <adr>] [-p <page_size>] [-a <address_bits>] [-k <size>] [<filename>]\n", 
                  argv[0]);
                exit(EXIT_FAILURE);
        }
    }
    
    switch (size) {
    case 0:
      size = 1024;
      break;
    case 1:
      address_bits = 8;
      page_size = 16;
      break;
    case 2:
      address_bits = 8;
      page_size = 16;
      break;
    case 4:
      address_bits = 8;
      page_size = 16;
      break;
    case 8:
      address_bits = 8;
      page_size = 16;
      break;
    case 16:
      address_bits = 8;
      page_size = 16;
      break;
    case 32:
      address_bits = 8;
      page_size = 16;
      break;
    case 64:
      address_bits = 8;
      page_size = 16;
      break;
    case 128:
      address_bits = 8;
      page_size = 16;
      break;
    case 256:
      address_bits = 8;
      page_size = 16;
      break;
    case 512:
      address_bits = 8;
      page_size = 16;
      break;
    case 1024:
      address_bits = 8;
      page_size = 16;
      break;
    case 2048:
      address_bits = 8;
      page_size = 16;
      break;
    default:
      // invalid size
       fprintf(stderr, "Invalid size. Known sizes: 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048 Kibit\n");
       exit(EXIT_FAILURE);      
    }

    if (end_adr == 0) {
      end_adr = size * 1024 / 8 - 1;
    }

    if (!(address_bits == 8 | address_bits == 16 | address_bits == 24)) { 
       // invalid number of address bits 
       fprintf(stderr, "Invalid number of address bits, choose 8, 16, or 24.\n");
       exit(EXIT_FAILURE);
    }
      
    if (!(page_size == 16 | page_size == 32 | page_size == 64 | page_size == 256)) { 
       // invalid page size 
       fprintf(stderr, "Invalid page size, valid page sizes: 16, 32,64, or 256.\n");
       exit(EXIT_FAILURE);
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
    start_remainder = page_size - (start_adr % page_size);
    if (start_remainder == page_size) {
        start_remainder = 0;
    }
    end_remainder = (end_adr+1) % page_size;
    
    if (start_remainder != 0) {
        // read start remainder page
        read_bytes = read_page(start_adr, start_remainder);
    }

    pages = ((end_adr+1) - start_adr - start_remainder - end_remainder) / page_size;
    for (page = 0; page < pages; page++) {
        // read whole pages
        read_bytes += read_page(start_adr + start_remainder + page * page_size, page_size);
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
    uint8_t data[page_size+4];
    uint8_t address_bytes;

    // prepare for read
    data[0] = READ_CMD;
    
     if (address_bits == 24) {
        // 24 bit address
        data[1] = start >> 16;
        data[2] = start >> 8 & 0x0000FF;
        data[3] = start & 0x0000FF;
        address_bytes = 3;    
    } else if (address_bits == 16) {
        data[1] = start >> 8 & 0x0000FF;
        data[2] = start & 0x0000FF;    
        address_bytes = 2;    
    } else {
        // 8 bit
        data[1] = start & 0x0000FF;    
        address_bytes = 1;    
    }

    
    // read the eeprom
    wiringPiSPIDataRW (CHANNEL, &data[0],  1 + address_bytes + count) ;     
    
    // write data to the file
    for(i = 4; i < count + 4; i++) {
        fputc(data[i], fp);
    }
    
    return count;
}

