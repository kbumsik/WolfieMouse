#ifndef MAIN_FSM_TABLE_HPP_
#define MAIN_FSM_TABLE_HPP_

// event enum
enum event { b1_pressed, b2_pressed, wheel_up, wheel_down, eol };

// state enum
enum state { state_1, state_2, state_3, state_4, state_5, state_6 };

// FSM action functions
void task_1(void);
void task_2(void);
void task_3(void);
void task_4(void);
void task_5(void);
void task_6(void);

void main_fsm_run_task(enum event event_input);

#endif /* MAIN_FSM_TABLE_HPP_ */
