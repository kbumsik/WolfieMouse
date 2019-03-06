#ifndef MAIN_CONTROL_H_
#define MAIN_CONTROL_H_

#include "mouse_data.h"

#ifdef __cplusplus
extern "C" {
#endif

extern void (*const control_loop_driver[])(struct mouse_data_pid *pid);

#ifdef __cplusplus
}
#endif

#endif /* MAIN_CONTROL_H_ */
