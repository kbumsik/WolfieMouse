
#include <Maze.hpp>
#include <MouseController.hpp>
#include <stdlib.h>


void clear_screen();

int main()
{
	char tmp;
	MouseController mouse ("1.txt");
	while (true) {
		/* First just print maze */
		mouse.printMaze();
		printf("please input a command\r\n");
		printf("q: exit, n:next\r\n");
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
		/* Then calcluate the distance */
		mouse.getDistanceAllCell();
		mouse.printMaze();
		printf("please input a command\r\n");
		printf("q: exit, n:next\r\n");
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
		printf("please input a command\r\n");
		printf("q: exit, n:next, p: print stack\r\n");
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
		printf("please input a command\r\n");
		printf("q: exit, n:next\r\n");
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

		clear_screen();
	}
end:
	mouse.saveMazeFile("2.txt");
	return 0;
}

void clear_screen()
{
#ifdef WINDOWS
    system("cls");
#else
    // Assume POSIX
    system ("clear");
#endif
}
