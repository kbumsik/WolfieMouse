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
	bool allowFile = false;
public:
	// Constructor
	StdIO(bool allowFile);
    // Overridden methods
    void open(char *filename, char *mode) override;
    int getchar() override;
    size_t write(const void *ptr, size_t size, size_t nmemb) override;
};

#endif /* STDIO_HPP_ */
