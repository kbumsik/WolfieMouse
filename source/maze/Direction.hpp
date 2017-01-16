/*
 * Direction.h
 *
 *  Created on: Jan 16, 2017
 *      Author: vagrant
 */

#ifndef DIRECTION_HPP_
#define DIRECTION_HPP_

/**
 * @brief direction enumeration
 */
enum Direction
{
    eDirError = -1, /* indicate error */
    row_plus = 0, /* direction in y-increasing way */
    col_plus = 1, /* direction in x-increasing way */
    row_minus = 2, /* direction in y-decreasing way */
    col_minus = 3 /* direction in x-decreasing way */
};

// prefix (++direction)
inline Direction& operator++(Direction& orig)
{
    orig = static_cast<Direction>(orig + 1); // static_cast required because + int -> int
    if (orig > col_minus) {
        orig = row_plus;
    }
    return orig;
}

// postfix (direction++)
inline Direction operator++(Direction& orig, int)
{
    Direction rVal = orig;
    ++orig;
    return rVal;
}

// prefix (--direction)
inline Direction& operator--(Direction& orig)
{
    orig = static_cast<Direction>(orig - 1); // static_cast required because + int -> int
    if (orig < row_plus) {
        orig = col_minus;
    }
    return orig;
}

// postfix (direction--)
inline Direction operator--(Direction& orig, int)
{
    enum Direction rVal = orig;
    --orig;
    return rVal;
}

#endif /* DIRECTION_HPP_ */
