/**
 *  @brief
 *      Interface to the Microdot pHAT Display.
 *
 * 		It displays uint8 as two hex digits. 
 * 		The font looks like TIL311.
 * 
 * 		The wiringPi library is used to control the I2C. 
 *
 *  @file
 *      microdot_phat_hex.h
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

#ifndef MICRODOT_PHAT_HEX_H_
#define MICRODOT_PHAT_HEX_H_

/*
 ** ===================================================================
 **  Method      :  write_hex_digits
 */
/**
 *  @brief
 * 		It displays uint8 as two hex digits. 
 * 		The font looks like TIL311.
 * 	@param
 * 		data[in]			data to display
 * 	@param
 * 		hi_nibble_dp[in]	high nibble decimal point (0 off)
 * 	@param
 * 		low_nibble_dp[in]	low nibble decimal point (0 off)
 * 	@param
 * 		position[in]		position: 0 right, 1 middle, 2 left
 *
 *  @return
 *      int		error number: -1 wiringPi, -2 parameter
 */
/* ===================================================================*/
int write_hex_digits(uint8_t data, uint8_t hi_nibble_dp, 
				     uint8_t low_nibble_dp, uint8_t position);
				     

#endif /* MICRODOT_PHAT_HEX_H_ */
