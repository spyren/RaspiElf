/**
 *  @brief
 *      Interface to the Microdot pHAT Display.
 *
 *      It displays uint8 as two hex digits. 
 *      The font looks like TIL311.
 * 
 *      The wiringPi library is used to control the I2C. 
 *
 *  @file
 *      microdot_phat_hex.c
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
#include <wiringPi.h>
#include <wiringPiI2C.h>

#include "microdot_phat_hex.h"

#define debug       0

#define DRIVER_BASE (0x61)


#define CONFIGURATION_REGISTER      0x00 // Set operation mode of IS31FL3730 
#define MATRIX_1_DATA_REGISTER      0x01 // Data Register Store the on or off state of each LED 
#define MATRIX_2_DATA_REGISTER      0x0E // Data Register Store the on or off state of each LED 
#define UPDATE_COLUMN_REGISTER      0x0C // Make the Data Register update the data 
#define LIGHTING_EFFECT_REGISTER    0x0D // Effect Register, Store the intensity control settings
#define PWM_REGISTER                0x19 // Modulate LED light with 128 different items
#define RESET_REGISTER              0xFF // Reset all registers to default value

#define SOFTWARE_SHUTDOWN_MODE      0x80
#define MATRIX1_AND_MATRIX2         0x18
#define AUDIO_INPUT_ENABLE          0x04
#define DOT_MATRIX_8x8              0x00

  
const uint8_t font_til311[16][7]={
  {   // 0 
    0b0110,
    0b1001,
    0b1001,
    0b1001,
    0b1001,
    0b1001,
    0b0110
  },
  {   // 1
    0b0001,
    0b0001,
    0b0001,
    0b0001,
    0b0001,
    0b0001,
    0b0001
  },
  {   // 2 
    0b1110,
    0b0001,
    0b0001,
    0b0110,
    0b1000,
    0b1000,
    0b1111
  },
  {   // 3 
    0b1110,
    0b0001,
    0b0001,
    0b0110,
    0b0001,
    0b0001,
    0b1110
  },
  {   // 4 
    0b1000,
    0b1001,
    0b1001,
    0b1111,
    0b0001,
    0b0001,
    0b0001
  },
  {   // 5
    0b1111,
    0b1000,
    0b1000,
    0b1110,
    0b0001,
    0b0001,
    0b1110
  },
  {   // 6 
    0b0110,
    0b1000,
    0b1000,
    0b1110,
    0b1001,
    0b1001,
    0b0110
  },
  {   // 7
    0b1111,
    0b0001,
    0b0001,
    0b0001,
    0b0001,
    0b0001,
    0b0001
  },
  {   // 8
    0b0110,
    0b1001,
    0b1001,
    0b0110,
    0b1001,
    0b1001,
    0b0110
  },
  {   // 9
    0b0110,
    0b1001,
    0b1001,
    0b0111,
    0b0001,
    0b0001,
    0b0110
  },
  {   // A
    0b0110,
    0b1001,
    0b1001,
    0b1111,
    0b1001,
    0b1001,
    0b1001
  },
  {   // B
    0b1110,
    0b1001,
    0b1001,
    0b1110,
    0b1001,
    0b1001,
    0b1110
  },
  {   // C
    0b0111,
    0b1000,
    0b1000,
    0b1000,
    0b1000,
    0b1000,
    0b0111
  },
  {   // D
    0b1110,
    0b1001,
    0b1001,
    0b1001,
    0b1001,
    0b1001,
    0b1110
  },
  {   // E
    0b1111,
    0b1000,
    0b1000,
    0b1111,
    0b1000,
    0b1000,
    0b1111
  },
  {   // F
    0b1111,
    0b1000,
    0b1000,
    0b1110,
    0b1000,
    0b1000,
    0b1000
  }
};

#if debug == 1
void print_dot(uint8_t row) {

  int i;
  printf("%2x", row);
  for (i=7; i>=0; i--) {
    if ((row >> i) & 0x01) {
      printf("*");
    } else {
      printf("-");
    }
  }
  printf("\n");
}
#endif

uint8_t mirror_5bit(uint8_t input) {
  uint8_t returnval = 0;
  int i;
  for (i = 0; i < 5; ++i) {
    returnval |= (input & 0x01);
    input >>= 1;
    returnval <<= 1;
  }
  return returnval;
}


/*
 ** ===================================================================
 **  Method      :  write_hex_digits
 */
/**
 *  @brief
 *      It displays uint8 as two hex digits. 
 *      The font looks like TIL311.
 *  @param
 *      data[in]            data to display
 *  @param
 *      hi_nibble_dp[in]    high nibble decimal point (0 off)
 *  @param
 *      low_nibble_dp[in]   low nibble decimal point (0 off)
 *  @param
 *      position[in]        position: 0 right, 1 middle, 2 left
 *
 *  @return
 *      int     error number: -1 wiringPi, -2 parameter
 */
/* ===================================================================*/
int write_hex_digits(uint8_t data, uint8_t hi_nibble_dp, 
                     uint8_t low_nibble_dp, uint8_t position) {

  int fd;
  uint8_t matrix[2][8];
  int c;
  int row;
  int column;
  uint8_t font_x_y[7];
  uint8_t low_dp = 0;
  uint8_t hi_dp = 0;

  if (position >= 0 && position < 3) {
    fd = wiringPiI2CSetup(DRIVER_BASE + position);
    if (fd < 0) {
      // can't open I2C device
      return (-1);
    }
  } else {
    // invalid position
    return (-2);
  }

  if (low_nibble_dp) {
    // notice low nibble decimal point
    low_dp = 0x80;
  }

  if (hi_nibble_dp) {
    // notice high nibble decimal point
    hi_dp = 0xFF;
  }

  wiringPiI2CWriteReg8 (fd, RESET_REGISTER, 0) ;
  wiringPiI2CWriteReg8 (fd, CONFIGURATION_REGISTER, MATRIX1_AND_MATRIX2 | DOT_MATRIX_8x8) ;

  // low nibble
  c = data & 0x0F;
  for (row=0; row<7; row++) { 
#if debug == 1  
    print_dot(font_til311[c][row]);
#endif
    wiringPiI2CWriteReg8 (fd, MATRIX_1_DATA_REGISTER+row, (mirror_5bit(font_til311[c][row]) >> 1) | low_dp);
  }

#if debug == 1
  printf("\n");
#endif

  // high nibble
  c = data >> 4;
  for (column=0; column<7; column++) {
    font_x_y[column] = 0;   
    for (row=0; row<7; row++) {
      if (font_til311[c][row] & (0x10 >> column)) {
	font_x_y[column] |= (0x01 << row);
      }   
    }
  }
  for (row=0; row<7; row++) { 
#if debug == 1
    print_dot(font_x_y[row]);
#endif
    wiringPiI2CWriteReg8 (fd, MATRIX_2_DATA_REGISTER+row, font_x_y[row]);
  }
  wiringPiI2CWriteReg8 (fd, MATRIX_2_DATA_REGISTER+7, hi_dp);

#if debug == 1
  printf("\n");
#endif

  wiringPiI2CWriteReg8 (fd, UPDATE_COLUMN_REGISTER, 0) ;

  close(fd);
}


