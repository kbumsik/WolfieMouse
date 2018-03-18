#include <common_maze.h>
#include <PositionController.hpp>
#include "Maze.hpp"

/*******************************************************************************
 * Constructor
 ******************************************************************************/
PositionController::PositionController(int row, int col, Direction dirTo) :
        pos({row, col}),
        dir(dirTo)
{
}

PositionController::PositionController(Position pos, Direction dirTo) :
        PositionController(pos.row, pos.col, dirTo)
{
}

PositionController::PositionController() :
        PositionController(0, 0, eDirError)
{
}

/*******************************************************************************
 * Public Methods
 ******************************************************************************/
void PositionController::turnRight()
{
    /* TODO: Do turn operation of the mouse */
    dir--;
}

void PositionController::turnLeft()
{
    /* TODO: Do turn operation of the mouse */
    dir++;
}

int PositionController::goForward()
{
    switch (dir) {
    case row_plus:
        if (MAZE_IS_ROW_SAFE_FORWARD(pos.row)) {
            /* TODO: Do move operation of the mouse */
            pos.row++;
            return COMMON_MAZE_SUCCESS;
        }
        break;
    case col_plus:
        if (MAZE_IS_COL_SAFE_FORWARD(pos.col)) {
            /* TODO: Do move operation of the mouse */
            pos.col++;
            return COMMON_MAZE_SUCCESS;
        }
        break;
    case row_minus:
        if (MAZE_IS_ROW_SAFE_BACKWARD(pos.row)) {
            /* TODO: Do move operation of the mouse */
            pos.row--;
            return COMMON_MAZE_SUCCESS;
        }
        break;
    case col_minus:
        if (MAZE_IS_COL_SAFE_BACKWARD(pos.col)) {
            /* TODO: Do move operation of the mouse */
            pos.col--;
            return COMMON_MAZE_SUCCESS;
        }
        break;
    default:
        break;
    }
    return COMMON_MAZE_ERROR;
}

Direction PositionController::getNextDir(Position posTo)
{
    /* get the next Position */
    Position nextPosition = posTo;
    Position pos_delta = nextPosition - getCurrentPos();
    switch (pos_delta.row) {
    case 1:
        return row_plus;
        break;
    case -1:
        return row_minus;
        break;
    default:
        break;
    }
    switch (pos_delta.col) {
    case 1:
        return col_plus;
        break;
    case -1:
        return col_minus;
        break;
    default:
        break;
    }
    /* return value should not this one */
    /* we can make a strange code ablow to indicate there is an error */
    return eDirError;
}

Position PositionController::getNextPos(Direction dirTo)
{
    Position tmp = pos;
    switch (dirTo) {
    case row_plus:
        tmp.row++;
        return tmp;
    case col_plus:
        tmp.col++;
        return tmp;
    case row_minus:
        tmp.row--;
        return tmp;
    case col_minus:
        tmp.col--;
        return tmp;
    default:
        printf("invalid direction!");
        return tmp;
    }
}

void PositionController::print(PositionController obj)
{
    printf("(%d,%d) ", obj.pos.row, obj.pos.col);
}

/*******************************************************************************
 * Private Methods
 ******************************************************************************/
