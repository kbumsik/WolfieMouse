/********************************************************************
 * File:    flash.h
 * Author:  Bryant Gonzaga
 *
 *******************************************************************/

#ifndef FLASH_H_
#define FLASH_H_

#include "common_header.h"

#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) /* Base @ of Sector 0, 16 Kbytes */

uint32_t write_flash(uint32_t num_of_bytes, uint8_t* data);

void read_flash(uint32_t num_of_bytes, uint8_t* data);

#endif /* FLASH_H_ */
