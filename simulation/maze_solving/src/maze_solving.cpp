
#include <Maze.hpp>
#include <MouseController.hpp>
#include <stdlib.h>
#include "IOInterface.hpp"
#include "StdIO.hpp"
#include "SimulMouse.hpp"

StdIO fileIO(true);
StdIO printIO(false);
// Create virtual mouse hardware for simulation
SimulMouse virtualMouse("maze_ieee_region_1_2015.txt", &fileIO, &printIO);

int main()
{
	char tmp;
	MouseController mouse ("out.txt", &fileIO, &printIO,
			(FinderInterface*)&virtualMouse, (MoverInterface*)&virtualMouse);
	while (true) {
		/* First just print maze */
		mouse.printMaze();
		printf("please input a command\n");
		printf("q: exit, n:next\n");
		fflush(stdout);
		while (true) {
			tmp= getchar();
			if (tmp == 'n') {
				break;
			} else if (tmp == 'q') {
				goto end;
			} else {
				continue;
			}
		}
		/* Then scan walls then calculate the distance */
		mouse.scanWalls();
		mouse.getDistanceAllCell();
		mouse.printMaze();
		printf("please input a command\n");
		printf("q: exit, n:next\n");
		fflush(stdout);
		while (true) {
			tmp= getchar();
			if (tmp == 'n') {
				break;
			} else if (tmp == 'q') {
				goto end;
			} else {
				continue;
			}
		}
		/* then get shortest path */
		mouse.getShortestPath();
		mouse.printMaze();
		printf("please input a command\n");
		printf("q: exit, n:next, p: print stack\n");
		fflush(stdout);
		while (true) {
			tmp= getchar();
			if (tmp == 'n') {
				break;
			} else if (tmp == 'q') {
				goto end;
			} else if (tmp == 'p') {
				mouse.printPathStack();
				fflush(stdout);
			} else {
				continue;
			}
		}
		/* Then move */
		mouse.moveNextCell();
		mouse.printMaze();
		printf("please input a command\n");
		printf("q: exit, n:next\n");
		fflush(stdout);
		while (true) {
			tmp= getchar();
			if (tmp == 'n') {
				break;
			} else if (tmp == 'q') {
				goto end;
			} else {
				continue;
			}
		}
	}
end:
	mouse.saveMazeFile("out.txt");
	return 0;
}
