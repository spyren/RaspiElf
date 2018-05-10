/**
 *  @brief
 *      Gets the data switches and gets LED data of an 
 *      Elf (Membership Card) and shows this on micro dot pHAT display.
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
#include <termios.h>
#include <time.h>
#include <string.h>
#include <wiringPi.h>
#include "raspi_gpio.h"
#include "microdot_phat_hex.h"

typedef enum {LOAD_READ, LOAD_WRITE, RUN, WAIT} elf_mode_t;

void usage_exit(int err_number, const char *str);
int getkey();


int main(int argc, char *argv[]) {
    int opt;
    uint8_t verbose_mode = FALSE;
    uint16_t adr = 0;
    elf_mode_t elf_mode;
    uint8_t write_protect;
    int key;
    uint8_t hi_nibble = TRUE;
    uint8_t nibble;
    uint8_t data = 0;
    
    // parse command line options
    while ((opt = getopt(argc, argv, "v")) != -1) {
        switch (opt) {
            case 'v':
                verbose_mode = TRUE;
                break;
            default:
                usage_exit(EXIT_FAILURE, argv[0]);
                break;
        }
    }

    // parse command
    if (argc - optind > 0) {
        // too many parameters
        usage_exit(EXIT_FAILURE, argv[0]);
    }

    if (init_port_mode() != 0) {
        // !! can't init ports
        fprintf(stderr, "can't init ports\n");
        exit(EXIT_FAILURE);
    }

    if (init_port_level() != 0) {
        // can't init ports
        fprintf(stderr, "can't init port levels, check switches\n");
        exit(EXIT_FAILURE);
    }

    // load mode
    digitalWrite(WAIT_N, 0);
    digitalWrite(CLEAR_N, 0);
    elf_mode = LOAD_READ;
    usleep(100);
    // reset
    digitalWrite(WAIT_N, 1);
    usleep(100);
    digitalWrite(WAIT_N, 0);
    usleep(100);
    
    // read
    digitalWrite(WRITE_N, 1);
    write_protect = TRUE;

    // get first byte
    digitalWrite(IN_N, 0);
    usleep(100);
    digitalWrite(IN_N, 1); 
  
    usleep(1000);

    write_hex_digits(0x01, TRUE, FALSE, 0);
    write_hex_digits(0xAB, FALSE, FALSE, 1);
    write_hex_digits(0xEF, FALSE, FALSE, 2);
     
    
    while(key != 'q') {
        usleep(1000);
        key = toupper(getkey());

        // write data
        if ((key >= '0' && key <= '9') | (key >= 'A' && key <= 'F')) {
            if (key <= '9') {
                nibble = (key - '0');
            } else {
                nibble = (key - 'A') + 10;
            }
            if (hi_nibble) {
                hi_nibble = FALSE;
                data &= 0x0F;
                data |= nibble << 4;
            } else {
                hi_nibble = TRUE;
                data &= 0xF0;
                data |= nibble;
            }
            write_byte(data);
        }
        
        switch (elf_mode) {
        case LOAD_READ:
        switch (key) {
            case EOF:
                write_hex_digits(read_byte(), TRUE, write_protect, 0);
                write_hex_digits(adr & 0x00FF, FALSE, FALSE, 1);
                write_hex_digits(adr >> 8, TRUE, FALSE, 2);             
                break;
            case '+':
            case 'I':
                // INPUT
                adr++;
                usleep(100);
                digitalWrite(IN_N, 0);
                usleep(100);
                digitalWrite(IN_N, 1); 
                break; 
            case '.':
            case 'W':
                elf_mode = LOAD_WRITE;
                write_protect = FALSE;
                digitalWrite(WRITE_N, 0);
                break;
            case 'G':
                elf_mode = RUN;
                digitalWrite(WAIT_N, 1);
                digitalWrite(CLEAR_N, 1);
                break;
            case 'L':
                // reset
                digitalWrite(WAIT_N, 1);
                usleep(100);
                digitalWrite(WAIT_N, 0);
                usleep(100);
                // get first byte
                digitalWrite(IN_N, 0);
                usleep(100);
                digitalWrite(IN_N, 1); 
                adr = 0;
                break;
            }
        
            break;
        case LOAD_WRITE:
        switch (key) {
            case EOF:
                write_hex_digits(read_switches(), TRUE, write_protect, 0);
                write_hex_digits(adr & 0x00FF, FALSE, FALSE, 1);
                write_hex_digits(adr >> 8, TRUE, FALSE, 2);             
                break;
            case '+':
            case 'I':
                // INPUT
                adr++;
                usleep(100);
                digitalWrite(IN_N, 0);
                usleep(100);
                digitalWrite(IN_N, 1); 
                break; 
            case '.':
            case 'W':
                elf_mode = LOAD_READ;
                write_protect = TRUE;
                digitalWrite(WRITE_N, 1);
                break;
            case 'G':
                elf_mode = RUN;
                digitalWrite(WAIT_N, 1);
                digitalWrite(CLEAR_N, 1);
                break;
            case 'L':
                // reset
                digitalWrite(WAIT_N, 1);
                usleep(100);
                digitalWrite(WAIT_N, 0);
                usleep(100);
                adr = 0;
                break;
            }
            break;
        case RUN:
        switch (key) {
            case EOF:
                write_hex_digits(read_switches(), TRUE, write_protect, 0);
                write_hex_digits(read_byte(), FALSE, FALSE, 1);
                write_hex_digits(0, FALSE, TRUE, 2);
                break;
            case '.':
            case 'W':
                if (write_protect) {
                    write_protect = FALSE;
                    digitalWrite(WRITE_N, 0);
                } else {
                    write_protect = TRUE;
                    digitalWrite(WRITE_N, 1);
                }
                break;
            case 'G':
                digitalWrite(WAIT_N, 0);
                elf_mode = WAIT;
                break;
            case 'L':
                // load mode
                digitalWrite(WAIT_N, 0);
                digitalWrite(CLEAR_N, 0);
                elf_mode = LOAD_WRITE;
                usleep(100);
                // reset
                digitalWrite(WAIT_N, 1);
                usleep(100);
                digitalWrite(WAIT_N, 0);
                usleep(100);
                adr = 0;
                if (write_protect) {
                    elf_mode = LOAD_READ;
                    // get first byte
                    digitalWrite(IN_N, 0);
                    usleep(100);
                    digitalWrite(IN_N, 1); 
                } else {
                    elf_mode = LOAD_WRITE;
                }
                break;
            }
            break;
        case WAIT:
        switch (key) {
            case EOF:
                write_hex_digits(read_switches(), TRUE, write_protect, 0);
                write_hex_digits(read_byte(), TRUE, FALSE, 1);
                write_hex_digits(0, FALSE, FALSE, 2);
                break;
            case '.':
            case 'W':
                if (write_protect) {
                    write_protect = FALSE;
                    digitalWrite(WRITE_N, 0);
                } else {
                    write_protect = TRUE;
                    digitalWrite(WRITE_N, 1);
                }
                break;
            case 'G':
                digitalWrite(WAIT_N, 1);
                elf_mode = RUN;
                break;
            case 'L':
                // load mode
                digitalWrite(WAIT_N, 0);
                digitalWrite(CLEAR_N, 0);
                usleep(100);
                // reset
                digitalWrite(WAIT_N, 1);
                usleep(100);
                digitalWrite(WAIT_N, 0);
                usleep(100);
                adr = 0;
                if (write_protect) {
                    elf_mode = LOAD_READ;
                    // get first byte
                    digitalWrite(IN_N, 0);
                    usleep(100);
                    digitalWrite(IN_N, 1); 
                } else {
                    elf_mode = LOAD_WRITE;
                }
                break;
            }
            break;
        }
        
        
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


int getkey() {
    int character;
    struct termios orig_term_attr;
    struct termios new_term_attr;

    /* set the terminal to raw mode */
    tcgetattr(fileno(stdin), &orig_term_attr);
    memcpy(&new_term_attr, &orig_term_attr, sizeof(struct termios));
    new_term_attr.c_lflag &= ~(ECHO|ICANON);
    new_term_attr.c_cc[VTIME] = 0;
    new_term_attr.c_cc[VMIN] = 0;
    tcsetattr(fileno(stdin), TCSANOW, &new_term_attr);

    /* read a character from the stdin stream without blocking */
    /*   returns EOF (-1) if no character is available */
    character = fgetc(stdin);

    /* restore the original terminal attributes */
    tcsetattr(fileno(stdin), TCSANOW, &orig_term_attr);

    return character;
}
