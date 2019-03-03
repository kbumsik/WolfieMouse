#include "main_fsm_table.hpp"
#include "hcms_290x_display.h"

// Simple display tasks
static void disp_task_1(void);
static void disp_task_2(void);
static void disp_task_3(void);
static void disp_task_4(void);
static void disp_task_5(void);
static void disp_task_6(void);

/*******************************************************************************
 * State machine definitions
 ******************************************************************************/
// transition and table
struct transition {
    enum event event_val;
    void (*task)(void);
    enum state next_state;
};

// stopwatch current state
enum state current_state = state_1;

// fsm function
void main_fsm_run_task(enum event event_input)
{
    static const struct transition state_1_transitions[] = {
    //  Event        Task             Next_state
        {b1_pressed, task_1,          state_1   },
        {b2_pressed, task_1,          state_1   },
        {wheel_up,   disp_task_2,     state_2   },
        {wheel_down, disp_task_6,     state_6   },
        {eol,        disp_task_1,     state_1   }
    };
    static const struct transition state_2_transitions[] = {
    //  Event        Task           Next_state
        {b1_pressed, task_2,        state_2     },
        {b2_pressed, task_2,        state_2     },
        {wheel_up,   disp_task_3,   state_3     },
        {wheel_down, disp_task_1,   state_1     },
        {eol,        disp_task_2,   state_2     }
    };
    static const struct transition state_3_transitions[] = {
    //  Event        Task           Next_state
        {b1_pressed, task_3,        state_3     },
        {b2_pressed, task_3,        state_3     },
        {wheel_up,   disp_task_4,   state_4     },
        {wheel_down, disp_task_2,   state_2     },
        {eol,        disp_task_3,   state_3     }
    };
    static const struct transition state_4_transitions[] = {
    //  Event        Task           Next_state
        {b1_pressed, task_4,        state_4     },
        {b2_pressed, task_4,        state_4     },
        {wheel_up,   disp_task_5,   state_5     },
        {wheel_down, disp_task_3,   state_3     },
        {eol,        disp_task_4,   state_4     }
    };
    static const struct transition state_5_transitions[] = {
    //  Event        Task           Next_state
        {b1_pressed, task_5,        state_5     },
        {b2_pressed, task_5,        state_5     },
        {wheel_up,   disp_task_6,   state_6     },
        {wheel_down, disp_task_4,   state_4     },
        {eol,        disp_task_5,   state_5     }
    };
    static const struct transition state_6_transitions[] = {
    //  Event        Task           Next_state
        {b1_pressed, task_6,        state_6     },
        {b2_pressed, task_6,        state_6     },
        {wheel_up,   disp_task_1,   state_1     },
        {wheel_down, disp_task_5,   state_5     },
        {eol,        disp_task_6,   state_6     }
    };
    // FSM table. Since it is const, it will be stored in FLASH
    static const struct transition *fsm_table[6] = {
        state_1_transitions, state_2_transitions, state_3_transitions,
        state_4_transitions, state_5_transitions, state_6_transitions,
    };

    // search for signal
    int i;
    for (i = 0; (fsm_table[current_state][i].event_val != event_input)
                && (fsm_table[current_state][i].event_val != eol);
         i++) {
        // Do nothing until hit a transition
    };

    // call task function and than change state
    current_state = fsm_table[current_state][i].next_state;
    fsm_table[current_state][i].task();
    return;
}

/*******************************************************************************
 * Task display
 ******************************************************************************/
static void disp_task_1(void)
{
    hcms_290x_matrix("TSK1");
}

static void disp_task_2(void)
{
    hcms_290x_matrix("TSK2");
}

void disp_task_3(void)
{
    hcms_290x_matrix("TSK3");
}

void disp_task_4(void)
{
    hcms_290x_matrix("TSK4");
}

void disp_task_5(void)
{
    hcms_290x_matrix("TSK5");
}

void disp_task_6(void)
{
    hcms_290x_matrix("TSK6");
}
