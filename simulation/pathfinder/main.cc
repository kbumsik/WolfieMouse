#include "Position.hpp"
#include "Queue.hpp"
#include <vector>
#include <stdio.h>
#include <deque>

std::vector<Position> pathPos = {
    Position{6,3}, Position{7,3}, Position{8,3}, Position{9,3}, Position{9,4}, Position{8,4}, Position{7,4}, Position{7,5}, Position{7,6}
};

Queue<Position> queue;

//enum {F , B , L, R} direction;

//Queue<char> path;
//std::deque<char> path
std::deque<char>path;

int main(void)
{
    int init_row= -1,init_col = -1;
    int final_col,final_row ;
    Position init;
    Position last;

    init.row = -1; init.col = -1;

    queue.init();

    for (int i = 0; i < pathPos.size(); i++)   {
        queue.pushToBack(pathPos[i]);
    }

    for(int i=0;i < pathPos.size(); i++) {
        if(init.col == -1 && init.row == -1)  {
            /*
            init.row = queue.peekFromFront().row;
            init.col = queue.popFromFront().col;

            last.col = queue.peekFromFront().col;
            last.row = queue.popFromFront().row;
*/
            init = queue.popFromFront();
            last = queue.popFromFront();

        }   else  {
            /*
            init.row = last.row;
            init.col = last.col;

            last.col = queue.peekFromFront().col;
            last.row = queue.popFromFront().row;
            */
            init = last;
            last = queue.popFromFront();
        }


        if(init.row == (last.row-1))
                path.push_back('F');
        else if(init.row == (last.row+1) )
                path.push_back('B');
        /*though same change in the position,
        due to robots direction (facing up or down) 
        it can be the left turn or right turn*/

        /*when robot is facing up*/
        else if(init.col == (last.col+1) && path.front() == 'F')
                path.push_back('L');
        else if(init.col == (last.col-1) && path.front() == 'F')
                path.push_back('R');
        /*when robot is facing up*/

        /*when robot is facing down*/
        else if(init.col == (last.col+1) && path.front() == 'B')
                path.push_back('R');
        else if(init.col == (last.col-1) && path.front() == 'B')
                path.push_back('L');
        /*when robot is facing down*/

    }

    for(int i=0;i < pathPos.size(); i++)    {
        printf("%c -> ",path.front());
        path.pop_front();
    }

    return 0;
}