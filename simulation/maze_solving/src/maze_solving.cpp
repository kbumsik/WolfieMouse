
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
enum WolfieState
{
    goGoal  = 0,
    goStart = 1,
    explore = 2
};

bool moveOneCell(MouseController &mouse);

int main()
{
    char tmp;
    WolfieState mouseState = goGoal;
    MouseController mouse("out.txt", &fileIO, &printIO,
            (FinderInterface*) &virtualMouse, (MoverInterface*) &virtualMouse);

    /* First just print maze */
    mouse.printMaze();
    printf("please input a command\n");
    printf("q: exit, n:next\n");
    fflush(stdout);
    while (true) {
        tmp = getchar();
        if (tmp == 'n') {
            break;
        } else if (tmp == 'q') {
            goto end;
        } else {
            continue;
        }
    }

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

    end: mouse.saveMazeFile("out.txt");
    return 0;
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
    mouse.printMaze();
    printf("please input a command\n");
    printf("q: exit, n:next, p: print stack\n");
    fflush(stdout);
    while (true) {
        tmp = getchar();
        if (tmp == 'n') {
            break;
        } else if (tmp == 'q') {
            return false;
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
        tmp = getchar();
        if (tmp == 'n') {
            break;
        } else if (tmp == 'q') {
            return false;
        } else {
            continue;
        }
    }
    return true;
}
