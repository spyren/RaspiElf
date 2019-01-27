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


 
int main(int argc, char *argv[]) {
    int i;
    int opt;
    uint8_t data[END_ADR+1+4];
    uint32_t start_adr = START_ADR;
    uint32_t end_adr = END_ADR;
    FILE *fp;
    int eeprom_fd;
    
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
    
    // no pages required for EEPROM read operation
    
    // prepare for read
    data[0] = READ_CMD;
    
    // 24 bit address
    data[1] = start_adr >> 16;
    data[2] = start_adr & 0x00FFFF >> 8;
    data[3] = start_adr & 0x0000FF;
    
    // read the eeprom
    wiringPiSPIDataRW (CHANNEL, &data[0], end_adr - start_adr + 4) ;     
    
    // write data to the file
    for(i = start_adr + 4; i <= end_adr + 4; i++) {
        fputc(data[i], fp);
    }

    fprintf(stderr, "0x%05x bytes read\n", end_adr - start_adr);
    
    fclose(fp);
    
    exit (0);

}


