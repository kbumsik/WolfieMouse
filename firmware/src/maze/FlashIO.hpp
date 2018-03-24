/*********************************************************************
 * File:        FlashIO.hpp
 * Author:      Bryant Gonzaga
 * Created:     3/23/2018
 * Modified:    3/23/2018
 *
 * Notes:
 *
 * Description:
 *  
 ********************************************************************/

#ifndef FLASH_IO_H_
#define FLASH_IO_H_

#include "IOInterface.hpp"
#include "flash.h"

#define NO_DATA_WRT     0x01
#define NEW_DATA_WRT    0x02
#define BUFFER_WRT      0x04

class FlashIO: public IOInterface
{
private:
    uint8_t* flashBuffer;
    uint32_t sizeOfBuffer;
    uint32_t index;
    uint8_t dataStatus;

public:
    /**
     * Class constructor
     */
    FlashIO();

    /**
     * This function writes the data given to it into flash memory.
     * Since flash must be erased before writing to it, this function
     * will write over previously written data. If you would like to 
     * simply erase the reserved sector of flash then you
     * @param ptr
     * @param size
     * @param nmemb
     * @return status code indicating if this were written ok
     */
    size_t write(const void* ptr, size_t size, size_t nmemb);

    /**
     * This function reads from the flash and stores the data in
     * flash_buffer. Since flash.h only provides a function for
     * reading blocks of flash, then this function should be called
     * first, before calling getchar(). Furthermore, since this
     * data in flash will be stored by write() then we already know
     * exactly how much data was stored in flash.
     * @return
     */
     size_t read();

    /**
     * This function will return a character from the flash_buffer.
     * It will also increase the index by one. Therefore, one could
     * read the whole buffer by calling this function in a loop.
     * @return character from flash_buffer
     */
    int getchar();

    /* Overidden Methods */
    void open(char *filename, char *mode);
};

#endif /* FLASH_IO_H_ */
