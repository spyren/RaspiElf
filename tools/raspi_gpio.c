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


/*
 ** ===================================================================
 **  Method      :  init_port_level
 */
/**
 *  @brief
 *      Initialises the Raspi GPIO port level
 *  @return
 *      int		error number -1 wiringPi, -2 any switch to ground
 */
/* ===================================================================*/
int init_port_level(void) {

    // write mode
    digitalWrite(WRITE_N, 0);
    
    // run
    digitalWrite(WAIT_N, 1);
    digitalWrite(CLEAR_N, 1);
    
    // in disable
    digitalWrite(IN_N, 1);

    if (!digitalRead(WAIT_N) || 
		!digitalRead(CLEAR_N) || !digitalRead(IN_N)) {
		// any of the mode pins is low -> switch in wrong position
		pinMode(WRITE_N, INPUT);
		pinMode(WAIT_N, INPUT);
		pinMode(CLEAR_N, INPUT);
		return -2;
	}
	 
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
    if (!digitalRead(OUTPUT_0) || !digitalRead(OUTPUT_1) || 
		!digitalRead(OUTPUT_2) || !digitalRead(OUTPUT_3) || 
		!digitalRead(OUTPUT_4) || !digitalRead(OUTPUT_5) ||
		!digitalRead(OUTPUT_6) || !digitalRead(OUTPUT_7)) 
	{
		// any of the data out pins is low -> switch in wrong position
		pinMode(OUTPUT_0, INPUT);
		pinMode(OUTPUT_1, INPUT);
		pinMode(OUTPUT_2, INPUT);
		pinMode(OUTPUT_3, INPUT);
		pinMode(OUTPUT_4, INPUT);
		pinMode(OUTPUT_5, INPUT);
		pinMode(OUTPUT_6, INPUT);
		pinMode(OUTPUT_7, INPUT);
		return -2;
	}
	    
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
 **  Method      :  init_port_mode
 */
/**
 *  @brief
 *      Initialises the Raspi GPIO port mode (direction, pullups)
 *  @return
 *      int		error number -1 wiringPi, -2 any switch to ground
 */
/* ===================================================================*/
int init_port_mode(void) {
    if (wiringPiSetupGpio() == -1) {
        return -1;
    }
    
    // read mode
    pinMode(WRITE_N, OUTPUT);
    
    // run
    pinMode(WAIT_N, OUTPUT);
    pinMode(CLEAR_N, OUTPUT);
    	
    // in 
    pinMode(IN_N, OUTPUT);
      
    // all outputs (switches)
    pinMode(OUTPUT_0, OUTPUT);
    pinMode(OUTPUT_1, OUTPUT);
    pinMode(OUTPUT_2, OUTPUT);
    pinMode(OUTPUT_3, OUTPUT);
    pinMode(OUTPUT_4, OUTPUT);
    pinMode(OUTPUT_5, OUTPUT);
    pinMode(OUTPUT_6, OUTPUT);
    pinMode(OUTPUT_7, OUTPUT);
	    
	// all inputs (LED)
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
 **  Method      :  init_port_read
 */
/**
 *  @brief
 *      Initialises the Raspi GPIO port read only mode (direction, pullups)
 *  @return
 *      int		error number -1 wiringPi, -2 any switch to ground
 */
/* ===================================================================*/
int init_port_read(void) {
    if (wiringPiSetupGpio() == -1) {
        return -1;
    }
    
    // read mode
    pinMode(WRITE_N, INPUT);
    
    // run
    pinMode(WAIT_N, INPUT);
    pinMode(CLEAR_N, INPUT);
    	
    // in 
    pinMode(IN_N, INPUT);
      
    // all outputs (switches)
    pinMode(OUTPUT_0, INPUT);
    pinMode(OUTPUT_1, INPUT);
    pinMode(OUTPUT_2, INPUT);
    pinMode(OUTPUT_3, INPUT);
    pinMode(OUTPUT_4, INPUT);
    pinMode(OUTPUT_5, INPUT);
    pinMode(OUTPUT_6, INPUT);
    pinMode(OUTPUT_7, INPUT);
	    
	// all inputs (LED)
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
 *      Writes a byte (data switches) to the Elf
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
int read_switches(void) {
    return (digitalRead(OUTPUT_0) + (digitalRead(OUTPUT_1) << 1) + 
    (digitalRead(OUTPUT_2) << 2) + (digitalRead(OUTPUT_3) << 3) +
    (digitalRead(OUTPUT_4) << 4) + (digitalRead(OUTPUT_5) << 5) + 
    (digitalRead(OUTPUT_6) << 6) + (digitalRead(OUTPUT_7) << 7));
}


