/********************************************************************
 * File:    flash.h
 * Author:  Bryant Gonzaga
 *
 *******************************************************************/

#ifndef FLASH_H_
#define FLASH_H_

#include "common_header.h"

#define FLASH_ADDR_SECTOR_1     ((uint32_t)0x08004000) /* Base addr of Sector 1, 16 Kbytes */
#define FLASH_SIZE_SECTOR_1     ((uint32_t)0x3FFC)     /* 16 Kbytes - 4 bytes */
#define FLASH_MAGIC_ADDR        ((uint32_t)0x08007FFC) /* Magic number address */
#define FLASH_MAGIC_NUMBER      ((uint32_t)0x07738135) /* This Magic number */  

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Writes the given data to Sector 1 of the flash. If users pass 0
 * for the num_of_bytes then this function simple erases Sector 0
 * 
 * @param data          pointer to an array of bytes
 * @param num_of_bytes  number of bytes in the data array
 * 
 * @return HAL_OK if all goes well. If not an error code is returned
 */
int write_flash(uint8_t* data, size_t bytes);

/**
 * Reads a number of bytes, specified by num_of_bytes, from flash and
 * stores them in the data array. Specifically reads from Sector 1.
 * 
 * @param data          pointer to data array
 * @param num_of_bytes  number of bytes to be read
 */
void read_flash(uint8_t* data, size_t offset, size_t bytes);

#ifdef __cplusplus
}
#endif

#endif /* FLASH_H_ */
