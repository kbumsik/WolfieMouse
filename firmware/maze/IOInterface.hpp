/********************************
 * Name:    IOInterface.hpp
 * Author:  Bryant Gonzaga
 * Date Modified:   2 Feb. 2017
 ********************************/

#ifndef IOINTERFACE_HPP_
#define IOINTERFACE_HPP_

#include <stddef.h>
#include <stdint.h>

class IOInterface
{
public:
    IOInterface(void) {}
    // replacement for fopen()
    virtual void open(char *filename, char *mode) = 0;
    // replacement for fgetc()
    virtual int get_char() = 0;
    // replacement for fwrite()
    virtual size_t write(const void *ptr, size_t size, size_t nmemb) = 0;
};

#endif
