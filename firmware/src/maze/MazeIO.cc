/********************************
 * Name:    IOInterface.hpp
 * Author:  Bumsik Kim; Bryant Gonzaga
 * Date Modified:   2 Feb. 2017
 ********************************/

#include "MazeIO.hpp"
#include "Maze.hpp"

/** FIXME: dynamically decide the starting direction */

/*******************************************************************************
 * Constructor
 ******************************************************************************/
MazeIO::MazeIO(Maze *mazePtr,  IOInterface *fileIO, IOInterface *printIO) :
    maze(mazePtr),
    maxRowSize(CONFIG_MAX_ROW_SIZE),
    maxColSize(CONFIG_MAX_COL_SIZE),
	fileIO(fileIO),
	printIO(printIO)
{
    // The initializer does it all
}

/*******************************************************************************
 * Public Methods
 ******************************************************************************/
Position MazeIO::getMousePosition(void)
{
    return mousePosition.getCurrentPos();
}

void MazeIO::setMousePosition(Position pos)
{
    mousePosition = PositionController(pos, eDirError);
}

void MazeIO::setMousePosition(PositionController posCon)
{
	mousePosition = posCon;
}

void MazeIO::setDestinations(const std::vector<Position> &des)
{
	destinations = des;
}

bool MazeIO::positionIsDestination(Position pos)
{
	for (int i = 0; i < destinations.size(); i++) {
		if (pos == destinations[i]) {
			return true; /* return true if position found in destination vector*/
		}
	}
	return false;
}

void MazeIO::printMaze(void)
{
    if (NULL == maze) {
        printf("No maze object");
        return;
    }
    writeBufferFromMaze(true);
    writeIOFromBuffer(printIO);
}

void MazeIO::loadMazeFromString(char* str)
{
    if (NULL == str) {
        printf("No str to load maze.\n");
        return;
    }

    // Reading from str
    Wall wallToPut;
    char tmp;
    char *buf = str;
    for (int i = 0; i < (CONFIG_MAX_ROW_SIZE * 2 + 1); i++) {
        for (int j = 0; j < (CONFIG_MAX_COL_SIZE * 2 + 1); j++) {
            tmp = *buf++;
            switch (tmp) {
            case '_':
            case '|':
                wallToPut = Wall::wall;
                if (j != CONFIG_MAX_COL_SIZE * 2) {
                	buf++;
                }
                break;
            case '.':
                wallToPut = Wall::empty;
				buf++;
                break;
            case '*':
                wallToPut = Wall::unknown;
				buf++;
                break;
            case 'S': /* Starting point */
                maze->startPos.row = i / 2;
                maze->startPos.col = j / 2;
                wallToPut = Wall::wallError;
                continue;
            case 'G':
            	maze->goalPos.push_back(Position{i/2, j/2});
                wallToPut = Wall::wallError;
                continue;
            case ' ':
            default:
                wallToPut = Wall::wallError;
                continue;
                break;
            }
            if ((i % 2 == 0) && (j % 2 == 1)) {
                maze->rowWall[i / 2][j / 2] = wallToPut;
            } else if ((i % 2 == 1) && (j % 2 == 0)) {
                maze->colWall[i / 2][j / 2] = wallToPut;
            }
        }
        /* Move buffer until newline */
        while (*buf != '\n') {
            if ((buf - str) >= sizeof(str)) {
                break;
            } else {
                buf++;
            }
        }
        buf++;  /* Skip '\n' */
    }

    /* update the cell */
    maze->updateCell();
}

void MazeIO::loadMazeFromFile(char* fileName)
{
    //Open maze file
    fileIO->open(fileName, "r");

    //Copy maze file to buffer
    size_t count = fileIO->read(this->buffer, sizeof(char), sizeof(this->buffer));
    if (count != sizeof(this->buffer)) {
        printf("Error reading file, check the format of the file.\n");
        return;
    }

    loadMazeFromString(this->buffer);
}

void MazeIO::saveMazeToFile(char* fileName)
{
    // check maze object
    if (NULL == maze) {
        printf("No maze object");
        return;
    }

    // try opening file
    fileIO->open(fileName, "w");

    writeBufferFromMaze(false);
    writeIOFromBuffer(fileIO);
}
/*******************************************************************************
 * Private Methods
 ******************************************************************************/
void MazeIO::writeBufferFromMaze(bool isShowMouse)
{
    char *ptr = buffer;
    for (int i = 0; i < (CONFIG_MAX_ROW_SIZE * 2 + 1); i++) {
        if (i % 2 == 0) {
            // row_plus/row_minus
            for (int j = 0; j < CONFIG_MAX_COL_SIZE; j++) {
                // print a space then wall
                *ptr++ = ' ';
                switch (maze->rowWall[i / 2][j]) {
                case Wall::empty:
                    *ptr++ = '.';
                    *ptr++ = '.';
                    break;
                case Wall::wall:
                	// The cell size is twice as long
                    *ptr++ = '_';
                    *ptr++ = '_';
                    break;
                case Wall::unknown:
                	// The cell size is twice as long
                    *ptr++ = '*';
                    *ptr++ = '*';
                    break;
                case Wall::wallError:
                default:
                    printf("Error on rowWall!");
                    *ptr++ = '?';
                    break;
                }
            }
            *ptr++ = ' ';
        } else {
            // col_plus/col_minus
            for (int j = 0; j < CONFIG_MAX_COL_SIZE + 1; j++) {
                /* print wall first */
                switch (maze->colWall[i / 2][j]) {
                case Wall::empty:
                    *ptr++ = '.';
                    break;
                case Wall::wall:
                    *ptr++ = '|';
                    break;
                case Wall::unknown:
                    *ptr++ = '*';
                    break;
                case Wall::wallError:
                default:
                    printf("Error on rowWall!");
                    *ptr++ = '?';
                    break;
                }
                // then print cell
                if (!(j >= CONFIG_MAX_COL_SIZE)) {
                    printCell(i / 2, j, isShowMouse, ptr);
                    ptr += 2;
                }
            }
        }
        /* print newline */
        *ptr++ = '\n';
    }
}

void MazeIO::writeIOFromBuffer(IOInterface *io)
{
	io->write(buffer, sizeof(char), sizeof(buffer));
}

void MazeIO::printCell(int row, int col, bool isShowMouse, char* buf)
{
    /* Check if this is mouse position */
    if (mousePosition.getCurrentPos().col == col &&
    	mousePosition.getCurrentPos().row == row && isShowMouse) {
    	switch(mousePosition.getCurrentDir()) {
			case Direction::row_minus:		//mouse pointing up
				*buf++ = 'M';
				*buf = '^';
				break;
			case Direction::col_minus:
				*buf++ = '<';	//mouse pointing left
				*buf = 'M';
				break;
			case Direction::row_plus:		//mouse pointing down
				*buf++ = 'M';
				*buf = 'v';
				break;
			case Direction::col_plus:		//mouse pointing right
				*buf++ = 'M';
				*buf = '>';
				break;
			case Direction::eDirError:
				*buf++ = 'M';
				*buf = 'M';
				break;
			default:
				*buf++ = ' ';
				*buf = 'M';
				break;
    	}
    } else if (positionIsDestination(Position{row, col}) && isShowMouse) {
    	*buf++ = ' ';
    	*buf = 'D';
    } else if (maze->getCell(row, col).attribute == Attribute::start) {
        *buf++ = ' ';
        *buf = 'S';
    } else if (maze->getCell(row, col).attribute == Attribute::goal) {
        *buf++ = ' ';
        *buf = 'G';
    } else if (isShowMouse) {
    	if (maze->getCell(row, col).distance == CELL_DISTANCE_UNREACHED) {
    		*buf++ = ' ';
    		*buf = 'x';
    	} else {
    		int dis = maze->getCell(row, col).distance;
    		*buf++ = (char) (dis/10 + 48);
    		*buf = (char) (dis%10 + 48);
    	}
    } else if (col != CONFIG_MAX_COL_SIZE) {
        *buf++ = ' ';
        *buf = ' ';
    }
}
