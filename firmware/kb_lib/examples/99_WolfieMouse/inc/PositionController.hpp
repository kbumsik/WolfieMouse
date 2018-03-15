#ifndef PositionController_h
#define PositionController_h

#include <Maze.hpp>

class PositionController
{
private:
	struct pos_t pos; /**< current position of the mouse */
	dir_e dir; /**< direction of the mouse */
	void init(int row, int col, dir_e dirTo);

public:
	PositionController (int row, int col, dir_e dirTo);
	PositionController (struct pos_t pos, dir_e dirTo);
	PositionController ();
	virtual void turnRight ();
	virtual void turnLeft ();
	virtual int goForward ();

	inline void setDir (dir_e dirToSet)
	{
		dir = dirToSet;
	}

	inline void setPos (pos_t posToSet)
	{
		pos = posToSet;
	}

	inline dir_e getCurrentDir ()
	{
		return dir;
	}
	inline pos_t getCurrentPos ()
	{
		return pos;
	}
	pos_t getNextPos (dir_e dirTo);

	inline pos_t getNextPos ()
	{
		return PositionController::getNextPos(dir);
	}

	dir_e getNextDir (pos_t posTo);

	inline dir_e getNextDir (PositionController posTo)
	{
		return getNextDir (posTo.getCurrentPos());
	}
	bool operator== (PositionController& rVal)
	{
		return (getCurrentPos() == rVal.getCurrentPos());
	}

	void print(PositionController obj);
};

#endif
