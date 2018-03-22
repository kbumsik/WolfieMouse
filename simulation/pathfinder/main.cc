#include "Position.hpp"
#include "Queue.hpp"
#include <vector>
#include <stdio.h>
#include <deque>

std::vector<Position> pathPos = {
Position{13,1}, Position{13,2}, Position{12,2}, Position{12,1}, Position{13,1}, Position{14,1}, 
Position{14,2}, Position{14,3}, Position{14,4}, Position{13,4}, Position{12,4}, Position{11,4}, 
Position{10,4}, Position{9,4}, Position{8,4}, Position{7,4}, Position{7,5}, Position{7,6}

};

Queue<Position> queue;

enum direction{F_U = 'U',F_D = 'D',cc90 = 'L',cw90 = 'R', V = 'V', H = 'H', smooth_R = 'r', smooth_L = 'l'} ;


std::deque<enum direction>path;
std::deque<enum direction>s_path;

int main(void)
{
   // int init_row= -1,init_col = -1;
    //int final_col,final_row ;
    Position init;
    Position last;
    direction path_direction;
    char current_status = V; // V : vertical, H : horizontal

    init.row = -1; init.col = -1;
    
    queue.init();
    

    for (int i = 0; i < pathPos.size(); i++)   {
        queue.pushToBack(pathPos[i]);
    }

    for(int i=0;i < pathPos.size(); i++) {

        if(init.col == -1 && init.row == -1)  {
            
            init = queue.popFromFront();
            last = queue.popFromFront();

        }   else  {
            
            init = last;
            last = queue.popFromFront();
        }

        if(current_status == V)  {
            if(init.col == (last.col-1))
                    path.push_back(F_U);
            else if(init.col == (last.col+1) )
                    path.push_back(F_D);
            /*though same change in the position,
            due to robots direction (facing up or down) 
            it can be the left turn or right turn*/

            /*when robot is facing up*/
            else if(init.row == (last.row+1) && path.back() == F_U){
                    path.push_back(cc90);
                    path.push_back(F_D);
                    current_status = H;
                    }
            else if(init.row == (last.row-1) && path.back() == F_U){
                    path.push_back(cw90);
                    path.push_back(F_U);
                    current_status = H;
                    }
            /*when robot is facing up*/

            /*when robot is facing down*/
            else if(init.row == (last.row+1) && path.back() == F_D){
                    path.push_back(cw90);
                    path.push_back(F_D);
                    current_status = H;
                    }
            else if(init.row == (last.row-1) && path.back() == F_D){
                    path.push_back(cc90);
                    path.push_back(F_U);
                    current_status = H;
                    }
            /*when robot is facing down*/
        } else if(current_status == H)  {
            if(init.row == (last.row-1))
                    path.push_back(F_U);
            else if(init.row == (last.row+1) )
                    path.push_back(F_D);
            /*though same change in the position,
            due to robots direction (facing up or down) 
            it can be the left turn or right turn*/

            /*when robot is facing up*/
            else if(init.col == (last.col+1) && path.back() == F_U){
                    path.push_back(cw90);
                    path.push_back(F_D);
                    current_status = V;
                    }
            else if(init.col == (last.col-1) && path.back() == F_U){
                    path.push_back(cc90);
                    path.push_back(F_U);
                    current_status = V;
                    printf("%d. %d -> %d,%d   /  %c \n",init.row,init.col, last.row,last.col,path.back());
                    }
            /*when robot is facing up*/

            /*when robot is facing down*/
            else if(init.col == (last.col+1) && path.back() == F_D){
                    path.push_back(cc90);
                    path.push_back(F_D);
                    current_status = V;
                    }
            else if(init.col == (last.col-1) && path.back() == F_D){
                    path.push_back(cw90);
                    path.push_back(F_U);
                    current_status = V;
                    }
            /*when robot is facing down*/
                           
                   
        }
        
    }

    for(int i=0; path.front() ; i++){
            
    }

    for(int i=0; path.front() ; i++)    {
        printf("%c -> ",path.front());
        path.pop_front();
    }
    printf("\n");

    return 0;
}