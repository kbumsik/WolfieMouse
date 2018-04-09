
#include <Maze.hpp>
#include <MouseController.hpp>
#include <stdlib.h>
#include "IOInterface.hpp"
#include "StdIO.hpp"
#include "SimulMouse.hpp"
#include <stdio.h>
#include "mazeString.hpp"

#ifndef MAZE_QUESTION
    #warning MAZE_QUESTION is not defined in Makefile. Set to default.
    #define MAZE_QUESTION "maze_ieee_region_1_2015.txt"
#endif
#ifndef MAZE_SAVED
    #warning MAZE_SAVED is not defined in Makefile. Set to default.
    #define MAZE_SAVED "out.txt"
#endif
#ifndef MAZE_OUT
    #warning MAZE_OUT is not defined in Makefile. Set to default.
    #define MAZE_OUT "out.txt"
#endif

bool moveOneCell(MouseController &mouse);

int main()
{
    StdIO fileIO(true);
    StdIO printIO(false);
    enum WolfieState {
        goGoal  = 0,
        goStart = 1,
        explore = 2
    } mouseState = goGoal;
    char keyinput;
    
    /* Check if files exists first */
    char *file_names[3] = {MAZE_QUESTION, MAZE_SAVED, MAZE_OUT};
    for (unsigned int i = 0; i < (sizeof(file_names)/sizeof(char *)); i++) {
        FILE *file = NULL;
        if ((file = fopen(file_names[i], "r")) == NULL) {
            printf("%s doesn't exist! Aborting.\n", file_names[i]);
            return 1;
        }
        fclose(file);
    }

    // Create virtual mouse hardware for simulation
    SimulMouse virtualMouse(const_cast<Maze::StringMaze *>(&mazeString), &fileIO, &printIO);
    // Create a mouse object
    MouseController mouse(MAZE_SAVED, &fileIO, &printIO,
            (FinderInterface*) &virtualMouse, (MoverInterface*) &virtualMouse);

    /* First just print maze */
    mouse.printMaze();
    printf("please input a command\n");
    printf("q: exit, n:next\n");
    fflush(stdout);
    while (true) {
        keyinput = getchar();
        if (keyinput == 'n') {
            break;
        } else if (keyinput == 'q') {
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

end:
    mouse.saveMazeFile(MAZE_OUT);
    return 0;
}

bool moveOneCell(MouseController &mouse)
{
    char keyinput;
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
        keyinput = getchar();
        if (keyinput == 'n') {
            break;
        } else if (keyinput == 'q') {
            return false;
        } else if (keyinput == 'p') {
            mouse.printPathStack();
            fflush(stdout);
        } else {
            continue;
        }
    }
    /* Then move */
    mouse.moveNextCell();

    /* And print */
    mouse.printMaze();
    printf("please input a command\n");
    printf("q: exit, n:next\n");
    fflush(stdout);
    while (true) {
        keyinput = getchar();
        if (keyinput == 'n') {
            break;
        } else if (keyinput == 'q') {
            return false;
        } else {
            continue;
        }
    }
    return true;
}
