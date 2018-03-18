
#include <Maze.hpp>
#include <MouseController.hpp>
#include <stdlib.h>
#include "IOInterface.hpp"
#include "StdIO.hpp"
#include "SimulMouse.hpp"
#include <stdio.h>

#define INPUT_FILE "maze_ieee_region_1_2015.txt"
#define SAVED_FILE "out.txt"
#define OUTPUT_FILE "out.txt"

StdIO fileIO(true);
StdIO printIO(false);
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
    
    /* Check if files exists first */
    char *file_names[3] = {INPUT_FILE, SAVED_FILE, OUTPUT_FILE};
    printf("%d\n", sizeof(file_names));

    for (unsigned int i = 0; i < (sizeof(file_names)/sizeof(char *)); i++) {
        FILE *file = NULL;
        if ((file = fopen(file_names[i], "r")) == NULL) {
            printf("%s doesn't exist! Aborting.\n", file_names[i]);
            return 1;
        }
        fclose(file);
    }

    /* Create object */
    WolfieState mouseState = goGoal;
    // Create virtual mouse hardware for simulation
    SimulMouse virtualMouse(INPUT_FILE, &fileIO, &printIO);
    // Create a mouse object
    MouseController mouse(SAVED_FILE, &fileIO, &printIO,
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

end:
    mouse.saveMazeFile(OUTPUT_FILE);
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
