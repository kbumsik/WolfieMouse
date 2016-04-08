#ifndef MouseController_h
#define MouseController_h

#include "config_maze.h"
#include "Maze.h"
#include "PositionController.h"
#include "Queue.h"

class MouseController : Maze, PositionController{

private:
  Queue<PositionController> pathStack;				/**< This is an assistant stack. When @getShortestPath invoked the path to the goal is cunstructed. */
  Queue<PositionController> availablePositionStack;	/**< I don't even know what is this. */
  int distance[mazeMAX_ROW_SIZE][mazeMAX_COL_SIZE]; /**< To store cost of moving of each cell */
  
  inline int getDistance(int row, int col){
	if (mazeIS_POS_OUT_BOUNDS(row, col))
	{
		return mazeERROR;
	}
	return distance[row][col];
  }
  inline int getDistance(struct position_t pos){
	return getDistance(pos.row, pos.col);
  }
  inline int getDistance(PositionController pos){
	position_t position = pos.getCurrentPosition();
	return getDistance(position.row, position.col);
  }
  inline int getNextDistance(PositionController pos, direction_e dirTo){
	PositionController tmp = PositionController(pos.getCurrentPosition(), dirTo);
	return getDistance(tmp.getNextPosition());
  }
  inline int getNextDistance(PositionController pos){
    return getDistance(pos.getNextPosition());
  }

  inline void setDistance(int row, int col, int dis){
    /* TODO: Wrap it with max? */
    distance[row][col] = dis;
  }

  inline void setDistance(struct position_t pos, int dis)
  {
    setDistance(pos.row, pos.col, dis);
  }
 
  void initDistance();
  int getHighestNeighbouringDistance(int row, int col);

public:
  MouseController();
  /* I make it */
  /* /I make it */

  void getDistanceAllCell();

  void getShortestPath();

  direction_e getDirectionToGo();

  void setDirectionToGo();

  /** On development
  void moveNextCell();
  void movingCompleted();

  Cell* getNextCell();

  virtual void turnRight();
  virtual void turnLeft();
  virtual void goForward();

  virtual void setWall(int row, int col, direction_e dir, wall_e status);
  virtual void updateCell(int row, int col);
  */
};
#endif
