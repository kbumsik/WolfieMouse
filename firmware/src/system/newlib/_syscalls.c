/**
 * @brief system call implementation for newlib
 * 
 * @file _syscalls.c
 * @author Bumsik Kim (k.bumsik@gmail.com)
 * @date 2018-04-08
 */

#include "terminal.h"

int _write(int file, char *ptr, int len) {
    int todo;
    static char str[2] = "\0\0";
    
    for (todo = 0; todo < len; todo++) {
        str[0] = *ptr++;
        terminal_puts(str);
    }
    return len;
}