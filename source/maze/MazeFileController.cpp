/*
 * MazeFileReader.cpp
 *
 *  Created on: Apr 8, 2016
 *      Author: Bumsik Kim
 */

#include "Maze.h"

#include <stdio.h>
#include <stddef.h>

static void clearLine(FILE *pFile);

int mazeINDEX_GOAL_ROW = 7;
int mazeINDEX_GOAL_COL = 7;
int mazeINDEX_START_ROW = 0;
int mazeINDEX_START_COL = 0;

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
	wall_e wallToPut;
	for (int i = 0; i < (mazeMAX_ROW_SIZE * 2 + 1); i++)
	{
		for (int j = 0; j < (mazeMAX_COL_SIZE * 2 + 1); j++)
		{
			if ((buf = fgetc(pFile)) == EOF)
			{
				printf("error?");	/* TODO: check error condition of fgets */
			}
			switch (buf)
			{
				case '-':
				case '|':
					wallToPut = wall;
					break;
				case '.':
					wallToPut = empty;
					break;
				case '*':
					wallToPut = unknown;
					break;
				case 'S':	/* Starting point */
					mazeINDEX_START_ROW = i/2;
					mazeINDEX_START_COL = j/2;
					wallToPut = eWallError;
					continue;
				case 'G':
					mazeINDEX_GOAL_ROW = i/2;
					mazeINDEX_GOAL_COL = j/2;
					wallToPut = eWallError;
					continue;
				case ' ':
				default:
					wallToPut = eWallError;
					continue;
					break;
			}
			if ( (i % 2 == 0) && (j % 2 == 1) )
			{
				rowWall[i/2][j/2] = wallToPut;
			}
			else if ( (i % 2 == 1) && (j % 2 == 0) )
			{
				colWall[i/2][j/2] = wallToPut;
			}
		}
		clearLine(pFile);
	}

	/* update the cell */
	updateCell();
}

void
Maze::printMaze()
{
	writeMazeToFile(stdout);
}

void
Maze::saveMazeFile(char* fileName)
{
	FILE *pFile;
	char buf;

	pFile = fopen(fileName, "w");
	if (NULL == pFile)
	{
		printf("Failed to open file");
	}
	writeMazeToFile(pFile);
}

static void
clearLine(FILE *pFILE)
{
	char buf;
	while((buf = fgetc(pFILE)) != '\n')
	{
		if(buf == EOF)
			break;
	}
}

void
Maze::writeMazeToFile (void *pFile)
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
						buf = '-';
					break;
					case unknown:
						buf = '*';
					break;
					case eWallError:
					default:
						printf("Error on rowWall!");
					break;
				}
				fputc(' ', (FILE*)pFile);
				fputc(buf, (FILE*)pFile);
			}
			fputc(' ', (FILE*)pFile);
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
				fputc(buf, (FILE*)pFile);

				if ((i/2 == mazeINDEX_START_ROW)&&(j == mazeINDEX_START_COL))
				{
					fputc('S', (FILE*)pFile);
				}
				else if ((i/2 == mazeINDEX_GOAL_ROW)&&(j == mazeINDEX_GOAL_COL))
				{
					fputc('G', (FILE*)pFile);
				}
				else if (j != mazeMAX_COL_SIZE)
				{
					fputc(' ', (FILE*)pFile);
				}
			}
		}
		/* print newline */
		fputc('\r', (FILE*)pFile);
		fputc('\n', (FILE*)pFile);
	}
}
