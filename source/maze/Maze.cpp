#include "Maze.h"

void Maze::initMaze()
{
  int i = 0;
  int j = 0;
  int k = 0;

  /* init the wall with unknown */
  for (i = 0; i < mazeMAX_ROW_SIZE; i++)
  {
    for (j = 0; j < mazeMAX_COL_SIZE; j++)
    {
      for (k = (int)row_plus; k <= (int)col_minus; k++)
      {
        setWall(i, j, (direction_e)k, unknown);
      }
    }
  }

  /* wrap the wall */
  for (i = 0; i < mazeMAX_ROW_SIZE; i++)
  {
    setWall(i, 0, col_minus, wall);
    setWall(i, mazeMAX_ROW_SIZE-1, col_plus, wall);
  }
  for (j = 0; j < mazeMAX_COL_SIZE; j++)
  {
    setWall(0, j, row_minus, wall);
    setWall(mazeMAX_COL_SIZE-1, j, row_plus, wall);
  }

  /* fill the first cell */
  setWall(0, 0, row_plus, wall);
  setWall(0, 0, col_plus, empty);
  setWall(0, 0, row_minus, wall);
  setWall(0, 0, col_minus, wall);

  /* update the cell */
  updateCell();
}

Maze::Maze() {
  initMaze();
}

wall_e Maze::getWall(int row, int col, direction_e dir)
{
  if (mazeIS_POS_OUT_BOUNDS(row,col))
  {
    return (wall_e)mazeERROR;
  }
  switch (dir)
  {
  case row_plus:
    return rowWall[row+1][col];
  case col_plus:
    return colWall[row][col+1];
  case row_minus:
    return rowWall[row][col];
  case col_minus:
    return colWall[row][col];
  default:
    return (wall_e)mazeERROR;
  }
  return (wall_e)mazeERROR;
}

cell_e Maze::getCell(int row, int col)
{
  return mazeIS_POS_OUT_BOUNDS(row,col)? (cell_e)mazeERROR : cell[row][col];
}

int Maze::setWall(int row, int col, direction_e dir, wall_e status)
{
  if (mazeIS_POS_OUT_BOUNDS(row,col))
  {
    return mazeERROR;
  }
  switch (dir)
  {
  case row_plus:
    rowWall[row+1][col] = status;
    return mazeSUCCESS;
  case col_plus:
    colWall[row][col+1] = status;
    return mazeSUCCESS;
  case row_minus:
    rowWall[row][col] = status;
    return mazeSUCCESS;
  case col_minus:
    colWall[row][col] = status;
    return mazeSUCCESS;
  default:
    return mazeERROR;
  }
}

int Maze::updateCell(int row, int col)
{
  if (mazeIS_POS_OUT_BOUNDS(row,col))
  {
    return mazeERROR;
  }
  if(Maze::getWall(row, col, row_plus) != unknown &&
     Maze::getWall(row, col, col_plus) != unknown &&
     Maze::getWall(row, col, row_minus) != unknown &&
     Maze::getWall(row, col, col_minus) != unknown)
  {
    cell[row][col] = searched;
  }
  else
  {
    cell[row][col] = unsearched;
  }
  return mazeSUCCESS;
}

void Maze::updateCell()
{
  int i = 0;
  int j = 0;
  for (i = 0; i < mazeMAX_ROW_SIZE; i++)
  {
    for (j = 0; j < mazeMAX_COL_SIZE; j++)
    {
      Maze::updateCell(i,j);
    }
  }
}