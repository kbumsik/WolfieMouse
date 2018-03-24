/*********************************************************************
 * File:        FlashIO.cc
 * Author:      Bryant Gonzaga
 * Created:     3/23/2018
 * Modified:    3/23/2018
 *
 * Notes:
 *
 * Description:
 *  
 ********************************************************************/

#include "FlashIO.hpp"

FlashIO::FlashIO()
{
    /* Initialize Variables */
    dataStatus = NO_FLASH_DATA;
    sizeOfBuffer = 0;
}

size_t FlashIO::write(const void *ptr, size_t size, size_t nmemb)
{
    sizeOfBuffer = nmemb * size;
    uint32_t writeStatus = write_flash( (uint8_t*) ptr, sizeOfBuffer );
    /* If every thing is ok */
    if (writeStatus == HAL_OK) {
        dataStatus = NEW_FLASH_DATA;
        return KB_OK;
    }

    dataStatus = NO_FLASH_DATA;
    return writestatus;
}

size_t FlashIO::read(uint8_t* readBuffer, uint32_t size)
{
    /* Check flash was actually written */
    if (dataStatus == NO_FLASH_DATA) {
        return KB_ERROR;
    }
    /* Check for null pointer */
    if (readBuffer == NULL) {
        return KB_ERROR;
    }
    /* Check that size matches written size */
    if (size != sizeOfBuffer) {
        return KB_ERROR;
    }
    /* Read Data from Flash */
    read_flash(readBuffer, size);

    return KB_OK;
}
