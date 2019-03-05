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

extern void (*const main_control_driver[])(struct main_pid *pid);


#ifdef __cplusplus
}
#endif

#endif /* MAIN_CONTROL_H_ */
