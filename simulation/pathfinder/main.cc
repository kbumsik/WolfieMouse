#include "Position.hpp"
#include "Queue.hpp"
#include "Maze.hpp"
#include "StdIO.hpp"

#include <iostream>
#include <vector>
#include <stdio.h>
#include <deque>

#define MAZE_FILE "maze_base.txt"

std::vector<Position> pathPos = {
	Position{15,0}, Position{14,0}, Position{13,0}, Position{12,0}, Position{11,0},
	Position{10,0}, Position{9,0}, 	Position{8,0}, 	Position{7,0}, 	Position{6,0},
	Position{5,0}, 	Position{4,0}, 	Position{3,0}, 	Position{2,0}, 	Position{1,0},
	Position{0,0}, 	Position{0,1}, 	Position{0,2}, 	Position{0,3}, 	Position{0,4},
	Position{0,5}, 	Position{0,6}, 	Position{0,7}, 	Position{0,8}, 	Position{0,9},
	Position{0,10},	Position{0,11}, Position{0,12}, Position{0,13}, Position{0,14},
	Position{0,15}, Position{1,15}, Position{2,15}, Position{3,15}, Position{4,15},
	Position{5,15}, Position{6,15}, Position{7,15}, Position{7,14}, Position{8,14},
	Position{8,13}, Position{9,13}, Position{9,12}, Position{9,11}, Position{9,10},
	Position{9,9},  Position{8,9}, 	Position{7,9}, 	Position{6,9}, 	Position{6,10},
	Position{6,11}, Position{5,11}, Position{5,10}, Position{5,9}, 	Position{5,8},
	Position{4,8}, 	Position{3,8}, 	Position{3,7}, 	Position{4,7}, 	Position{4,6},
	Position{3,6}, 	Position{3,5}, 	Position{4,5}, 	Position{5,5}, 	Position{6,5},
	Position{6,4}, 	Position{7,4}, 	Position{7,5}, 	Position{8,5}, 	Position{8,4},
	Position{9,4}, 	Position{10,4},	Position{11,4}, Position{12,4}, Position{13,4},
	Position{13,5}, Position{12,5}, Position{11,5}, Position{10,5}, Position{9,5},
	Position{9,6},  Position{8,6}
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
	smooth_L = 'L',
	R_dia_turn = 'r',
	L_dia_turn = 'l',
	diagonal_forward = 'D'
};

std::deque<enum direction> path; // position based path.

std::deque<enum real_move> move; // smooth_path

std::deque<enum real_move> diagonal_move; // diagonal path
std::deque<enum real_move> temp_move;

template<typename T>
void printPath(std::string name, T &path) {
	// print name
	std::cout << name << std::endl;

	char old_dir = path.front();
	int dup_count = 0;
	// Print path with duplication counting
	for (auto const& dir: path) {
		char new_dir = (char) dir;
		if (old_dir == new_dir) {
			dup_count++;
		} else {
			if (dup_count == 1) {
				std::cout << old_dir << " -> ";
			} else {
				std::cout << dup_count << old_dir << " -> ";
				dup_count = 1;
			}
		}
		old_dir = new_dir;
	}
	// print last one
	if (dup_count == 1) {
		std::cout << old_dir;
	} else {
		std::cout << dup_count << old_dir;
	}
	// double newline
	std::cout << std::endl << std::endl;
}

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
	
	// Create a maze object only for printing purpose
    StdIO fileIO(true);
    StdIO printIO(false);
    Maze maze(MAZE_FILE, &fileIO, &printIO);
	maze.printMazeWithPositions(pathPos);

	for (int i = 0; i < pathPos.size(); i++)
	{
		swap = (15 - (pathPos[i].row));
		pathPos[i].row = (pathPos[i].col);
		pathPos[i].col = swap;
	}
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

	printPath("Non-smooth", path);
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

	printPath("Smooth", move);

	// diagonal move//

	move.push_back(F);
	move.push_back(H_F);
	move.push_back(smooth_R);
	move.push_back(smooth_L);
	move.push_back(smooth_R);
	move.push_back(smooth_L);
	move.push_back(H_F);

	while (!move.empty())
	{
		if (move.front() == F)
		{
			diagonal_move.push_back(move.front());
			move.pop_front();
		}
		else if (move.front() == H_F)
		{

			diagonal_move.push_back(H_F); //insert H_F
			move.pop_front();
			d_start_direction = move.front();
			// before_direction = move.front();
			// d_start_direction = before_direction;
			while (move.front() == smooth_R || move.front() == smooth_L) // temp_move -> inserting all nearby Left/Right moving direction
			{
				temp_move.push_back(move.front());
				move.pop_front();
				cnt++;
			}

			if (cnt == 1) // 100% smooth turn
			{
				diagonal_move.push_back(temp_move.front());
				temp_move.pop_front();
				//diagonal_move.push_back(H_F);
				cnt--;
			}
			else if (cnt > 1) // various direction
			{
				while (cnt >= 1)
				{
					before_direction = temp_move.front();
					temp_move.pop_front();
					if (before_direction != temp_move.front()) // diagonal case counting
					{
						d_cnt++;
						before_direction = temp_move.front();
						d_end_direction = before_direction;
						temp_move.pop_front();
						cnt--;
					}
					else if (before_direction == temp_move.front() && d_cnt != 1) // diagonal case moving direction
					{
						diagonal_move.push_back((d_start_direction == smooth_R) ? R_dia_turn : L_dia_turn);

						while (d_cnt != 1)
						{
							diagonal_move.push_back(diagonal_forward);
							d_cnt--;
						}
						diagonal_move.push_back(diagonal_forward); // as we start at d_cnt = 1 and ends at d_cnt = 1
						diagonal_move.push_back((d_end_direction == smooth_R) ? R_dia_turn : L_dia_turn);
					}
					else if (before_direction == temp_move.front())
					{ // smooth turn case
						if ((diagonal_move.back() != L_dia_turn) && (diagonal_move.back() != R_dia_turn))
						{
							diagonal_move.push_back(temp_move.front());
							temp_move.pop_front(); //////////?????????
							if (temp_move.empty())
								diagonal_move.push_back(H_F); //////////?????????
							cnt--;
						}
						else if ((diagonal_move.back()) == (L_dia_turn) || (diagonal_move.back()) == (R_dia_turn))
						//diagonal case finish
						{
							//diagonal_move.push_back(temp_move.front());
							//temp_move.pop_front(); //////////?????????
							//diagonal_move.push_back(H_F); //////////?????????
							cnt--;
						}
					}

					else if (temp_move.empty() && d_cnt != 1)
					{
						diagonal_move.push_back((d_start_direction == smooth_R) ? R_dia_turn : L_dia_turn);
						while (d_cnt != 1)
						{
							diagonal_move.push_back(diagonal_forward);
							d_cnt--;
						}
						diagonal_move.push_back(diagonal_forward); // as we start at d_cnt = 1 and ends at d_cnt = 1
						diagonal_move.push_back((d_end_direction == smooth_R) ? R_dia_turn : L_dia_turn);
					}
				}

				cnt = 0;
				d_cnt = 1;
			}
		}
	}

	printPath("Diagonal", diagonal_move);
	return 0;
}