
#include <stdlib.h>
#include "MouseController.h"
#include "Maze.h"


int main() {
	MouseController mouse;
	mouse = MouseController("1.txt");
	mouse.printMaze();
	mouse.saveMazeFile("2.txt");
	return 0;
}
