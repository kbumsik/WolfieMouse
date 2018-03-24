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
    dataStatus = NO_DATA_WRT;
    flashBuffer = NULL;
    sizeOfBuffer = 0;
    index = 0;
}

size_t FlashIO::write(const void *ptr, size_t size, size_t nmemb)
{
    sizeOfBuffer = nmemb * size;
    uint32_t writeStatus = write_flash( (uint8_t*) ptr, sizeOfBuffer );
    if (writeStatus == HAL_OK) {
        
    }
     return
}

size_t FlashIO::read()
{
    if (flashBuffer == NULL) {
        flashBuffer = malloc(sizeof(uint8_t) * sizeOfBuffer);
    }
    read_flash(flashBuffer, sizeOfBuffer);
}

FlashIO::getchar()
{
    if (flashBuffer == NULL) {
        return KB_ERROR;
    }
    if (index >= sizeOfBuffer) {
        return KB_ERROR;
    }
     return flashBuffer[index++];
}
