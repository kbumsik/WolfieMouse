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
        dataStatus = FlashIO::NO_DATA;
    } else {
        dataStatus = FlashIO::DATA_STORED;
    }
}

size_t FlashIO::write(const void *ptr, size_t size, size_t count)
{
    uint32_t writeStatus = write_flash((uint8_t*) ptr, size * count);
    /* If every thing is ok */
    if (writeStatus == KB_OK) {
        dataStatus = FlashIO::DATA_STORED;
        return KB_OK;
    }
    return writeStatus;
}

int FlashIO::read(uint8_t* readBuffer, size_t offset, size_t size)
{
    /* Check flash was actually written */
    if (dataStatus == FlashIO::NO_DATA) {
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

int FlashIO::open(char *filename, char *mode)
{
    // When writing, it is always return yes
    if ((mode != NULL) && (*mode == 'w')) {
        return 0;
    }
    // reset offset. filename doesn't do anything.
    offset = 0;

    if (FlashIO::DATA_STORED == dataStatus) {
        return 0;
    } else {
        return -1; // There is no data in flash yet
    }
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