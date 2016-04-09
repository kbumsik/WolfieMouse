
#include <stdlib.h>
#include "MouseController.h"
#include "Maze.h"


int main() {
	Maze testMaze;
	testMaze.readMazeFromFile("1.txt");
	testMaze.printMaze();
	testMaze.saveMazeFile("2.txt");
	return 0;
}
