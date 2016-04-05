#include "MouseController.h"


MouseController::MouseController(){
  pathStack = Queue<PositionController>();
  availablePositionStack = Queue<PositionController>();
}

inline int MouseController::getDistance(int row, int col)
{
  if (mazeIS_POS_OUT_BOUNDS(row, col))
  {
    return mazeERROR;
  }
  return distance[row][col];
}

inline int MouseController::getDistance(struct position_t pos)
{
  return getDistance(pos.row, pos.col);
}

inline int MouseController::getDistance(PositionController pos)
{
  position_t position = pos.getCurrentPosition();
  return getDistance(position.row, position.col);
}

inline int MouseController::getNextDistance(PositionController pos, direction_e dirTo)
{
  PositionController tmp = PositionController(pos.getCurrentPosition(), dirTo);
  return getDistance(tmp.getNextPosition());
}
inline int MouseController::getNextDistance(PositionController pos)
{
  return getDistance(pos.getNextPosition());
}


inline void MouseController::setDistance(int row, int col, int dis)
{
  /* TODO: Wrap it with max? */
  distance[row][col] = dis;
}

inline void MouseController::setDistance(struct position_t pos, int dis)
{
  setDistance(pos.row, pos.col, dis);
}

void MouseController::initDistance()
{
  int row;
  int col;
  //Fill the last
  for (row = 0; row < mazeMAX_ROW_SIZE; row++)
  {
    for (col = 0; col < mazeMAX_COL_SIZE; col++)
    {
      setDistance(row, col, UNREACHED);
    }
  }
}

void MouseController::getDistanceAllCell()
{
  int currentPathDistance = mazeSTART_DISTANCE + 1; /* This is how far the 'water' has flowed */
  int row;
  int col;
  
  /* set all distance as UNREACHED */
  initDistance();
  
  /* Firstly set the distance of the current opsition to 0 */
  setDistance(getCurrentPosition(), mazeSTART_DISTANCE);

  while (1)
  {
    /* Creating a loop which scans the whole maze */
    for (row = 0; row <= mazeMAX_ROW_SIZE; row++)
    { 
      for (col = 0 ; col <= mazeMAX_COL_SIZE; col++)
      {
      /* If the cell has already been reached, then continue to the next cell */
        if (getDistance(row, col) != UNREACHED)
        {
          continue;
        }
        /* If there is a neighbouring cell which has been */
        if (getHighestNeighbouringDistance(row, col) != UNREACHED)
        {
          /* you have reached the current cell */
          setDistance(row, col, currentPathDistance);
        }
      }
    }
    if (getDistance(mazeINDEX_GOAL_ROW, mazeINDEX_GOAL_COL) != UNREACHED) //If the destination cell has a value after a sweep, the algorithm ends
    {
      break;
    }
    /* Increment the distance because we are scanning again. */
    currentPathDistance++;
  }
  /**
   * The highestNeighbouringCell(x,y) function returns the value of the highest,
   * accessible (ie there are no separating walls) neighbouring cell. Initially,
   * all the cells are given a value of UNREACHED(which is -1), except the 
   * micromouse's current cell, which is given value 0. 
   * Then the grid of cell values is scanned. On the first scan, only the cells 
   * next to, and accessible to the cellwhere the micromouse is, will be given 
   * a (non-UNREACHED) value. That value is 1. This is repeated untill the 
   * destination cell has been given a (non-UNREACHED) value.
   */
}

/* TODO: Could be a bottleneck */
int MouseController::getHighestNeighbouringDistance(int row, int col)
{
  int tmp = UNREACHED;
  int cmp;
  /* Check out of bounds first */
  if (mazeIS_POS_OUT_BOUNDS(row, col))
  {
    return mazeERROR;
  }

  if (getWall(row, col, row_plus) != wall)
  {
    cmp = getDistance(row + 1, col);
    if (cmp > tmp)
    {
      tmp = cmp;
    }
  }
  if (getWall(row, col, col_plus) != wall)
  {
    cmp = getDistance(row, col + 1);
    if (cmp > tmp)
    {
      tmp = cmp;
    }
  }
  if (getWall(row, col, row_minus) != wall)
  {
    cmp = getDistance(row - 1, col);
    if (cmp > tmp)
    {
      tmp = cmp;
    }
  }
  if (getWall(row, col, col_minus) != wall)
  {
    cmp = getDistance(row, col - 1);
    if (cmp > tmp)
    {
      tmp = cmp;
    }
  }
  return tmp;
}


void MouseController::getShortestPath()
{
  /* init all variables */
  PositionController position(getCurrentPosition(), getCurrentDirection());
  bool isFound;
  int currentDistance = 0;
  int i = 0;

  availablePositionStack.init();
  pathStack.init();

  /* set first stack = the current position */
  availablePositionStack.pushToBack(position);

  while (1)
  {
    isFound = false;
    //copy the next position
    position = availablePositionStack.peekFromBack();
    currentDistance = getDistance(position);
    //if currentposition is goal, break
    if (position.isInGoal())
    {
      isFound = true;
      pathStack.pushToBack(availablePositionStack.popFromBack());
      break;
    }
    /* Now look for next available cells, meaning neighbour cells that has 
      distance of 1 more */
    /* Look around in counter-clockwise */
    for (i = (int)row_plus; i <= (int)col_minus; i++)
    {
      if(getNextDistance(position, (direction_e)i) == (currentDistance + 1))
      {
        if(!isFound)
        {
          pathStack.pushToBack(availablePositionStack.popFromBack());
          isFound = true;
        }
        availablePositionStack.pushToBack(PositionController(position.getNextPosition((direction_e)i), (direction_e)i));
      }
    }

    if (!isFound)
    { 
      /* if no available next cell */
      availablePositionStack.popFromBack();
      /* pop pathstack until it meet next availableStack */
      while (!(
        (pathStack.peekFromBack().getNextPosition(row_plus) == availablePositionStack.peekFromBack().getCurrentPosition())||
        (pathStack.peekFromBack().getNextPosition(col_plus) == availablePositionStack.peekFromBack().getCurrentPosition())||
        (pathStack.peekFromBack().getNextPosition(row_minus)== availablePositionStack.peekFromBack().getCurrentPosition())||
        (pathStack.peekFromBack().getNextPosition(col_minus)== availablePositionStack.peekFromBack().getCurrentPosition())
        ))
      {
        pathStack.popFromBack();
      }
    }
  }
  /* Delete the top element - it is the current position! */
  pathStack.popFromFront();
}

direction_e MouseController::getNextDirection()
{
  /* get the next position */
  PositionController nextPosition = pathStack.peekFromFront();
  position_t pos_delta = nextPosition.getCurrentPosition() - this->getCurrentPosition();
  switch (pos_delta.row)
  {
  case 1 : return row_plus;
    break;
  case -1: return row_minus;
    break;
  default:
    break;
  }
  switch (pos_delta.col)
  {
  case 1: return col_plus;
    break;
  case -1: return col_minus;
    break;
  default:
    break;
  }
  /* return value should not this one */
  /* we can make a strange code ablow to indicate there is an error */
  return row_plus;
}

