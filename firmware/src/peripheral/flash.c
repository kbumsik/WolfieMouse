/********************************************************************
 * file:    flash.c
 * Author:  Bryant Gonzaga
 *
 *******************************************************************/

#include "common_source.h"
#include "flash.h"

int write_flash(uint8_t* data, size_t bytes)
{
    /* Check Parameters */
    if (bytes > 0 && data == NULL) {
        return KB_ERROR;
    }
    if (bytes >= FLASH_SIZE_SECTOR_1) {
        return KB_ERROR;
    }
    /* Variables */
    static FLASH_EraseInitTypeDef flash_erase_struct;
    uint32_t sector_error;
    uint32_t address = FLASH_ADDR_SECTOR_1;

    /* Unlock Flash */
    HAL_FLASH_Unlock();
    
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR
                    | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);

    /* Erase Flash Struct */
    flash_erase_struct.TypeErase = FLASH_TYPEERASE_SECTORS;
    flash_erase_struct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
    flash_erase_struct.Sector = FLASH_SECTOR_1;
    flash_erase_struct.NbSectors = 1;

    /* Erase Flash */
    if (HAL_FLASHEx_Erase(&flash_erase_struct, &sector_error) != HAL_OK) {
        return HAL_ERROR;
    }

    /* Clear Cache */
    __HAL_FLASH_DATA_CACHE_DISABLE();
    __HAL_FLASH_INSTRUCTION_CACHE_DISABLE();

    __HAL_FLASH_DATA_CACHE_RESET();
    __HAL_FLASH_INSTRUCTION_CACHE_RESET();

    __HAL_FLASH_INSTRUCTION_CACHE_ENABLE();
    __HAL_FLASH_DATA_CACHE_ENABLE();
    
    /* Write Bytes to Flash */
    HAL_StatusTypeDef ret;
    for (; bytes > 0; bytes--) {
        ret = HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, address++, *(data++));
        if (ret != HAL_OK) {
            /* Lock Flash */
            HAL_FLASH_Lock();
            /* Return Error */
            return ret;
        }
    }

    /* Write Magic number */
    ret = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLASH_MAGIC_ADDR, FLASH_MAGIC_NUMBER);
    if (ret != HAL_OK) {
        /* Lock Flash */
        HAL_FLASH_Lock();
        /* Return Error */
        return ret;
    }
    
    /* Lock Flash */
    HAL_FLASH_Lock();

    /* Everything Went Well */
    return HAL_OK;
}

void read_flash(uint8_t* data, size_t offset, size_t bytes)
{
    /* Check Parameters */
    if (bytes > 0 && data == NULL) {
        return;
    }
    /* Read Flash */
    uint8_t *address = (__IO uint32_t*)(FLASH_ADDR_SECTOR_1 + offset);
    for (; bytes > 0; bytes--) {
        *(data++) = *(address++);
    }
}
