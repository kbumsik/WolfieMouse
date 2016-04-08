#include "PositionController.h"

// prefix (++direction)
Direction&
operator++ (Direction& orig)
{
	orig = static_cast<Direction>(orig + 1); // static_cast required because enum + int -> int
	if (orig > col_minus)
	{
		orig = row_plus;
	}
	return orig;
}

// postfix (direction++)
Direction
operator++ (Direction& orig, int)
{
	Direction rVal = orig;
	++orig;
	return rVal;
}

// prefix (--direction)
Direction&
operator-- (Direction& orig)
{
	orig = static_cast<Direction>(orig - 1); // static_cast required because enum + int -> int
	if (orig < row_plus)
	{
		orig = col_minus;
	}
	return orig;
}

// postfix (direction--)
Direction
operator-- (Direction& orig, int)
{
	Direction rVal = orig;
	--orig;
	return rVal;
}

PositionController::PositionController (int row, int col, direction_e dirTo)
{
	pos.row = row;
	pos.col = col;
	dir = dirTo;
}

PositionController::PositionController (struct position_t pos,
										direction_e dirTo)
{
	PositionController(pos.row, pos.col, dirTo);
}

PositionController::PositionController ()
{
	PositionController(0, 0, mazeDIRECTION_START);
}

void
PositionController::turnRight ()
{
	/* TODO: Do turn operation of the mouse */
	dir--;
}

void
PositionController::turnLeft ()
{
	/* TODO: Do turn operation of the mouse */
	dir++;
}

int
PositionController::goForward ()
{
	switch (dir)
	{
		case row_plus:
			if (mazeIS_ROW_SAFE_TO_MOVE_FORWARD(pos.row))
			{
				/* TODO: Do move operation of the mouse */
				pos.row++;
				return mazeSUCCESS;
			}
		break;
		case col_plus:
			if (mazeIS_COL_SAFE_TO_MOVE_FORWARD(pos.col))
			{
				/* TODO: Do move operation of the mouse */
				pos.col++;
				return mazeSUCCESS;
			}
		break;
		case row_minus:
			if (mazeIS_ROW_SAFE_TO_MOVE_BACKWARD(pos.row))
			{
				/* TODO: Do move operation of the mouse */
				pos.row--;
				return mazeSUCCESS;
			}
		break;
		case col_minus:
			if (mazeIS_COL_SAFE_TO_MOVE_BACKWARD(pos.col))
			{
				/* TODO: Do move operation of the mouse */
				pos.col--;
				return mazeSUCCESS;
			}
		break;
		default:
		break;
	}
	return mazeERROR;
}

position_t
PositionController::getNextPosition (direction_e dirTo)
{
	position_t tmp = pos;
	switch (dirTo)
	{
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
			return tmp;
	}
}

bool
PositionController::isInGoal ()
{
	position_t tmp = getCurrentPosition();
	return (tmp.row == mazeINDEX_GOAL_ROW && tmp.col == mazeINDEX_GOAL_COL) ?
			true : false;
}
