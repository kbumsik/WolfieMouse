#ifndef MAIN_LOGGER_H_
#define MAIN_LOGGER_H_

#include "main_control.h"
#include "range.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize logger thread
 */
void thread_logger_loop_init(void);

/**
 * @brief Log micromouse data
 *
 * @param pid       PID setpoint data
 * @param range     Data from range finder
 * @param step      Travel distance data
 * @param speed     Wheel speed data
 * @param outputT   Transitional speed
 * @param outputR   Rotational speed
 * @return int      0 on success, otherwise it faild due to
 *                  the writer being busy. Failed call is not a critical error.
 */
int logger_log (
    struct main_pid *pid,
    struct range_data *range,
    struct step_data *step,
    struct speed_data *speed,
    int32_t outputT,
    int32_t outputR);

#ifdef __cplusplus
}
#endif

#endif /* MAIN_LOGGER_H_ */
