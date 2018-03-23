/********************************************************************
 * file:    flash.c
 * Author:  Bryant Gonzaga
 *
 *******************************************************************/

#include "common_source.h"
#include "flash.h"

uint32_t write_flash(uint8_t* data, uint32_t num_of_bytes)
{
    /* Check Parameters */
    if (num_of_bytes > 0 && data == NULL) {
        return KB_ERROR;
    }
    /* Variables */
    static FLASH_EraseInitTypeDef flash_erase_struct;
    uint32_t sector_error;
    uint32_t address = ADDR_FLASH_SECTOR_1;

    /* Unlock Flash */
    HAL_FLASH_Unlock();

    /* Erase Flash Struct */
    flash_erase_struct.TypeErase = FLASH_TYPEERASE_SECTORS;
    flash_erase_struct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
    flash_erase_struct.Sector = FLASH_SECTOR_1;
    flash_erase_struct.NbSectors = 1;

    /* Erase Flash */
    if (HAL_FLASHEx_Erase(&flash_erase_struct, &sector_error) != HAL_OK) {
        return sector_error;
    }

    /* Clear Cache */
    __HAL_FLASH_DATA_CACHE_DISABLE();
    __HAL_FLASH_INSTRUCTION_CACHE_DISABLE();

    __HAL_FLASH_DATA_CACHE_RESET();
    __HAL_FLASH_INSTRUCTION_CACHE_RESET();

    __HAL_FLASH_INSTRUCTION_CACHE_ENABLE();
    __HAL_FLASH_DATA_CACHE_ENABLE();
    
    /* Write Bytes to Flash */
    for (num_of_bytes; num_of_bytes > 0; num_of_bytes--) {
        sector_error = HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE,  address++, *(data++));
        if (sector_error != HAL_OK) {
            /* Lock Flash */
            HAL_FLASH_Lock();
            /* Return Error */
            return sector_error;
        }
    }
    
    /* Lock Flash */
    HAL_FLASH_Lock();

    /* Everything Went Well */
    return HAL_OK;
}

void read_flash(uint8_t* data, uint32_t num_of_bytes)
{
    /* Check Parameters */
    if ( num_of_bytes > 0 && data == NULL) {
        return;
    }
    /* Read Flash */
    uint32_t address = ADDR_FLASH_SECTOR_1;
    for (num_of_bytes; num_of_bytes > 0; num_of_bytes--) {
        *(data++) = *( (__IO uint32_t*) address++ );
    }
}
