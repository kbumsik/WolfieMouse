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
    offset = 0;

    uint32_t magic_num = *((uint32_t*) FLASH_MAGIC_ADDR);
    if (magic_num != FLASH_MAGIC_NUMBER) {
        dataStatus = NO_FLASH_DATA;
    } else {
        dataStatus = NEW_FLASH_DATA;
    }
}

size_t FlashIO::write(const void *ptr, size_t size, size_t count)
{
    uint32_t writeStatus = write_flash((uint8_t*) ptr, size * count);
    /* If every thing is ok */
    if (writeStatus == KB_OK) {
        dataStatus = NEW_FLASH_DATA;
        return KB_OK;
    }
    return writeStatus;
}

int FlashIO::read(uint8_t* readBuffer, size_t offset, size_t size)
{
    /* Check flash was actually written */
    if (dataStatus == NO_FLASH_DATA) {
        return KB_ERROR;
    }
    /* Check for null pointer */
    if (readBuffer == NULL) {
        return KB_ERROR;
    }
    /* Read Data from Flash */
    read_flash(readBuffer, offset, size);

    return KB_OK;
}

size_t FlashIO::read(void * ptr, size_t size, size_t count)
{
    if(this->read((uint8_t *)ptr, offset, size * count) != KB_OK) {
        return EOF;
    }
    offset += size * count;
    return count;
}

void FlashIO::open(char *filename, char *mode)
{
    // reset offset. filename doesn't do anything.
    offset = 0;
}

int FlashIO::getchar()
{
    char result;
    if(this->read((uint8_t *)&result, offset, 1) != KB_OK) {
        return EOF;
    }
    offset += 1;
    return (int)result;
}