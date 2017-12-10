/**
 *  @brief
 *      Copies the Elf (Membership Card) memory to a binary file. 
 *
 *  @file
 *      raspi_gpio.h
 *  @author
 *      Peter Schmid, peter@spyr.ch
 *  @date
 *      2017-11-30
 *  @remark
 *      Language: gcc version 4.9.2 on Raspberry Pi 3, Raspbian
 *  @copyright
 *      Peter Schmid, Switzerland
 *
 *      This file is part of "RaspiElf" software.
 *
 *		"RaspiElf" software is free software: you can redistribute it
 *		and/or modify it under the terms of the GNU General Public License as
 *		published by the Free Software Foundation, either version 3 of the
 *		License, or (at your option) any later version.
 *
 *		"RaspiElf" is distributed in the hope that it will be useful,
 *		but WITHOUT ANY WARRANTY; without even the implied warranty of
 *		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *		GNU General Public License for more details.
 *
 *		You should have received a copy of the GNU General Public License along
 *		with "RaspiElf". If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef RASPI_GPIO_H_
#define RASPI_GPIO_H_

// Port numbers (BCM numbering system)
#define IN_N        4
#define WAIT_N      17
#define CLEAR_N     18
#define WE_N        27
#define OUTPUT_0    22
#define OUTPUT_1    23
#define OUTPUT_2    24
#define OUTPUT_3    10
#define OUTPUT_4    9
#define OUTPUT_5    25
#define OUTPUT_6    11
#define OUTPUT_7    8
#define INPUT_0     7
#define INPUT_1     5
#define INPUT_2     6
#define INPUT_3     12
#define INPUT_4     13
#define INPUT_5     18
#define INPUT_6     16
#define INPUT_7     26
#define RX_Q		15
#define TX_EF3		14

// default start and end address
#define START_ADR   0x0000
#define END_ADR     0xFFFF

/*
 ** ===================================================================
 **  Method      :  init_ports
 */
/**
 *  @brief
 *      Initialises the Raspi GPIO ports
 *  @return
 *      int		error number -1 wiringPi, -2 any switch to ground
 */
/* ===================================================================*/
int init_ports(void);

/*
 ** ===================================================================
 **  Method      :  write_byte
 */
/**
 *  @brief
 *      Writes a byte (data switches) to the Elf
 *  @param
 *      byte    the data to write
 *  @return
 *      None
 */
/* ===================================================================*/
void write_byte(int byte);

/*
 ** ===================================================================
 **  Method      :  read_byte
 */
/**
 *  @brief
 *      Reads a byte from the Elf
 *  @param
 *     
 *  @return
 *      The byte from the Elf
 */
/* ===================================================================*/
int read_byte(void);

/*
 ** ===================================================================
 **  Method      :  read_switches
 */
/**
 *  @brief
 *      Reads the data switches from the Elf
 *  @param
 *     
 *  @return
 *      The byte from the Elf
 */
/* ===================================================================*/
int read_switches(void);

#endif /* RASPI_GPIO_H_ */
