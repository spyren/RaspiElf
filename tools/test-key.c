/**
 *  @brief
 *      Reads keyboard events for /dev/input/event0 or a specified device.
 *
 *   	synopsis
 *       $ test-key [-v] [<device-filename>]  
 *
 *      without command parameter get is executed
 * 
 *     -v
 *        verbose, all events are printed out
 *  @file
 *      elf-test.c
 *  @author
 *      Peter Schmid, peter@spyr.ch
 *  @date
 *      2018-05-20
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
#include <stdbool.h>
#include <unistd.h>
#include <termios.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <linux/input.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAX_FN_LENGTH 200

void usage_exit(int err_number, const char *str);

int main(int argc, char *argv[]) {

  /* how many bytes were read */
  size_t rb;

  struct input_event ev;
  char fn[MAX_FN_LENGTH];
  int fd;

  uint8_t verbose_mode = false;
  int opt;

  
  // parse command line options
  while ((opt = getopt(argc, argv, "v")) != -1) {
    switch (opt) {
    case 'v':
      verbose_mode = true;
      break;
    default:
      usage_exit(EXIT_FAILURE, argv[0]);
      break;
    }
  }

  // parse command
  if (argc - optind <= 1) {
    // no or one parameter left -> filename for device
    if (argc - optind  == 1) {
      strncpy(fn, argv[optind], MAX_FN_LENGTH);
    } else {
      // no parameter -> default filename
      strncpy(fn, "/dev/input/event0", MAX_FN_LENGTH); 
    }
  } else {
    // too many parameters
    usage_exit(EXIT_FAILURE, argv[0]);
  }
  
  fd = open(fn, O_RDONLY);
  if (fd < 0) {
    perror("can not open file");
    exit (1);
  }

  while(1) {
    rb=read(fd, &ev, sizeof(struct input_event));

    if (rb < (int) sizeof(struct input_event)) {
      perror("evtest: short read");
      exit (1);
    }

    if (EV_KEY == ev.type) {
      printf("Code: %d 0x%x, value %d\n", ev.code, ev.code, ev.value);
    } else {
      if (verbose_mode) {
	printf("Type: %d not a key event\n", ev.type);
      }
    }

  }
}

void usage_exit(int err_number, const char *str) {
  fprintf(stderr, "\
Usage: %s [-v] [<device-filename>]\n\
-v verbose\n\
\n",
	  str);
  exit(err_number);
}
