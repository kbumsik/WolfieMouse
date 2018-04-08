/*
 * fakeIO.cc
 *
 *  Created on: Apr 7, 2017
 *      Author: kbumsik
 */



#include "fakeIO.hpp"

/*******************************************************************************
 * Constructor
 ******************************************************************************/
FakeIO::FakeIO()
{
    // The initializer does it all
}

/*******************************************************************************
 * Public Methods
 ******************************************************************************/
int FakeIO::open(char *filename, char *mode)
{
    return 0;
}

int FakeIO::getchar()
{
    return EOF;
}

size_t FakeIO::read(void *ptr, size_t size, size_t count)
{
    return count;
}

size_t FakeIO::write(const void *ptr, size_t size, size_t count)
{
    return count;
}

