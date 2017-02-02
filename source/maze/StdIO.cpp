/********************************
 * Name:    StdIO.cpp
 * Author:  Bryant Gonzaga
 * Date Modified:   2 Feb. 2017
 ********************************/

#include "StdIO.hpp"

void StdIO::open(char *filename, char *mode)
{
    file = fopen(filename, mode);
   if (NULL == file) {
        printf("Failed to open file");
   }
}

int StdIO::getchar()
{
    return fgetc(file);
}

size_t StdIO::write(const void *ptr, size_t size, size_t nmemb)
{
    return 0;
}

void StdIO::clearLine()
{
    char buf;
    while ((buf = fgetc(file)) != '\n') {
        if (buf == EOF)
            break;
    }
}
