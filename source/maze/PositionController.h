#ifndef PositionController_h
#define PositionController_h

#include "Maze.h"

/**
* @brief direction enumeration
*/
typedef enum Direction {
  eDirError = -1, /* indicate error */
  row_plus = 0,     /* direction in y-increasing way */
  col_plus = 1,     /* direction in x-increasing way */
  row_minus = 2,    /* direction in y-decreasing way */
  col_minus = 3     /* direction in x-decreasing way */
}direction_e;

/**
 * @brief position structure
 */
typedef struct {
  int row; /* y (or row) index */
  int col; /* x (or column) index */
}position_t;

class PositionController {
private:
  position_t pos;           /* current position of the mouse */
  direction_e dir; /* direction of the mouse */

public:
  PositionController(int row, int col, direction_e dirTo);
  PositionController(position_t pos, direction_e dirTo);
  PositionController();
  virtual void turnRight();
  virtual void turnLeft();
  virtual int goForward();
  inline direction_e getCurrentDirection();
  inline position_t getCurrentPosition();
  position_t getNextPosition(direction_e dirTo);
  inline position_t getNextPosition();

  bool isInGoal();
};

// prefix (++direction)
Direction& operator++(Direction& orig)
{
	orig = static_cast<Direction>(orig + 1); // static_cast required because enum + int -> int
	if (orig > col_minus)
	{
		orig = row_plus;
	}
	return orig;
}

// postfix (direction++)
Direction operator++(Direction& orig, int)
{
	Direction rVal = orig;
	++orig;
	return rVal;
}

// prefix (--direction)
Direction& operator--(Direction& orig)
{
	orig = static_cast<Direction>(orig - 1); // static_cast required because enum + int -> int
	if (orig < row_plus)
	{
		orig = col_minus;
	}
	return orig;
}

// postfix (direction--)
Direction operator--(Direction& orig, int)
{
	Direction rVal = orig;
	--orig;
	return rVal;
}

bool operator==(PositionController& lVal, PositionController& rVal)
{
	return (lVal.getCurrentPosition() == rVal.getCurrentPosition());
}

bool operator==(const position_t& lVal, const position_t& rVal)
{
	return ((lVal.row == rVal.row) && (lVal.col == rVal.col));
}
#endif