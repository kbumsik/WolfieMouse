#include "PositionController.h"

PositionController::PositionController(int row, int col, direction_e dirTo)
{
  pos.row = row;
  pos.col = col;
  dir = dirTo;
}

PositionController::PositionController(position_t pos, direction_e dirTo)
{
  PositionController(pos.row, pos.col, dirTo);
}

PositionController::PositionController()
{
  PositionController(0, 0, mazeDIRECTION_START);
}

void PositionController::turnRight() {
  /* TODO: Do turn operation of the mouse */
  dir--;
}

void PositionController::turnLeft() {
  /* TODO: Do turn operation of the mouse */
  dir++;
}

int PositionController::goForward() {
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

inline direction_e PositionController::getCurrentDirection()
{
  return dir;
}

inline position_t PositionController::getCurrentPosition()
{
  return pos;
}

position_t PositionController::getNextPosition(direction_e dirTo) {
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

inline position_t PositionController::getNextPosition() {
  return PositionController::getNextPosition(dir);
}

bool PositionController::isInGoal()
{
  position_t tmp = getCurrentPosition();
  return (tmp.row == mazeINDEX_GOAL_ROW && tmp.col == mazeINDEX_GOAL_COL)? true : false;
}