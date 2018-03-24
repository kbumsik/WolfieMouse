#include "task_tests.hpp"





void task_test1_time(void)
{
    /* Give time to get ready */
    hcms_290x_matrix("RDY.");
    for (int i = 0; i < 6; i++) {
        kb_gpio_toggle(LED2_PORT, LED2_PIN);
        kb_delay_ms(500);
    }
    // TODO: stop blinky here
    hcms_290x_matrix("Go");
    kb_delay_ms(500);
    hcms_290x_matrix("    ");

    /* Motor test running */
    // system_enable_range_finder();
    system_start_driving();
    pid_input_setpoint(&g_pid_T, 18);

    kb_delay_ms(4000);
    /* Motor test running done */
    system_stop_driving();
    // system_disable_range_finder();
    pid_reset(&g_pid_T);
    pid_reset(&g_pid_R);

    main_fsm(eol);
}

void task_test1_distance(void)
{
    /* Give time to get ready */
    hcms_290x_matrix("RDY.");
    for (int i = 0; i < 6; i++) {
        kb_gpio_toggle(LED2_PORT, LED2_PIN);
        kb_delay_ms(500);
    }
    // TODO: stop blinky here
    hcms_290x_matrix("Go");
    kb_delay_ms(500);
    hcms_290x_matrix("    ");

    /* Motor test running */
    motion_cmd_t cmd;
    for (int i = 0; i < 6; i++) {
        cmd.type = motion_cmd_t::straight;
        cmd.unit = 1;
        cmd.on_start = NULL;
        cmd.on_completed = NULL;
        if (i == 5) {
            // Put an event at the end of moving
            cmd.on_completed = on_task_done;
        }
        motion_queue(&cmd);
    }

    // Sleep (wait) until task is done
    xSemaphoreTake(semphr_task, portMAX_DELAY);

    main_fsm(eol);
}

void task_test2_CW(void)
{
    /* Give time to get ready */
    hcms_290x_matrix("RDY.");
    for (int i = 0; i < 6; i++) {
        kb_gpio_toggle(LED2_PORT, LED2_PIN);
        kb_delay_ms(500);
    }
    // TODO: stop blinky here
    hcms_290x_matrix("Go");
    kb_delay_ms(500);
    hcms_290x_matrix("    ");

    /* Motor test running */
    motion_cmd_t cmd;
    for (int i = 0; i < 2; i++) {
        cmd.type = motion_cmd_t::turn;
        cmd.unit = -1;
        cmd.on_start = NULL;
        cmd.on_completed = NULL;
        if (i == 1) {
            // Put an event at the end of moving
            cmd.on_completed = on_task_done;
        }
        motion_queue(&cmd);
    }

    // Sleep (wait) until task is done
    xSemaphoreTake(semphr_task, portMAX_DELAY);

    main_fsm(eol);
}

void task_test2_CCW(void)
{

    /* Give time to get ready */
    hcms_290x_matrix("RDY.");
    for (int i = 0; i < 6; i++) {
        kb_gpio_toggle(LED2_PORT, LED2_PIN);
        kb_delay_ms(500);
    }
    // TODO: stop blinky here
    hcms_290x_matrix("Go");
    kb_delay_ms(500);
    hcms_290x_matrix("    ");

    /* Motor test running */
    motion_cmd_t cmd;
    for (int i = 0; i < 2; i++) {
        cmd.type = motion_cmd_t::turn;
        cmd.unit = 1;
        cmd.on_start = NULL;
        cmd.on_completed = NULL;
        if (i == 1) {
            // Put an event at the end of moving
            cmd.on_completed = on_task_done;
        }
        motion_queue(&cmd);
    }

    // Sleep (wait) until task is done
    xSemaphoreTake(semphr_task, portMAX_DELAY);

    main_fsm(eol);
}

void task_test3_walls(void)
{
    hcms_290x_matrix("Range");
    kb_delay_ms(2000);

    system_enable_range_finder();
    static volatile uint16_t * const sensors[] = {
            &range_F, &range_L, &range_R
    };
    static uint16_t detect_range[] = {
            MEASURE_RANGE_F_DETECT,
            MEASURE_RANGE_L_DETECT,
            MEASURE_RANGE_R_DETECT
    };
    static char display[5] = {' ', ' ', ' ', ' ', '\0'};
    is_b2_pressed = 0;
    while (!is_b2_pressed) {
        char *ptr = display;
        for (int i = 0; i < 3 ; i++) {
            *ptr++ = (*sensors[i] > detect_range[i])?'0':'X';
        }
        hcms_290x_matrix(display);
    }

    // end
    is_b2_pressed = 0;
    system_disable_range_finder();
    main_fsm(eol);
}

void task_test4(void)
{
    /* Not implemented */
    hcms_290x_matrix("None");
    kb_delay_ms(2000);
    main_fsm(eol);
}