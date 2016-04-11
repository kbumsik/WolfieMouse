#include "MouseController.h"

void
MouseController::init()
{
	pathStack = Queue<PositionController>();
	availablePositionStack = Queue<PositionController>();
	/* FIXME: Set the default start point */
	setPos({index_start_row, index_start_col});
	setDir(mazeDIRECTION_START);
	updateCell();
}

MouseController::MouseController ()
: Maze()
{
	init();
}

MouseController::MouseController (char *filename)
: Maze(filename)
{
	init();
}

void
MouseController::initDistance ()
{
	int row;
	int col;
	//Fill the last
	for (row = 0; row < mazeMAX_ROW_SIZE; row++)
	{
		for (col = 0; col < mazeMAX_COL_SIZE; col++)
		{
			setDis(row, col, UNREACHED);
		}
	}
}

void
MouseController::getDistanceAllCell ()
{
	int currentPathDistance = mazeSTART_DISTANCE + 1; /* This is how far the 'water' has flowed */
	int row;
	int col;

	/* set all distance as UNREACHED */
	initDistance();

	/* Firstly set the distance of the current opsition to 0 */
	setDis(getCurrentPos(), mazeSTART_DISTANCE);

	while (1)
	{
		/* Creating a loop which scans the whole maze */
		for (row = 0; row < mazeMAX_ROW_SIZE; row++)
		{
			for (col = 0; col < mazeMAX_COL_SIZE; col++)
			{
				/* If the cell has already been reached, then continue to the next cell */
				if (getDis(row, col) != UNREACHED)
				{
					continue;
				}
				/* If there is a neighbouring cell which has been */
				if (getHighestNeighbouringDistance(row, col) == (currentPathDistance -1))
				{
					/* you have reached the current cell */
					setDis(row, col, currentPathDistance);
				}
			}
		}
		if (getDis(index_goal_row, index_goal_col) != UNREACHED) //If the destination cell has a value after a sweep, the algorithm ends
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
int
MouseController::getHighestNeighbouringDistance (int row, int col)
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
		cmp = Maze::getDistance(row + 1, col);
		if (cmp > tmp)
		{
			tmp = cmp;
		}
	}
	if (getWall(row, col, col_plus) != wall)
	{
		cmp = getDis(row, col + 1);
		if (cmp > tmp)
		{
			tmp = cmp;
		}
	}
	if (getWall(row, col, row_minus) != wall)
	{
		cmp = getDis(row - 1, col);
		if (cmp > tmp)
		{
			tmp = cmp;
		}
	}
	if (getWall(row, col, col_minus) != wall)
	{
		cmp = getDis(row, col - 1);
		if (cmp > tmp)
		{
			tmp = cmp;
		}
	}
	return tmp;
}

void
MouseController::getShortestPath ()
{
	/* init all variables */
	PositionController position;
	bool isFound;
	int currentDistance = 0;
	int i = 0;

	availablePositionStack.init();
	pathStack.init();

	/* set first stack = the current position */
	availablePositionStack.pushToBack(PositionController(getCurrentPos(),getCurrentDir()));

	while (1)
	{
		isFound = false;
		//copy the next position
		position = availablePositionStack.peekFromBack();
		currentDistance = getDis(position);
		//if currentposition is goal, break
		if (position.getCurrentPos() == (pos_t){index_goal_row, index_goal_col})
		{
			isFound = true;
			pathStack.pushToBack(availablePositionStack.popFromBack());
			break;
		}
		/* Now look for next available cells, meaning neighbour cells that has
		 distance of 1 more */
		/* Look around in counter-clockwise */
		for (i = (int) row_plus; i <= (int) col_minus; i++)
		{
			if (getNextDis(position, (dir_e) i)
					== (currentDistance + 1)) /* FIXME: check walls too */
			{
				if (wall == getWall(position.getCurrentPos().row, position.getCurrentPos().col, (dir_e)i))
				{
					continue;
				}
				if (!isFound)
				{
					pathStack.pushToBack(availablePositionStack.popFromBack());
					isFound = true;
				}
				availablePositionStack.pushToBack(
											PositionController(
													position.getNextPos((dir_e) i),
													(dir_e) i));

			}
		}

		if (!isFound)
		{
			/* if no available next cell */
			position = availablePositionStack.popFromBack();
			/* pop pathstack until it meet next availableStack */
			while (!(
					   ( (pathStack.peekFromBack().getNextPos(row_plus) == availablePositionStack.peekFromBack().getCurrentPos()) && (wall != getWall(pathStack.peekFromBack().getCurrentPos().row, pathStack.peekFromBack().getCurrentPos().col, row_plus))  )
					|| ( (pathStack.peekFromBack().getNextPos(col_plus) == availablePositionStack.peekFromBack().getCurrentPos()) && (wall != getWall(pathStack.peekFromBack().getCurrentPos().row, pathStack.peekFromBack().getCurrentPos().col, col_plus))  )
					|| ( (pathStack.peekFromBack().getNextPos(row_minus)== availablePositionStack.peekFromBack().getCurrentPos()) && (wall != getWall(pathStack.peekFromBack().getCurrentPos().row, pathStack.peekFromBack().getCurrentPos().col, row_minus)) )
					|| ( (pathStack.peekFromBack().getNextPos(col_minus)== availablePositionStack.peekFromBack().getCurrentPos()) && (wall != getWall(pathStack.peekFromBack().getCurrentPos().row, pathStack.peekFromBack().getCurrentPos().col, col_minus)) )
					))
			{
				pathStack.popFromBack();
			}
		}
	}
	/* Delete the top element - it is the current position! */
	pathStack.popFromFront();
}

dir_e
MouseController::getDirectionToGo ()
{
	/* get the next position */
	PositionController nextPosition = pathStack.peekFromFront();
	return getNextDir(nextPosition);
}

void
MouseController::setDirectionToGo ()
{
	setDir(getDirectionToGo());
}

bool
MouseController::isGoal ()
{
	return (getCell(getCurrentPos()).isGoal) ?
			true : false;
}

bool
MouseController::isStart ()
{
	return (getCell(getCurrentPos()).isStart) ?
			true : false;
}

void
MouseController::moveNextCell()
{
	/* 1. turn first */
	dir_e tmp_d = getDirectionToGo();
		/* before setting direction we need to set how much trun */
	setDirectionToGo();
	/* 2. scan side wall */
	/* 3. move */
	pos_t tmp_p = getNextPos();
		/* move command */
	setPos(tmp_p);
	/* 4. scan the front wall */
	/* 5. update */
	updateCell();
}


struct cell
MouseController::getCell(pos_t pos)
{
	return Maze::getCell(pos.row, pos.col);
}

void
MouseController::updateCell()
{
	pos_t tmp = getCurrentPos();
	Maze::updateCell();
	Maze::setMouse(tmp.row, tmp.col);
}


void
MouseController::printPathStack()
{
	void (PositionController::*pvFunc)(PositionController) = &PositionController::print;
	printf("pathStack: ");
	pathStack.print(pvFunc);
	printf("\r\n");
}
void
MouseController::printAvailablePositionStack()
{
	printf("availableStack: ");
	availablePositionStack.print(&PositionController::print);
	printf("\r\n");
}
