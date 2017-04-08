
#include <Maze.hpp>
#include <MouseController.hpp>
#include <stdlib.h>
#include "IOInterface.hpp"
#include "StdIO.hpp"
#include "SimulMouse.hpp"

StdIO fileIO(false);
StdIO printIO(false);
// Create virtual mouse hardware for simulation
SimulMouse virtualMouse(NULL, &fileIO, &printIO);
enum WolfieState
{
    goGoal  = 0,
    goStart = 1,
    explore = 2
};

bool moveOneCell(MouseController &mouse);

void main_run(void)
{
    char tmp;
    WolfieState mouseState = goGoal;
    MouseController mouse(NULL, &fileIO, &printIO,
            (FinderInterface*) &virtualMouse, (MoverInterface*) &virtualMouse);

    /* First just print maze */
    mouse.printMaze();

    while (true) {
        //Finite State Machine
        if (mouse.allDestinationsReached()) {
            switch (mouseState) {
            case goGoal:
                mouseState = explore;
                mouse.setUnsearchDes(4);
                break;
            case explore:
                mouseState = goStart;
                mouse.setStartAsDes();
                break;
            case goStart:
                mouseState = goGoal;
                mouse.setGoalAsDes();
                break;
            default:
                printf("Invalid state");
                break;
            }
        } else {
            if (!moveOneCell(mouse)) {
                goto end;
            }
        }
    }

end:
    return;
}

bool moveOneCell(MouseController &mouse)
{
    char tmp;
    /* Then scan walls then calculate the distance */
    if (mouse.scanWalls()) {
        mouse.getDistanceAllCell();
        /* then get shortest path */
        mouse.getShortestPath();
    } else {
        mouse.getDistanceAllCell();
    }
    /* Then move */
    mouse.moveNextCell();
    //mouse.printMaze();
    return true;
}
