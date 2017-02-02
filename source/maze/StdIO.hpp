/********************************
 * Name:    StdIO.hpp
 * Author:  Bryant Gonzaga
 * Date Modified:   2 Feb. 2017
 ********************************/

#ifndef STDIO_HPP_
#define STDIO_HPP_

#include "IOInterface.hpp"
#include <stdio.h>
#include <stddef.h>

class StdIO: public IOInterface
{
public:
    //Overridden methods
    void open(char *filename, char *mode);
    int getchar();
    size_t write(const void *ptr, size_t size, size_t nmemb);
    //other methods
    void clearLine();
};

#endif /* STDIO_HPP_ */
