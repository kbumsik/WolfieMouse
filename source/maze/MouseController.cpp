#include <common_maze.h>
#include <MouseController.hpp>

/*******************************************************************************
 * Constructor
 ******************************************************************************/
MouseController::MouseController() :
        MouseController(NULL, NULL, NULL, NULL, NULL)
{
}

MouseController::MouseController(char *filename, IOInterface *fileIO,
            IOInterface *printIO, FinderInterface *finder, MoverInterface *mover) :
        Maze(filename, fileIO, printIO),
		finder(finder),
		mover(mover)
{
    pathStack = Queue<PositionController>();
    availablePositionStack = Queue<PositionController>();
    /* FIXME: Set the default start point */
    setPos(startPos);
    setDir(CONFIG_DEFAULT_START_DIRECTION);
    updateCell();
}

/*******************************************************************************
 * Public Methods
 ******************************************************************************/
void MouseController::scanWalls(void)
{
    Position curPos = getCurrentPos();
    Direction curDir = getCurrentDir();
    Position nextPos = getNextPos();
    // Firstly detect a wall right in front of the mouse
    Wall result = finder->examineWall(curPos, curDir, *this);
    setWall(curPos, curDir, result);
    // Then search walls of facing cells 
    for (int i = (int) row_plus; i <= (int) col_minus; i++) {
    	Direction dir = (Direction) i;
        result = finder->examineWall(nextPos, dir, *this);
        setWall(nextPos, dir, result);
    }
    // Lastly, update state of cells.
    updateCell(curPos);
    updateCell(nextPos);
}

void MouseController::getDistanceAllCell()
{
    int currentPathDistance = CELL_DISTANCE_START + 1;  //This is how far the 'water' has flowed
    int row;
    int col;
    /* set all distance as UNREACHED */
    initDistance();

    /* Firstly set the distance of the current position to 0 */
    setDis(getCurrentPos(), CELL_DISTANCE_START);

    while (1) {
        /* Creating a loop which scans the whole maze */
        for (row = 0; row < CONFIG_MAX_ROW_SIZE; row++) {
            for (col = 0; col < CONFIG_MAX_COL_SIZE; col++) {
                /* If the cell has already been reached, then continue to the next cell */
                if (getDis(row, col) != CELL_DISTANCE_UNREACHED) {
                    continue;
                }
                /* If there is a neighboring cell which has been */
                if (getHighestNeighbouringDistance(row, col)
                        == (currentPathDistance - 1)) {
                    /* you have reached the current cell */
                    setDis(row, col, currentPathDistance);
                }
            }
        }
        if ((getDis(goalPos[0].row, goalPos[0].col) != CELL_DISTANCE_UNREACHED) ||
        	(getDis(goalPos[1].row, goalPos[1].col) != CELL_DISTANCE_UNREACHED) ||
			(getDis(goalPos[2].row, goalPos[2].col) != CELL_DISTANCE_UNREACHED) ||
			(getDis(goalPos[3].row, goalPos[3].col) != CELL_DISTANCE_UNREACHED) ) {
            break; //If the destination cell has a value after a sweep, the algorithm ends
        }
        /* Increment the distance because we are scanning again. */
        currentPathDistance++;
    }
    /**
     * The highestNeighbouringCell(x,y) function returns the value of the highest,
     * accessible (ie there are no separating walls) neighboring cell. Initially,
     * all the cells are given a value of UNREACHED(which is -1), except the
     * micromouse's current cell, which is given value 0.
     * Then the grid of cell values is scanned. On the first scan, only the cells
     * next to, and accessible to the cell where the micromouse is, will be given
     * a (non-UNREACHED) value. That value is 1. This is repeated until the
     * destination cell has been given a (non-UNREACHED) value.
     */
}

void MouseController::getShortestPath()
{
	/* init all variables */
	PositionController position;
	bool isFound;
	int currentDistance = 0;
	int i = 0;

	availablePositionStack.init();
	pathStack.init();

	/* set first stack = the current position */
	pathStack.pushToBack(PositionController(getCurrentPos(),getCurrentDir()));

	while (1) {
        isFound = false;
        //copy the next position
        position = pathStack.peekFromBack();
        currentDistance = getDis(position);
        //if current position is goal, break
        if ((position.getCurrentPos() == Position {goalPos[0].row, goalPos[0].col}) ||
        	(position.getCurrentPos() == Position {goalPos[1].row, goalPos[1].col}) ||
			(position.getCurrentPos() == Position {goalPos[2].row, goalPos[2].col}) ||
			(position.getCurrentPos() == Position {goalPos[3].row, goalPos[3].col}) ){
            isFound = true;
            //pathStack.pushToBack(availablePositionStack.popFromBack());
            break;
        }
        /* Now look for next available cells, meaning neighboring cells that has
         distance of 1 more */
        /* Look around in counter-clockwise */
        for (i = (int) row_plus; i <= (int) col_minus; i++) {
        	Direction dir = (Direction) i;
            if (getNextDis(position, dir) == (currentDistance + 1)){
                // Check if that cell is reachable. In other words, check if the
                // cell is not blocked by a wall.
                if (wall == getWall(position.getCurrentPos(), dir)) {
                    continue;
                }
                if (!isFound) {
                    isFound = true;
                }
                availablePositionStack.pushToBack(
                		PositionController(position.getNextPos(dir), dir));
            }
        }
		if (!isFound) {
			/* When not found available next cell?
			 * pop(pathStack) until the peek(pathStack) is adjacent to peek(availablePathStack)
			 * Should also check if they are adjacent distance.
			 */
			bool isAdjacent;
			bool isNextDistance;
			PositionController posFromPath;
			PositionController posFromAvailable;
			do {
				pathStack.popFromBack();
				posFromPath = pathStack.peekFromBack();
				posFromAvailable = availablePositionStack.peekFromBack();
				isAdjacent =
						(  (posFromPath.getNextPos(row_plus)  == posFromAvailable.getCurrentPos())
						|| (posFromPath.getNextPos(col_plus)  == posFromAvailable.getCurrentPos())
						|| (posFromPath.getNextPos(row_minus) == posFromAvailable.getCurrentPos())
						|| (posFromPath.getNextPos(col_minus) == posFromAvailable.getCurrentPos())
						);
				isNextDistance = (getDis(posFromPath) + 1) == getDis(posFromAvailable);
			} while (!(isAdjacent && isNextDistance));
		}
        pathStack.pushToBack(availablePositionStack.popFromBack());
	}
	/* Delete the top element - it is the current position! */
	// FIXME: Do not delete it?
	// FIXME: When not poping, the Front is always (0,0)
	pathStack.popFromFront();
}

void MouseController::moveNextCell()
{
    /* 1. turn first */
    Direction tmp_d = getDirectionToGo();
    /* before setting direction we need to set how much trun */
    setDirectionToGo();
    /* 2. scan side wall */
    scanWalls();		// TODO: Try using this outside
    /* 3. move */
    Position tmp_p = getNextPos();
    /* move command */
    setPos(tmp_p);
    /* 4. scan the front wall */
    /* 5. update */
    updateCell();
}

bool MouseController::isInGoal()
{
    return (getCell(getCurrentPos()).attribute == goal) ? true : false;
}

bool MouseController::isInStart()
{
    return (getCell(getCurrentPos()).attribute == start) ? true : false;
}

void MouseController::printMaze()
{
    Maze::updateCell();
    mazeIO.setMousePosition(*this);
    Maze::printMaze();
}

void MouseController::printPathStack()
{
    void (PositionController::*pvFunc)(
            PositionController) = &PositionController::print;
    printf("pathStack: ");
    pathStack.print(pvFunc);
    printf("\n");
}

void MouseController::printAvailablePositionStack()
{
    printf("availableStack: ");
    availablePositionStack.print(&PositionController::print);
    printf("\n");
}

void MouseController::turnRight()
{
	/* Use MovingInterface */

	/* Call parent method */
	PositionController::turnRight();
}

void MouseController::turnLeft()
{
	/* Use MovingInterface */

	/* Call parent method */
	PositionController::turnLeft();
}

int MouseController::goForward()
{
	/* Call parent method */
	int ret = PositionController::goForward();
	/* Check if it is really OK to move */
	if (ret != COMMON_MAZE_SUCCESS) {
		return ret;
	}
	/* Use MovingInterface */
	// TODO: What if the mouse actually failed to move??
	return COMMON_MAZE_SUCCESS;
}
/*******************************************************************************
 * Private Methods
 ******************************************************************************/
Cell MouseController::getCell(Position pos)
{
    return Maze::getCell(pos.row, pos.col);
}


void MouseController::initDistance()
{
    int row;
    int col;
    //Fill the last
    for (row = 0; row < CONFIG_MAX_ROW_SIZE; row++) {
        for (col = 0; col < CONFIG_MAX_COL_SIZE; col++) {
            setDis(row, col, CELL_DISTANCE_UNREACHED);
        }
    }
}

/* TODO: Could be a bottleneck */
int MouseController::getHighestNeighbouringDistance(int row, int col)
{
    int tmp = CELL_DISTANCE_UNREACHED;
    int cmp;
    /* Check out of bounds first */
    if (MAZE_IS_POS_OUT_BOUNDS(row, col)) {
        return COMMON_MAZE_ERROR;
    }

    if (getWall(row, col, row_plus) != wall) {
        cmp = Maze::getDistance(row + 1, col);
        if (cmp > tmp) {
            tmp = cmp;
        }
    }
    if (getWall(row, col, col_plus) != wall) {
        cmp = getDis(row, col + 1);
        if (cmp > tmp) {
            tmp = cmp;
        }
    }
    if (getWall(row, col, row_minus) != wall) {
        cmp = getDis(row - 1, col);
        if (cmp > tmp) {
            tmp = cmp;
        }
    }
    if (getWall(row, col, col_minus) != wall) {
        cmp = getDis(row, col - 1);
        if (cmp > tmp) {
            tmp = cmp;
        }
    }
    return tmp;
}

Direction MouseController::getDirectionToGo()
{
    /* get the next position */
    PositionController nextPosition = pathStack.peekFromFront();
    return getNextDir(nextPosition);
}

void MouseController::setDirectionToGo ()
{
	setDir(getDirectionToGo());
}
