#ifndef MouseController_h
#define MouseController_h

#include <config_maze.hpp>
#include <Maze.hpp>
#include <PositionController.hpp>
#include <Queue.hpp>

/**
 * @brief      This class it the top level of the mouse control.
 * @details    ===== MOUSE CONTROL PROCEDURE =====
 *  1. call @getDistanceAllCell
 *  2. call @getShortestPath
 *  3. call @moveNextCell
 */
class MouseController : public Maze, public PositionController
{
private:
	Queue<PositionController> pathStack; /**< This is an assistant stack. When @getShortestPath invoked the path to the goal is cunstructed. */
	Queue<PositionController> availablePositionStack; /**< I don't even know what is this. */

	void init();

	inline int getDis(int row, int col)
	{
		return Maze::getDistance(row, col);
	}
	inline int getDis (struct pos_t pos)
	{
		return Maze::getDistance(pos.row, pos.col);
	}
	inline int getDis (PositionController pos)
	{
		pos_t position = pos.getCurrentPos();
		return Maze::getDistance(position.row, position.col);
	}
	inline int getNextDis (PositionController pos, dir_e dirTo)
	{
		PositionController tmp = PositionController(pos.getCurrentPos(),
													dirTo);
		return getDis(tmp.getNextPos());
	}
	inline int getNextDis (PositionController pos)
	{
		return getDis(pos.getNextPos());
	}

	inline int setDis(int row, int col, int dis)
	{
		return Maze::setDistance(row, col, dis);
	}
	inline void setDis (struct pos_t pos, int dis)
	{
		setDis(pos.row, pos.col, dis);
	}

	void initDistance ();
	int getHighestNeighbouringDistance (int row, int col);
	dir_e getDirectionToGo ();
	struct cell getCell(pos_t pos);
	void updateCell();

	void setDirectionToGo ();
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
public:
	MouseController ();

	MouseController (char *filename);

	void getDistanceAllCell ();

	void getShortestPath ();

	void moveNextCell();
	bool isGoal ();
	bool isStart ();

	void printPathStack();
	void printAvailablePositionStack();
};
#endif
