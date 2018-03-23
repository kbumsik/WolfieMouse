/********************************************************************
 * File:    flash.h
 * Author:  Bryant Gonzaga
 *
 *******************************************************************/

#ifndef FLASH_H_
#define FLASH_H_

#include "common_header.h"

#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) /* Base @ of Sector 0, 16 Kbytes */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Writes the given data to Sector 0 of the flash. If users pass 0
 * for the num_of_bytes then this function simple erases Sector 0
 * 
 * @param data          pointer to an array of bytes
 * @param num_of_bytes  number of bytes in the data array
 * 
 * @return HAL_OK if all goes well. If not an error code is returned
 */
uint32_t write_flash(uint8_t* data, uint32_t num_of_bytes);

/**
 * 
 */
void read_flash(uint8_t* data, uint32_t num_of_bytes);

#ifdef __cplusplus
}
#endif

#endif /* FLASH_H_ */
