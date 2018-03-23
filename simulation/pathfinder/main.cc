#include "Position.hpp"
#include "Queue.hpp"
#include <vector>
#include <stdio.h>
#include <deque>

std::vector<Position> pathPos = {
    Position{13, 1}, Position{13, 2}, Position{12, 2}, Position{12, 1}, Position{13, 1}, Position{14, 1},
    Position{14, 2}, Position{14, 3}, Position{14, 4}, Position{13, 4}, Position{12, 4}, Position{11, 4},
    Position{10, 4}, Position{9, 4}, Position{8, 4}, Position{7, 4}, Position{7, 5}, Position{7, 6}

};

Queue<Position> queue;

enum direction
{
	F_U = 'U',
	F_D = 'D',
	cc90 = 'L',
	cw90 = 'R',
	V = 'V',
	H = 'H'
};
enum real_move
{
	F = 'F',
	H_F = 'h',
	smooth_R = 'R',
	smooth_L = 'L'
};

std::deque<enum direction> path;
std::deque<enum real_move> move;

int main(void)
{
	// int init_row= -1,init_col = -1;
	//int final_col,final_row ;
	Position init;
	Position last;
	direction path_direction;
	real_move move_direction;
	char current_status = V; // V : vertical, H : horizontal

	init.row = -1;
	init.col = -1;

	queue.init();

	for (int i = 0; i < pathPos.size(); i++)
	{
		queue.pushToBack(pathPos[i]);
	}

	for (int i = 0; i < pathPos.size(); i++)
	{

		if (init.col == -1 && init.row == -1)
		{

			init = queue.popFromFront();
			last = queue.popFromFront();
		}
		else
		{

			init = last;
			last = queue.popFromFront();
		}

		if (current_status == V)
		{
			if (init.col == (last.col - 1))
				path.push_back(F_U);
			else if (init.col == (last.col + 1))
				path.push_back(F_D);
			/*though same change in the position,
	    due to robots direction (facing up or down) 
	    it can be the left turn or right turn*/

			/*when robot is facing up*/
			else if (init.row == (last.row + 1) && path.back() == F_U)
			{
				path.push_back(cc90);
				path.push_back(F_D);
				current_status = H;
			}
			else if (init.row == (last.row - 1) && path.back() == F_U)
			{
				path.push_back(cw90);
				path.push_back(F_U);
				current_status = H;
			}
			/*when robot is facing up*/

			/*when robot is facing down*/
			else if (init.row == (last.row + 1) && path.back() == F_D)
			{
				path.push_back(cw90);
				path.push_back(F_D);
				current_status = H;
			}
			else if (init.row == (last.row - 1) && path.back() == F_D)
			{
				path.push_back(cc90);
				path.push_back(F_U);
				current_status = H;
			}
			/*when robot is facing down*/
		}
		else if (current_status == H)
		{
			if (init.row == (last.row - 1))
				path.push_back(F_U);
			else if (init.row == (last.row + 1))
				path.push_back(F_D);
			/*though same change in the position,
	    due to robots direction (facing up or down) 
	    it can be the left turn or right turn*/

			/*when robot is facing up*/
			else if (init.col == (last.col + 1) && path.back() == F_U)
			{
				path.push_back(cw90);
				path.push_back(F_D);
				current_status = V;
			}
			else if (init.col == (last.col - 1) && path.back() == F_U)
			{
				path.push_back(cc90);
				path.push_back(F_U);
				current_status = V;
			}
			/*when robot is facing up*/

			/*when robot is facing down*/
			else if (init.col == (last.col + 1) && path.back() == F_D)
			{
				path.push_back(cc90);
				path.push_back(F_D);
				current_status = V;
			}
			else if (init.col == (last.col - 1) && path.back() == F_D)
			{
				path.push_back(cw90);
				path.push_back(F_U);
				current_status = V;
			}
			/*when robot is facing down*/
		}
	}

	/*
    pritnf("robot path \n")
    while(!path.empty())   {
	printf("%c -> ",path.front());
	path.pop_front();
    }
    printf("\n");
    */

	///////////////////// I don't delete this for just incase we need it /////////////////////

	while (!path.empty())
	{
		if (path.front() == cc90)
		{
			path.pop_front();
			path.pop_front();
			if (move.back() == F)
				move.push_back(H_F);
			move.push_back(smooth_L);
			if (path.front() == F_U || path.front() == F_D)
				move.push_back(H_F);
		}
		else if (path.front() == cw90)
		{
			path.pop_front();
			path.pop_front();
			if (move.back() == F)
				move.push_back(H_F);
			move.push_back(smooth_R);
			if (path.front() == F_U || path.front() == F_D)
				move.push_back(H_F);
		}
		else if (path.front() == F_U || path.front() == F_D)
		{
			path.pop_front();
			move.push_back(F);
		}
	}

	printf(" robot moving direction \n");

	while (!move.empty())
	{
		printf("%c -> ", move.front());
		move.pop_front();
	}
	printf("\n");

	return 0;
}