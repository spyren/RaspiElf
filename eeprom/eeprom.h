/**
 *  @brief
 *      Defines for SPI EEPROMs. 
 *
 *  @file
 *      eeprom.h
 *  @author
 *      Peter Schmid, peter@spyr.ch
 *  @date
 *      2019-01-26
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

#ifndef EEPROM_H_
#define EEPROM_H_

#define START_ADR   (0x00000)
#define END_ADR     (0x1FFFF)

#define SPEED       (500000)
#define CHANNEL     (1)

// EEPROM commands
#define READ_CMD    (0x03)
#define WRITE_CMD   (0x02)
#define WREN_CMD    (0x06)
#define WRDI_CMD    (0x04)
#define RDSR_CMD    (0x05)
#define WRSR_CMD    (0x01)

#define WRITE_IN_PROCESS    (0x01)
 
#define PAGE_SIZE   (256)

#endif /* EEPROM_H_ */

