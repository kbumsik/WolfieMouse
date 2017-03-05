
#include <Maze.hpp>
#include <MouseController.hpp>
#include <stdlib.h>
#include "IOInterface.hpp"
#include "StdIO.hpp"
#include "SimulMouse.hpp"

StdIO fileIO(true);
StdIO printIO(false);
SimulMouse virtualMouse;


int main()
{
	char tmp;
	MouseController mouse ("1.txt", &fileIO, &printIO,
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
		/* Then calculate the distance */
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
	mouse.saveMazeFile("2.txt");
	return 0;
}
