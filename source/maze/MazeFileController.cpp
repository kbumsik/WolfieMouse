/*
 * MazeFileReader.cpp
 *
 *  Created on: Apr 8, 2016
 *      Author: Bumsik Kim
 */

#include "Maze.h"

#include <stdio.h>
#include <stddef.h>

static void
clearLine (FILE *pFile);

/** FIXME: dynamically decide the starting dirction */

void
Maze::readMazeFromFile (char* fileName)
{
	FILE *pFile;
	char buf;

	pFile = fopen(fileName, "r");
	if (NULL == pFile)
	{
		printf("Failed to open file");
	}
	/**
	 * Reading part
	 */
	enum wall wallToPut;
	for (int i = 0; i < (mazeMAX_ROW_SIZE * 2 + 1); i++)
	{
		for (int j = 0; j < (mazeMAX_COL_SIZE * 2 + 1); j++)
		{
			if ((buf = fgetc(pFile)) == EOF)
			{
				printf("error?"); /* TODO: check error condition of fgets */
			}
			switch (buf)
			{
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
				index_start_row = i / 2;
				index_start_col = j / 2;
				wallToPut = eWallError;
				continue;
			case 'G':
				index_goal_row = i / 2;
				index_goal_col = j / 2;
				wallToPut = eWallError;
				continue;
			case ' ':
			default:
				wallToPut = eWallError;
				continue;
			break;
			}
			if ((i % 2 == 0) && (j % 2 == 1))
			{
				rowWall[i / 2][j / 2] = wallToPut;
			}
			else if ((i % 2 == 1) && (j % 2 == 0))
			{
				colWall[i / 2][j / 2] = wallToPut;
			}
		}
		clearLine(pFile);
	}

	/* update the cell */
	updateCell();
}

void
Maze::printMaze ()
{
	writeMazeToFile(stdout, true);
}

void
Maze::saveMazeFile (char* fileName)
{
	FILE *pFile;
	char buf;

	pFile = fopen(fileName, "w");
	if (NULL == pFile)
	{
		printf("Failed to open file");
	}
	writeMazeToFile(pFile, false);
}

void
Maze::writeMazeToFile (void *pFile, bool isShowMouse)
{
	char buf;

	for (int i = 0; i < (mazeMAX_ROW_SIZE * 2 + 1); i++)
	{
		if (i % 2 == 0)
		{
			for (int j = 0; j < mazeMAX_COL_SIZE; j++)
			{
				switch (rowWall[i / 2][j])
				{
				case empty:
					buf = '.';
				break;
				case wall:
					buf = '_';
				break;
				case unknown:
					buf = '*';
				break;
				case eWallError:
				default:
					printf("Error on rowWall!");
				break;
				}
				fputc(' ', (FILE*) pFile);
				fputc(buf, (FILE*) pFile);
			}
			fputc(' ', (FILE*) pFile);
		}
		else
		{
			for (int j = 0; j < mazeMAX_COL_SIZE + 1; j++)
			{
				switch (colWall[i / 2][j])
				{
				case empty:
					buf = '.';
				break;
				case wall:
					buf = '|';
				break;
				case unknown:
					buf = '*';
				break;
				case eWallError:
				default:
					printf("Error on rowWall!");
				break;
				}
				/* print wall first */
				fputc(buf, (FILE*) pFile);
				if(!(j >= mazeMAX_COL_SIZE))
				{
					printCell(i/2, j, isShowMouse, pFile);
				}
			}
		}
		/* print newline */
		fputc('\r', (FILE*) pFile);
		fputc('\n', (FILE*) pFile);
	}
}

static void
clearLine (FILE *pFILE)
{
	char buf;
	while ((buf = fgetc(pFILE)) != '\n')
	{
		if (buf == EOF)
			break;
	}
}

void
Maze::printCell(int row, int col, bool isShowMouse, void *pFile)
{
	/* Check if this is mouse position */
	if (getCell(row, col).isMouse && isShowMouse)
	{
		fputc('M', (FILE*) pFile);
	}
	else if (getCell(row, col).isStart)
	{
		fputc('S', (FILE*) pFile);
	}
	else if (getCell(row, col).isGoal)
	{
		fputc('G', (FILE*) pFile);
	}
	else if (isShowMouse)
	{
		fputc((getCell(row, col).distance == UNREACHED)
			  ? 'x' : '0'+getCell(row, col).distance, (FILE*) pFile);
	}
	else if (col != mazeMAX_COL_SIZE)
	{
		fputc(' ', (FILE*) pFile);
	}
}
