/********************************
 * Name:    StdIO.cc
 * Author:  Bryant Gonzaga
 * Date Modified:   2 Feb. 2017
 ********************************/

#include "StdIO.hpp"

/*******************************************************************************
 * Constructor
 ******************************************************************************/
StdIO::StdIO(bool allowFile) :
	IOInterface(),
	allowFile(allowFile)
{
    // The initializer does it all
}

/*******************************************************************************
 * Public Methods
 ******************************************************************************/
void StdIO::open(char *filename, char *mode)
{
	if (!allowFile) {
    	printf("This StdIO does not allow file");
		return;
	}
    _file = fopen(filename, mode);
    if (NULL == _file) {
    	printf("Failed to open file");
    }
}

int StdIO::getchar()
{
	if (allowFile) {
	    return fgetc(_file);
	} else {
		return fgetc(stdin);
	}
}

size_t StdIO::write(const void *ptr, size_t size, size_t nmemb)
{
	if (allowFile) {
	    return fwrite(ptr, size, nmemb, _file);
	} else {
	    return fwrite(ptr, size, nmemb, stdout);
	}
}
