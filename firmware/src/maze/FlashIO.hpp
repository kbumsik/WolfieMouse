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

#define NO_FLASH_DATA   0x01    /* No Data in flash */
#define NEW_FLASH_DATA  0x02    /* Flash programmed */ 

class FlashIO: public IOInterface
{
private:
    uint32_t sizeOfBuffer;
    uint8_t dataStatus;

public:
    /**
     * Class Constructor
     */
    FlashIO();

    /**
     * Class Deconstructor
     */
    ~FlashIO();

    /**
     * This function writes the data given to it into flash memory.
     * Since flash must be erased before writing to it, this function
     * will write over previously written data. If you would like to 
     * simply erase the reserved sector of flash then you
     * 
     * @param ptr
     * @param size
     * @param nmemb
     * @return status code indicating if this were written ok
     */
    size_t write(const void* ptr, size_t size, size_t nmemb);

    /**
     * This function reads from flash and stores the contents in
     * readBuffer. Furthermore, it makes sure that the size of the
     * buffer matches the number of bytes written to flash.
     * @param readBuffer    pointer to array to hold data
     * @param size          size of buffer
     */
    size_t read(uint8_t* readBuffer, uint32_t size);

    /* Overidden Methods */
    void open(char *filename, char *mode);
    int getchar();
};

#endif /* FLASH_IO_H_ */
