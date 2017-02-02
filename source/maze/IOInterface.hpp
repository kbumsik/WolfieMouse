/********************************
 * Name:    IOInterface.hpp
 * Author:  Bryant Gonzaga
 * Date Modified:   2 Feb. 2017
 ********************************/

#ifndef IOINTERFACE_HPP_
#define IOINTERFACE_HPP_

#include <stdio.h>
#include <stddef.h>

class IOInterface
{
protected:
    FILE *file;
public:
    // replacement for fopen()
    virtual void open(char *filename, char *mode) = 0;
    // replacement for fgetc()
    virtual int getchar() = 0;
    // replacement for fwrite()
    virtual size_t write(const void *ptr, size_t size, size_t nmemb) = 0;
};

#endif
