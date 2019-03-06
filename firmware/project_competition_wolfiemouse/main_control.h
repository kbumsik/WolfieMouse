#ifndef MAIN_CONTROL_H_
#define MAIN_CONTROL_H_

#include "pid.h"

#ifdef __cplusplus
extern "C" {
#endif

// PID handler
struct main_pid {
    pid_handler_t tran;
    pid_handler_t rot;
};

// Target steps
struct step_data {
    uint32_t left;
    uint32_t right;
};

// Speed information
struct speed_data {
    int32_t left;
    int32_t right;
    int32_t diff;    // left - right
};

extern void (*const main_control_driver[])(struct main_pid *pid);


#ifdef __cplusplus
}
#endif

#endif /* MAIN_CONTROL_H_ */
