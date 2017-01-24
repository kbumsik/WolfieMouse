/*
 * MazeIO.cpp
 *
 *  Created on: Jan 17, 2017
 *      Author: vagrant
 */

#include "MazeIO.hpp"
#include "Maze.hpp"


static void clearLine(FILE *pFile);
/** FIXME: dynamically decide the starting dirction */

/*******************************************************************************
 * Constructor
 ******************************************************************************/
MazeIO::MazeIO(Maze *mazePtr) :
    maxRowSize(CONFIG_MAX_ROW_SIZE),
    maxColSize(CONFIG_MAX_COL_SIZE),
    maze(mazePtr)
{
    // The initializer does it all
}

/*******************************************************************************
 * Public Methods
 ******************************************************************************/
Position MazeIO::getMousePosition(void)
{
    return mousePosition;
}

void MazeIO::setMousePosition(Position pos)
{
    mousePosition = pos;
}

void MazeIO::printMaze(void)
{
    if (NULL == maze) {
        printf("No maze object");
        return;
    }
    writeBufferFromMaze(true);
    writeFileFromBuffer(stdout);
}

void MazeIO::loadMaze(char* fileName)
{
    FILE *pFile;
    char buf;

    if (NULL == fileName) {
        printf("No file to load maze.\n");
        return;
    }

    pFile = fopen(fileName, "r");
    if (NULL == pFile) {
        printf("Failed to open file");
        return;
    }
    /**
     * Reading part
     */
    Wall wallToPut;
    for (int i = 0; i < (CONFIG_MAX_ROW_SIZE * 2 + 1); i++) {
        for (int j = 0; j < (CONFIG_MAX_COL_SIZE * 2 + 1); j++) {
            if ((buf = fgetc(pFile)) == EOF) {
                printf("error?"); /* TODO: check error condition of fgets */
            }
            switch (buf) {
            case '_':
            case '|':
                wallToPut = wall;
                break;
            case '.':
                wallToPut = empty;
                break;
            case '*':
                wallToPut = unknown;
                break;
            case 'S': /* Starting point */
                maze->index_start_row = i / 2;
                maze->index_start_col = j / 2;
                wallToPut = wallError;
                continue;
            case 'G':
                maze->index_goal_row = i / 2;
                maze->index_goal_col = j / 2;
                wallToPut = wallError;
                continue;
            case ' ':
            default:
                wallToPut = wallError;
                continue;
                break;
            }
            if ((i % 2 == 0) && (j % 2 == 1)) {
                maze->rowWall[i / 2][j / 2] = wallToPut;
            } else if ((i % 2 == 1) && (j % 2 == 0)) {
                maze->colWall[i / 2][j / 2] = wallToPut;
            }
        }
        clearLine(pFile);
    }

    /* update the cell */
    maze->updateCell();
}

void MazeIO::saveMaze(char* fileName)
{
    // check maze object
    if (NULL == maze) {
        printf("No maze object");
        return;
    }

    FILE *pFile;
    char buf;
    // try opening file
    pFile = fopen(fileName, "w");
    if (NULL == pFile) {
        printf("Failed to open file");
        return;
    }
    writeBufferFromMaze(false);
    writeFileFromBuffer(pFile);
}
/*******************************************************************************
 * Private Methods
 ******************************************************************************/
void MazeIO::writeBufferFromMaze(bool isShowMouse)
{
    char buf;
    char *ptr = buffer;
    for (int i = 0; i < (CONFIG_MAX_ROW_SIZE * 2 + 1); i++) {
        if (i % 2 == 0) {
            // row_plus/row_minus
            for (int j = 0; j < CONFIG_MAX_COL_SIZE; j++) {
                // print a space then wall
                *ptr++ = ' ';
                switch (maze->rowWall[i / 2][j]) {
                case empty:
                    *ptr++ = '.';
                    break;
                case wall:
                    *ptr++ = '_';
                    break;
                case unknown:
                    *ptr++ = '*';
                    break;
                case wallError:
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
                case empty:
                    *ptr++ = '.';
                    break;
                case wall:
                    *ptr++ = '|';
                    break;
                case unknown:
                    *ptr++ = '*';
                    break;
                case wallError:
                default:
                    printf("Error on rowWall!");
                    *ptr++ = '?';
                    break;
                }
                // then print cell
                if (!(j >= CONFIG_MAX_COL_SIZE)) {
                    printCell(i / 2, j, isShowMouse, ptr);
                    ptr++;
                }
            }
        }
        /* print newline */
        *ptr++ = '\n';
    }
}

void MazeIO::writeFileFromBuffer(FILE *pFile)
{
    fwrite(buffer, sizeof(char), sizeof(buffer), pFile);
}

void MazeIO::printCell(int row, int col, bool isShowMouse, char* buf)
{
    /* Check if this is mouse position */
    if (mousePosition.col == col && mousePosition.row == row && isShowMouse) {
        *buf = 'M';
    } else if (maze->getCell(row, col).attribute == start) {
        *buf = 'S';
    } else if (maze->getCell(row, col).attribute == goal) {
        *buf = 'G';
    } else if (isShowMouse) {
        *buf = (maze->getCell(row, col).distance == CELL_DISTANCE_UNREACHED) ?
                        'x' : '0' + maze->getCell(row, col).distance;
    } else if (col != CONFIG_MAX_COL_SIZE) {
        *buf = ' ';
    }
}

/*******************************************************************************
 * Private Functions
 ******************************************************************************/
static void clearLine(FILE *pFILE)
{
    char buf;
    while ((buf = fgetc(pFILE)) != '\n') {
        if (buf == EOF)
            break;
    }
}
