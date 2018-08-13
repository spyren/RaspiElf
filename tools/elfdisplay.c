/**
 *  @brief
 *      Reads a hex keypad and sets the data and gets LED data of an Elf (Membership Card) and 
 *      shows address and data on micro dot pHAT display.
 *
 *      The console (stdin) can be used as keyboard or the EV_KEY events from USB keypad 
 *      (e.g. /dev/input/event0). 
 *
 *      Synopsis
 *        $ elfdisplay [-v] [<device-filename>]
 *
 *      The decimal points are used to display the modes:
 *
 *      LOAD
 *            #        #####        ###        #####       #####      ######  
 *           ##       #     #      #   #      #     #     #     #     #     # 
 *          # #       #     #     #     #           #     #           #     # 
 *            #        #####      #     #      #####      #           #     # 
 *            #       #     #     #     #     #           #           #     # 
 *            #       #     #      #   #      #           #     #     #     # 
 *      OOO #####      #####        ###       ####### ###  #####      ######
 *  
 *      RUN
 *            #        #####        ###        #####       #####      ######  
 *           ##       #     #      #   #      #     #     #     #     #     # 
 *          # #       #     #     #     #           #     #           #     # 
 *            #        #####      #     #      #####      #           #     # 
 *            #       #     #     #     #     #           #           #     # 
 *            #       #     #      #   #      #           #     #     #     # 
 *          ##### OOO  #####        ###       ####### ###  #####      ######
 *  
 *      WAIT
 *            #        #####        ###        #####       #####      ######  
 *           ##       #     #      #   #      #     #     #     #     #     # 
 *          # #       #     #     #     #           #     #           #     # 
 *            #        #####      #     #      #####      #           #     # 
 *            #       #     #     #     #     #           #           #     # 
 *            #       #     #      #   #      #           #     #     #     # 
 *          #####      #####  OOO   ###       ####### ###  #####      ######
 *  
 *      ADDRESS
 *            #        #####        ###        #####       #####      ######  
 *           ##       #     #      #   #      #     #     #     #     #     # 
 *          # #       #     #     #     #           #     #           #     # 
 *            #        #####      #     #      #####      #           #     # 
 *            #       #     #     #     #     #           #           #     # 
 *            #       #     #      #   #      #           #     #     #     # 
 *      ### #####      #####        ###  OOO  ####### ###  #####      ######
 *  
 *      MEMORY PROTECT
 *            #        #####        ###        #####       #####      ######  
 *           ##       #     #      #   #      #     #     #     #     #     # 
 *          # #       #     #     #     #           #     #           #     # 
 *            #        #####      #     #      #####      #           #     # 
 *            #       #     #     #     #     #           #           #     # 
 *            #       #     #      #   #      #           #     #     #     # 
 *      ### #####      #####        ###       ####### ###  #####  OOO ######
 *  
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
#include <linux/input.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "raspi_gpio.h"
#include "microdot_phat_hex.h"

typedef enum {LOAD, RUN, WAIT, ADDRESS, SWITCH} elf_mode_t;

// local prototypes

void usage_exit(int err_number, const char *str);
int getkey();
int getkeyevent(int fd);
void inc_elf();
void reset_elf();
void load_elf();
void run_elf();

int main(int argc, char *argv[]) {
  uint16_t adr = 0;
  uint8_t data = 0;
  elf_mode_t elf_mode;
  uint8_t memory_protect;
  int key;
  uint8_t hi_nibble = TRUE;
  uint8_t hi_byte = TRUE;
  uint8_t nibble;
  uint8_t hexin = 0;
  uint8_t in;
  uint8_t sw;
  int i;
  int fd;
  
  uint8_t verbose_mode = FALSE;
  uint8_t input_device = FALSE;
  int opt;
  
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
  if (argc - optind <= 1) {
    if (argc - optind == 1) {
      // one parameter left -> filename for device
      fd = open(argv[optind], O_RDONLY);
      if (fd < 0) {
	perror("can not open device.");
	exit (1);
      }
      if (ioctl(fd, EVIOCGRAB, 1) != 0) {
	perror("can not set GRAB mode.");
	exit (1);
      }
      input_device = TRUE;
    }
  } else {
    // too many parameters
    usage_exit(EXIT_FAILURE, argv[0]);
  }
  
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

  clear_display();
  
  // clear display
  if (write_hex_digits(0xCD, TRUE, 0, 0) < 0) {
    // can't open I2C
    fprintf(stderr, "can't open I2C\n");
    exit(EXIT_FAILURE);
  }
  if (write_hex_digits(0x18, 0, 0, 2) < 0) {
    // can't open I2C
    fprintf(stderr, "can't open I2C\n");
    exit(EXIT_FAILURE);
  }
  if (write_hex_digits(0x02, 0, 0, 1) < 0) {
    // can't open I2C
    fprintf(stderr, "can't open I2C\n");
    exit(EXIT_FAILURE);
  }
    
  // load mode
  elf_mode = LOAD;
  load_elf();
  reset_elf();
    
  // read
  digitalWrite(WRITE_N, 1);
  memory_protect = TRUE;

  // get first byte
  inc_elf();
  
  usleep(1000);
     
  while(key != 'Q') {
    if (input_device) {
      // get key from event device
      key = toupper(getkeyevent(fd));
      if (key == ' ') {
        printf("wrong key\n");
      }
    } else {
      // get key from stdin
      usleep(20000);
      key = toupper(getkey());
    }

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

    if (elf_mode != SWITCH) {
      in = read_byte();
      sw = read_switches();
    }
    
    // simple statemachine
    switch (elf_mode) {
    case LOAD:
      switch (key) {
      case EOF:
	// no key pressed
	write_hex_digits(memory_protect?in:data, TRUE, memory_protect, 0);
	write_hex_digits(adr & 0x00FF, FALSE, FALSE, 1);
	write_hex_digits(adr >> 8, TRUE, FALSE, 2);             
	break;
      case '\n':
      case 'I':
	// INPUT key
	adr++;
	hi_nibble = TRUE;
	inc_elf();
	break; 
      case '-':
      case 'W':
      case 'M':
	// toggle memory_protect
	if (memory_protect) {
	  // disable memory protect (write)
	  memory_protect = FALSE;	
	  // set address
	  reset_elf();
	  // count up to adr
	  for (i=0; i<adr; i++) {
	    inc_elf();
	  }
	  digitalWrite(WRITE_N, 0);
	} else {
	  // memory protect for LOAD (read)
	  memory_protect = TRUE;
	  digitalWrite(WRITE_N, 1);
	  // get first byte
	  inc_elf(); 
	}
	hi_nibble = TRUE;
	break;
      case '.':
      case 'R':
	// RUN
	elf_mode = RUN;
	hi_nibble = TRUE;
	// default is write for RUN
	memory_protect = FALSE;	
	digitalWrite(WRITE_N, 0);
	run_elf();
	break;
      case '+':
      case 'L':
	// LOAD again -> read address
	elf_mode = ADDRESS;
	hi_nibble = TRUE;
	hi_byte = TRUE;
	hexin = adr >> 8;
	break;
      case 0x08: // backspace
      case 'K':
      case 'S':
	// SWITCH mode
	// put the switch port to all 1s (the switches on EMC can override)
	data = 0xFF;
	hi_nibble = TRUE;
	// read (the switch on EMC can override)
	digitalWrite(WRITE_N, 1);	
	elf_mode = SWITCH;
	clear_display();
      }
      break;
    case RUN:
      switch (key) {
      case EOF:
	write_hex_digits(sw, TRUE, memory_protect, 0);
	write_hex_digits(in, FALSE, FALSE, 1);
	write_hex_digits(0, FALSE, TRUE, 2);
	break;
      case '-':
      case 'W':
      case 'M':
	// Toggle write
	if (memory_protect) {
	  memory_protect = FALSE;
	  digitalWrite(WRITE_N, 0);
	} else {
	  memory_protect = TRUE;
	  digitalWrite(WRITE_N, 1);
	}
	hi_nibble = TRUE;
	break;
      case '.':
      case 'R':
	// WAIT
	digitalWrite(WAIT_N, 0);
	elf_mode = WAIT;
	hi_nibble = TRUE;
	break;
      case '+':
      case 'L':
	// LOAD mode
	load_elf();
	reset_elf();
	adr = 0;
	memory_protect = TRUE;
	digitalWrite(WRITE_N, 1);
	elf_mode = LOAD;
	// get first byte
	inc_elf();
	hi_nibble = TRUE;
	break;
      case 0x08: // backspace
      case 'K':
      case 'S':
	// SWITCH mode
	// put the switch port to all 1s (the switches on EMC can override)
	data = 0xFF;
	hi_nibble = TRUE;
	// read (the switch on EMC can override)
	digitalWrite(WRITE_N, 1);	
	elf_mode = SWITCH;
	clear_display();
      }
      break;
    case WAIT:
      switch (key) {
      case EOF:
	write_hex_digits(sw, TRUE, memory_protect, 0);
	write_hex_digits(in, TRUE, FALSE, 1);
	write_hex_digits(0, FALSE, FALSE, 2);
	break;
      case '-':
      case 'W':
      case 'M':
	if (memory_protect) {
	  memory_protect = FALSE;
	  digitalWrite(WRITE_N, 0);
	} else {
	  memory_protect = TRUE;
	  digitalWrite(WRITE_N, 1);
	}
	hi_nibble = TRUE;
	break;
      case '.':
      case 'R':
	// RUN
	elf_mode = RUN;
	hi_nibble = TRUE;
	run_elf();
	break;
      case '+':
      case 'L':
	// LOAD mode
	load_elf();
	reset_elf();
	adr = 0;
	memory_protect = TRUE;
	digitalWrite(WRITE_N, 1);
	elf_mode = LOAD;
	// get first byte
	inc_elf();
	hi_nibble = TRUE;
	break;
      case 0x08: // backspace
      case 'K':
      case 'S':
	// SWITCH mode
	// put the switch port to all 1s (the switches on EMC can override)
	data = 0xFF;
	hi_nibble = TRUE;
	// read (the switch on EMC can override)
	digitalWrite(WRITE_N, 1);	
	elf_mode = SWITCH;
	clear_display();
      }
      break;
    case ADDRESS:
      switch (key) {
      case EOF:
	// no key pressed
	// write_hex_digits(read_switches(), TRUE, memory_protect, 0);
	write_hex_digits(adr & 0x00FF, FALSE, TRUE, 1);
	write_hex_digits(adr >> 8, TRUE, FALSE, 2);             
	break;
      case '+':
      case 'L':
      case 'I':
      case '\n':
	// address input completed
	reset_elf();
	// count up to adr
	digitalWrite(WRITE_N, 1);
	for (i=0; i<adr; i++) {
	  inc_elf();
	}
	if (memory_protect) {
	  // get first byte
	  inc_elf();
	} else {
	  digitalWrite(WRITE_N, 0);
	}
	elf_mode = LOAD;
	break;
      }
      break;
    case SWITCH:
      switch (key) {
      case 0x08: // backspace
      case 'K':
      case 'S':
	// go to the keypad-mode LOAD
	// LOAD mode
	load_elf();
	// reset
	reset_elf();
	memory_protect = TRUE;
	digitalWrite(WRITE_N, 1);
	adr = 0;
	// get first byte
	inc_elf();
	elf_mode = LOAD;
	hi_nibble = TRUE;
	break;
      }
      break;
    }
        
  } 
    
  exit(0);   
}

void usage_exit(int err_number, const char *str) {
  fprintf(stderr, "\
Usage: %s [-v] [<device-filename>]\n\
-v verbose\n\
\n",
	  str);
  exit(err_number);
}

int getkeyevent(int fd) {
  int k;
  size_t rb;
  struct input_event ev;
  fd_set rfds;
  struct timeval tv;

  /* Watch fd to see when it has input. */
  FD_ZERO(&rfds);
  FD_SET(fd, &rfds);
  tv.tv_sec = 0;
  tv.tv_usec = 20000;
  
  int retval = select(fd+1, &rfds, NULL, NULL, &tv);

  if (retval == -1) {
    perror("select()");
    k = EOF;
  } else if (retval) {
    // Data is available now
    rb=read(fd, &ev, sizeof(struct input_event));

    if (rb < (int) sizeof(struct input_event)) {
      perror("evtest: short read");
      k = EOF;
    }

    if (EV_KEY == ev.type) {
      // key event
      if (ev.value == 0 || ev.value == 2) {
	// key pressed or key repeat
	switch (ev.code) {
	case 1:
	  // escape
	  k = 'd';
	  break;
	case 29:
	  // Ctrl
	  k = 'e';
	  break;
	case 56:
	  // Alt
	  k = 'f';
	  break;
	case 14:
	  // CLR
	  k = 8;
	  break;
	case 69:
	  // NumLock
	  k = 'a';
	  break;
	case 98:
	  // /
	  k = 'b';
	  break;
	case 55:
	  // *
	  k = 'c';
	  break;
	case 74:
	  // -
	  k = '-';
	  break;
	case 71:
	  // 7
	  k = '7';
	  break;
	case 72:
	  // 8
	  k = '8';
	  break;
	case 73:
	  // 9
	  k = '9';
	  break;
	case 75:
	  // 4
	  k = '4';
	  break;
	case 76:
	  // 5
	  k = '5';
	  break;
	case 77:
	  // 6
	  k = '6';
	  break;
	case 78:
	  // +
	  k = '+';
	  break;
	case 79:
	  // 1
	  k = '1';
	  break;
	case 80:
	  // 2
	  k = '2';
	  break;
	case 81:
	  // 3
	  k = '3';
	  break;
	case 82:
	  // 0
	  k = '0';
	  break;
	case 83:
	  // .
	  k = '.';
	  break;
	case 96:
	  // Enter
	  k = '\n';
	  break;
	default:
	  k = ' ';
	  break;
	}
      }
    }
  } else {
    // timeout
    k = EOF;
  }
  return (k);
}

int getkey() {
  int character;
  struct termios orig_term_attr;
  struct termios new_term_attr;

  /* set the terminal to raw mode */
  tcgetattr(fileno(stdin), &orig_term_attr);
  memcpy(&new_term_attr, &orig_term_attr, sizeof(struct termios));
  new_term_attr.c_lflag &= ~(ECHO|ICANON);
  //new_term_attr.c_lflag &= ~(ICANON);
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

void inc_elf() {
  digitalWrite(IN_N, 0);
  usleep(100);
  digitalWrite(IN_N, 1); 
  usleep(100);
}

void reset_elf() {
  digitalWrite(WAIT_N, 1);
  usleep(100);
  digitalWrite(WAIT_N, 0);
  usleep(100);
}

void load_elf() {
  digitalWrite(WAIT_N, 0);
  digitalWrite(CLEAR_N, 0);
  usleep(100);
}

void run_elf() {
  digitalWrite(WAIT_N, 1);
  digitalWrite(CLEAR_N, 1);
}
