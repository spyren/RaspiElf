/**
 *  @brief
 *      Interface to the Elf Membership Card.
 * 
 * 		The wiringPi library is used to control the GPIOs. 
 *
 *  @file
 *      raspi_gpio.c
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


#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wiringPi.h>
#include "raspi_gpio.h"

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
 *      None
 */
/* ===================================================================*/
int init_ports(void) {
    if (wiringPiSetupGpio() == -1) {
        return -1;
    }
    
    // read mode
    pinMode(WE_N, OUTPUT);
    digitalWrite(WE_N, 1);
    
    // run
    pinMode(WAIT_N, OUTPUT);
    digitalWrite(WAIT_N, 1);
    pinMode(CLEAR_N, OUTPUT);
    digitalWrite(CLEAR_N, 1);
    
    // in disable
    pinMode(IN_N, OUTPUT);
    digitalWrite(IN_N, 1);
    
    // reset
    digitalWrite(CLEAR_N, 0);
    
    // load
    digitalWrite(WAIT_N, 0);
    
    // all outputs are high
    pinMode(OUTPUT_0, OUTPUT);
    pinMode(OUTPUT_1, OUTPUT);
    pinMode(OUTPUT_2, OUTPUT);
    pinMode(OUTPUT_3, OUTPUT);
    pinMode(OUTPUT_4, OUTPUT);
    pinMode(OUTPUT_5, OUTPUT);
    pinMode(OUTPUT_6, OUTPUT);
    pinMode(OUTPUT_7, OUTPUT);
    write_byte(0xFF);      
    
    pinMode(INPUT_0, INPUT);
    pinMode(INPUT_1, INPUT);
    pinMode(INPUT_2, INPUT);
    pinMode(INPUT_3, INPUT);
    pinMode(INPUT_4, INPUT);
    pinMode(INPUT_5, INPUT);
    pinMode(INPUT_6, INPUT);
    pinMode(INPUT_7, INPUT);
    // input pins have pull ups
    pullUpDnControl (INPUT_0, PUD_UP) ;
    pullUpDnControl (INPUT_1, PUD_UP) ;
    pullUpDnControl (INPUT_2, PUD_UP) ;
    pullUpDnControl (INPUT_3, PUD_UP) ;
    pullUpDnControl (INPUT_4, PUD_UP) ;
    pullUpDnControl (INPUT_5, PUD_UP) ;
    pullUpDnControl (INPUT_6, PUD_UP) ;
    pullUpDnControl (INPUT_7, PUD_UP) ;

    return 0;
}
    
/*
 ** ===================================================================
 **  Method      :  write_byte
 */
/**
 *  @brief
 *      Writes a byte to the Elf
 *  @param
 *      byte    the data to write
 *  @return
 *      None
 */
/* ===================================================================*/
void write_byte(int byte) {
    if (byte & 0b00000001)
        digitalWrite(OUTPUT_0, 1);
    else
        digitalWrite(OUTPUT_0, 0);     
    if (byte & 0b00000010)
        digitalWrite(OUTPUT_1, 1);
    else
        digitalWrite(OUTPUT_1, 0);     
    if (byte & 0b00000100)
        digitalWrite(OUTPUT_2, 1);
    else
        digitalWrite(OUTPUT_2, 0);     
    if (byte & 0b00001000)
        digitalWrite(OUTPUT_3, 1);
    else
        digitalWrite(OUTPUT_3, 0);     
    if (byte & 0b00010000)
        digitalWrite(OUTPUT_4, 1);
    else
        digitalWrite(OUTPUT_4, 0);     
    if (byte & 0b00100000)
        digitalWrite(OUTPUT_5, 1);
    else
        digitalWrite(OUTPUT_5, 0);     
    if (byte & 0b01000000)
        digitalWrite(OUTPUT_6, 1);
    else
        digitalWrite(OUTPUT_6, 0);     
    if (byte & 0b10000000)
        digitalWrite(OUTPUT_7, 1);
    else
        digitalWrite(OUTPUT_7, 0);     
}

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
int read_byte(void) {
    return (digitalRead(INPUT_0) + (digitalRead(INPUT_1) << 1) + 
    (digitalRead(INPUT_2) << 2) + (digitalRead(INPUT_3) << 3) +
    (digitalRead(INPUT_4) << 4) + (digitalRead(INPUT_5) << 5) + 
    (digitalRead(INPUT_6) << 6) + (digitalRead(INPUT_7) << 7));
}


