/*
 * position_struct.h
 *
 *  Created on: Jan 16, 2017
 *      Author: vagrant
 */

#ifndef POSITION_HPP_
#define POSITION_HPP_

/**
 * @brief position structure
 */
struct Position
{
    int row; /* y (or row) index */
    int col; /* x (or column) index */

    Position operator-(const Position& rVal)
    {
        Position result;
        result.col = col - rVal.col;
        result.row = row - rVal.row;
        return result;
    }

    bool operator==(const Position& rVal)
    {
        return ((row == rVal.row) && (col == rVal.col));
    }
};

#endif /* POSITION_HPP_ */
