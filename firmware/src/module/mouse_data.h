#ifndef MODULE_MOUSE_DATA_H_
#define MODULE_MOUSE_DATA_H_

#include "pid.h"

#ifdef __cplusplus
extern "C" {
#endif

// PID handler
struct mouse_data_pid {
    pid_handler_t tran;
    pid_handler_t rot;
};

// Target steps
struct mouse_data_step {
    uint32_t left;
    uint32_t right;
};

// Speed information
struct mouse_data_speed {
    int32_t left;
    int32_t right;
    int32_t diff;    // left - right
};

#ifdef __cplusplus
}
#endif

#endif /* MODULE_MOUSE_DATA_H_ */
