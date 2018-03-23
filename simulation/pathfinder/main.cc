#include "Position.hpp"
#include "Queue.hpp"
#include <vector>
#include <stdio.h>
#include <deque>

std::vector<Position> pathPos = {
	Position{5,1}, Position{5,0}, Position{4,0}, Position{3,0}, Position{3,1}, Position{3,2}, Position{3,3}, Position{3,4}, Position{3,5}, Position{3,6}, Position{3,7}, Position{4,7}, Position{5,7}, Position{6,7}

}
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
	smooth_L = 'L',
	R_dia_turn = 'r',
	L_dia_turn = 'l',
	diagonal_forward = 'D'
};

std::deque<enum direction> path; // position based path.

std::deque<enum real_move> move; // smooth_path

std::deque<enum real_move> diagonal_move; // diagonal path
std::deque<enum real_move> temp_move;

int main(void)
{
	// int init_row= -1,init_col = -1;
	//int final_col,final_row ;
	Position init;
	Position last;
	direction path_direction;
	real_move move_direction;
	real_move before_direction;
	real_move d_start_direction;
	real_move d_end_direction;

	char current_status = V; // V : vertical, H : horizontal
	int cnt = 0;
	int d_cnt = 1;
	int swap;

	init.row = -1;
	init.col = -1;

	queue.init();

	for (int i = 0; i < pathPos.size(); i++)
	{
		swap = (15 - (pathPos[i].row));
		pathPos[i].row = (pathPos[i].col);
		pathPos[i].col = swap;
	}
	for (int i = 0; i < pathPos.size(); i++)
	{
		printf("position {%d,%d}", pathPos[i].row, pathPos[i].col);
	}
	printf("\n");
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

	//     printf("robot path \n");
	//     while(!path.empty())   {
	// 	printf("%c -> ",path.front());
	// 	path.pop_front();
	//     }
	//     printf("\n");

	///////////////////// I don't delete this for just incase we need it /////////////////////

	//smooth turn//

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

	while (!move.empty())
	{
		printf("%c -> ", move.front());
		move.pop_front();
	}
	printf("\n");

	///////////////////// I don't delete this for just incase we need it /////////////////////

	// diagonal move//

	// move.push_back(F);
	// move.push_back(H_F);
	// move.push_back(smooth_R);
	// move.push_back(smooth_L);
	// move.push_back(smooth_R);
	// move.push_back(smooth_L);
	// move.push_back(H_F);

	// while (!move.empty())
	// {
	// 	if (move.front() == F)
	// 	{
	// 		diagonal_move.push_back(move.front());
	// 		move.pop_front();
	// 	}
	// 	else if (move.front() == H_F)
	// 	{

	// 		diagonal_move.push_back(H_F); //insert H_F
	// 		move.pop_front();
	// 		d_start_direction = move.front();
	// 		// before_direction = move.front();
	// 		// d_start_direction = before_direction;
	// 		while (move.front() == smooth_R || move.front() == smooth_L) // temp_move -> inserting all nearby Left/Right moving direction
	// 		{
	// 			temp_move.push_back(move.front());
	// 			move.pop_front();
	// 			cnt++;
	// 		}

	// 		if (cnt == 1) // 100% smooth turn
	// 		{
	// 			diagonal_move.push_back(temp_move.front());
	// 			temp_move.pop_front();
	// 			//diagonal_move.push_back(H_F);
	// 			cnt--;
	// 		}
	// 		else if (cnt > 1) // various direction
	// 		{
	// 			while (cnt >= 1)
	// 			{
	// 				before_direction = temp_move.front();
	// 				temp_move.pop_front();
	// 				if (before_direction != temp_move.front()) // diagonal case counting
	// 				{
	// 					d_cnt++;
	// 					before_direction = temp_move.front();
	// 					d_end_direction = before_direction;
	// 					temp_move.pop_front();
	// 					cnt--;
	// 				}
	// 				else if (before_direction == temp_move.front() && d_cnt != 1) // diagonal case moving direction
	// 				{
	// 					diagonal_move.push_back((d_start_direction == smooth_R) ? R_dia_turn : L_dia_turn);

	// 					while (d_cnt != 1)
	// 					{
	// 						diagonal_move.push_back(diagonal_forward);
	// 						d_cnt--;
	// 					}
	// 					diagonal_move.push_back(diagonal_forward); // as we start at d_cnt = 1 and ends at d_cnt = 1
	// 					diagonal_move.push_back((d_end_direction == smooth_R) ? R_dia_turn : L_dia_turn);
	// 				}
	// 				else if (before_direction == temp_move.front())
	// 				{ // smooth turn case
	// 					if ((diagonal_move.back() != L_dia_turn) && (diagonal_move.back() != R_dia_turn))
	// 					{
	// 						diagonal_move.push_back(temp_move.front());
	// 						temp_move.pop_front(); //////////?????????
	// 						if (temp_move.empty())
	// 							diagonal_move.push_back(H_F); //////////?????????
	// 						cnt--;
	// 					}
	// 					else if ((diagonal_move.back()) == (L_dia_turn) || (diagonal_move.back()) == (R_dia_turn))
	// 					//diagonal case finish
	// 					{
	// 						//diagonal_move.push_back(temp_move.front());
	// 						//temp_move.pop_front(); //////////?????????
	// 						//diagonal_move.push_back(H_F); //////////?????????
	// 						cnt--;
	// 					}
	// 				}

	// 				else if (temp_move.empty() && d_cnt != 1)
	// 				{
	// 					diagonal_move.push_back((d_start_direction == smooth_R) ? R_dia_turn : L_dia_turn);
	// 					while (d_cnt != 1)
	// 					{
	// 						diagonal_move.push_back(diagonal_forward);
	// 						d_cnt--;
	// 					}
	// 					diagonal_move.push_back(diagonal_forward); // as we start at d_cnt = 1 and ends at d_cnt = 1
	// 					diagonal_move.push_back((d_end_direction == smooth_R) ? R_dia_turn : L_dia_turn);
	// 				}
	// 			}

	// 			cnt = 0;
	// 			d_cnt = 1;
	// 		}
	// 	}
	// }

	// printf(" robot moving direction \n");

	// while (!diagonal_move.empty())
	// {
	// 	printf("%c -> ", diagonal_move.front());
	// 	diagonal_move.pop_front();
	// }
	// printf("\n");

	return 0;
}