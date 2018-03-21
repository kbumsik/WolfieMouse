#include "Position.hpp"
#include "Queue.hpp"
#include <vector>
#include "stdio.hpp"
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
    int init_row= NULL,init_col = NULL;
    int final_col,final_row ;

    queue.init();
    for (int i = 0; i < pathPos.size(); i++) 
    {
        queue.pushToBack(pathPos[i]);
    }

    for(int i=0;i < pathPos.size(); i++)
    {
        if(init_col == NULL && init_row == NULL)
        {
            init_row = queue.peekFromFront().row;
            init_col = queue.popFromFront().col;

            final_col = queue.peekFromFront().col;
            final_row = queue.popFromFront().row;
        }
        else
        {
            init_row = final_row;
            init_col = final_col;

            final_col = queue.peekFromFront().col;
            final_row = queue.popFromFront().row;
        }


        if(init_row == (final_row-1))
                path.push_back('F');
        else if(init_row == (final_row+1) )
                path.push_back('B');
        /*though same change in the position,
        due to robots direction (facing up or down) 
        it can be the left turn or right turn*/

        /*when robot is facing up*/
        else if(init_col == (final_col+1) && path.front() == 'F')
                path.push_back('L');
        else if(init_col == (final_col-1) && path.front() == 'F')
                path.push_back('R');
        /*when robot is facing up*/

        /*when robot is facing down*/
        else if(init_col == (final_col+1) && path.front() == 'B')
                path.push_back('R');
        else if(init_col == (final_col-1) && path.front() == 'B')
                path.push_back('L');
        /*when robot is facing down*/

    }

    for(int i=0;i < pathPos.size(); i++)
    {
        printf("%c -> ",path.front());
        path.pop_front();
    }

    return 0;
}