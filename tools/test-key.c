#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <linux/input.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {

  /* how many bytes were read */
  size_t rb;

  struct input_event ev;
  int fd;
  int yalv;
  
  fd = open("/dev/input/event0", O_RDONLY);  

  while(1) {
    rb=read(fd, &ev, sizeof(struct input_event));

    if (rb < (int) sizeof(struct input_event)) {
      perror("evtest: short read");
      exit (1);
    }

    if (EV_KEY == ev.type) {
      printf("Code: %d 0x%x, value %d\n", ev.code, ev.code, ev.value);
    } else {
      //      printf("Type: %d not a key event\n", ev.type);
    }

  }
}
