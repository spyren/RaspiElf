/**
 *  @brief
 *      Reads a hex keypad and sets the data and gets LED data of an Elf (Membership Card) and 
 *      shows address and data on micro dot pHAT display.
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
#include <ctype.h>
#include <wiringPi.h>
#include "raspi_gpio.h"
#include "microdot_phat_hex.h"

typedef enum {LOAD_READ, LOAD_WRITE, RUN, WAIT, ADDRESS} elf_mode_t;

int getkey();


int main(int argc, char *argv[]) {
  uint16_t adr = 0;
  uint8_t data = 0;
  elf_mode_t elf_mode;
  uint8_t write_protect;
  int key;
  uint8_t hi_nibble = TRUE;
  uint8_t hi_byte = TRUE;
  uint8_t nibble;
  uint8_t hexin = 0;
  int i;
  
  if (init_port_mode() != 0) {
    // can't init ports
    fprintf(stderr, "can't init ports\n");
    exit(EXIT_FAILURE);
  }

  if (init_port_level() != 0) {
    // can't init port levels
    fprintf(stderr, "can't init port levels, check switches\n");
    exit(EXIT_FAILURE);
  }

  // clear display
  if (write_hex_digits(0xCD, TRUE, 0, 0) < 0) {
    // can't open I2C
    fprintf(stderr, "can't open I2C\n");
    exit(EXIT_FAILURE);
  }
  if (write_hex_digits(0x18, 0, 0, 0) < 2) {
    // can't open I2C
    fprintf(stderr, "can't open I2C\n");
    exit(EXIT_FAILURE);
  }
  if (write_hex_digits(0x02, 0, 0, 0) < 1) {
    // can't open I2C
    fprintf(stderr, "can't open I2C\n");
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
     
  while(key != 'Q') {
    usleep(20000);
    key = toupper(getkey());

    // read hex keys
    if ((key >= '0' && key <= '9') | (key >= 'A' && key <= 'F')) {
      if (key <= '9') {
	nibble = (key - '0');
      } else {
	nibble = (key - 'A') + 10;
      }
      if (hi_nibble) {
	hi_nibble = FALSE;
	hexin &= 0x0F;
	hexin |= nibble << 4;
      } else {
	hi_nibble = TRUE;
	hexin &= 0xF0;
	hexin |= nibble;
      }
      if (elf_mode == ADDRESS) {
	// address input
	if (hi_byte) {
	  adr &= 0x00FF;
	  adr |= hexin << 8;
	  if (hi_nibble) {
	    hexin = adr & 0x00FF;
	    hi_byte = FALSE;
	  }
	} else {
	  adr &= 0xFF00;
	  adr |= hexin;
	  if (hi_nibble) {
	    hexin = adr >> 8;
	    hi_byte = TRUE;
	  }
	}	
      } else {
	// data input
	data = hexin;
	write_byte(data);
      }
    }
        
    // simple statemachine
    switch (elf_mode) {
    case LOAD_READ:
      switch (key) {
      case EOF:
	// no key pressed
	write_hex_digits(read_byte(), TRUE, write_protect, 0);
	write_hex_digits(adr & 0x00FF, FALSE, FALSE, 1);
	write_hex_digits(adr >> 8, TRUE, FALSE, 2);             
	break;
      case '\n':
      case 'I':
	// INPUT key
	adr++;
	hi_nibble = TRUE;
	usleep(100);
	digitalWrite(IN_N, 0);
	usleep(100);
	digitalWrite(IN_N, 1); 
	break; 
      case '-':
      case 'W':
	// write enable for LOAD
	elf_mode = LOAD_WRITE;
	hi_nibble = TRUE;
	write_protect = FALSE;
	digitalWrite(WRITE_N, 0);
	break;
      case '+':
      case 'R':
	// RUN
	elf_mode = RUN;
	digitalWrite(WAIT_N, 1);
	digitalWrite(CLEAR_N, 1);
	hi_nibble = TRUE;
	break;
      case '.':
      case 'L':
	// LOAD again -> read address
	elf_mode = ADDRESS;
	hi_nibble = TRUE;
	hi_byte = TRUE;
	hexin = adr >> 8;
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
      case '\n':
      case 'I':
	// INPUT
	adr++;
	usleep(100);
	digitalWrite(IN_N, 0);
	usleep(100);
	digitalWrite(IN_N, 1); 
	hi_nibble = TRUE;
	break; 
      case '-':
      case 'W':
	// write protect for LOAD
	elf_mode = LOAD_READ;
	write_protect = TRUE;
	digitalWrite(WRITE_N, 1);
	hi_nibble = TRUE;
	break;
      case '+':
      case 'R':
	// RUN
	elf_mode = RUN;
	digitalWrite(WAIT_N, 1);
	digitalWrite(CLEAR_N, 1);
	hi_nibble = TRUE;
	break;
      case '.':
      case 'L':
	// LOAD again -> read address
	elf_mode = ADDRESS;
	hi_nibble = TRUE;
	hexin = adr >> 8;
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
      case '-':
      case 'W':
	// Toggle write
	if (write_protect) {
	  write_protect = FALSE;
	  digitalWrite(WRITE_N, 0);
	} else {
	  write_protect = TRUE;
	  digitalWrite(WRITE_N, 1);
	}
	hi_nibble = TRUE;
	break;
      case '+':
      case 'R':
	// WAIT
	digitalWrite(WAIT_N, 0);
	elf_mode = WAIT;
	hi_nibble = TRUE;
	break;
      case '.':
      case 'L':
	// LOAD mode
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
	hi_nibble = TRUE;
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
      case '-':
      case 'W':
	if (write_protect) {
	  write_protect = FALSE;
	  digitalWrite(WRITE_N, 0);
	} else {
	  write_protect = TRUE;
	  digitalWrite(WRITE_N, 1);
	}
	hi_nibble = TRUE;
	break;
      case '+':
      case 'R':
	// RUN
	digitalWrite(WAIT_N, 1);
	elf_mode = RUN;
	hi_nibble = TRUE;
	break;
      case '.':
      case 'L':
	// LOAD mode
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
	hi_nibble = TRUE;
	break;
      }
      break;
    case ADDRESS:
      switch (key) {
      case EOF:
	// no key pressed
	// write_hex_digits(read_switches(), TRUE, write_protect, 0);
	write_hex_digits(adr & 0x00FF, FALSE, TRUE, 1);
	write_hex_digits(adr >> 8, TRUE, FALSE, 2);             
	break;
      case '.':
      case 'L':
      case '\n':
	// address input completed
	// reset
	digitalWrite(WAIT_N, 1);
	usleep(100);
	digitalWrite(WAIT_N, 0);
	usleep(100);
	// count up to adr
	for (i=0; i<adr; i++) {
	  digitalWrite(IN_N, 0);
	  digitalWrite(IN_N, 1); 
	}
	if (write_protect) {
	  elf_mode = LOAD_READ;
	  // get first byte
	  digitalWrite(IN_N, 0);
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
