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
void FakeIO::open(char *filename, char *mode)
{
}

int FakeIO::getchar()
{
    return 1;
}

size_t FakeIO::write(const void *ptr, size_t size, size_t nmemb)
{
    return 1;
}

