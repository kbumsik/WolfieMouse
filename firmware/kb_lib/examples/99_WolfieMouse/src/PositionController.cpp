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

void
PositionController::init(int row, int col, dir_e dirTo)
{
	pos.row = row;
	pos.col = col;
	dir = dirTo;
}

PositionController::PositionController (int row, int col, dir_e dirTo)
{
	init(row, col, dirTo);
}

PositionController::PositionController (struct pos_t pos,
										dir_e dirTo)
{
	init(pos.row, pos.col, dirTo);
}

PositionController::PositionController ()
{
	init(0, 0, mazeDIRECTION_START);
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

pos_t
PositionController::getNextPos (dir_e dirTo)
{
	pos_t tmp = pos;
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
			printf("invalid direction!");
			return tmp;
	}
}

dir_e
PositionController::getNextDir (pos_t posTo)
{
	/* get the next position */
	pos_t nextPosition = posTo;
	pos_t pos_delta = nextPosition - getCurrentPos();
	switch (pos_delta.row)
	{
	case 1:
		return row_plus;
	break;
	case -1:
		return row_minus;
	break;
	default:
	break;
	}
	switch (pos_delta.col)
	{
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

void
PositionController::print(PositionController obj)
{
	printf("(%d,%d) ", obj.pos.row, obj.pos.col);
}
