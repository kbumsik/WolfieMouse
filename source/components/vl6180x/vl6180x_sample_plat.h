/*
 * vl6180x_sample_plat.h
 *
 * VL6180 sample code wraper for STM32 Nucleo F401re board  plus VL6180x shield
 */

#ifndef VL6180X_SAMPLE_PLAT_H_
#define VL6180X_SAMPLE_PLAT_H_

#include <stdio.h>
#include <stdarg.h>
#include <vl6180x_api.h>
#include <string.h>

#ifdef TRACE
#include "diag\trace.h"
#else 
#define trace_printf(...) (void)0
#endif

#include "stm32f4xx_hal.h"
#include "x-nucleo-6180xa1.h"

extern void debug_stuff(void);

#define MyDev_printf(fmt, ...) trace_printf(fmt, ##__VA_ARGS__)
#define MyDev_Init(dev) (dev=0x52)

#define MyDev_ClearInterruptCpu(...) /* nothing*/
#define MyDev_SetEvent(...) /* nothing */

#define MyDev_uSleep(x) XNUCLEO6180XA1_WaitMilliSec((x+999)/1000)
#define ShowTime             800 /* Time msg are displayed */


extern uint32_t TimeStarted;       /* various display and mode delay starting time */

void SetDisplayString(const char *msg);
extern int BSP_GetPushButton(void);
extern uint32_t g_TickCnt;

#define ALPHA (int)(0.85*(1<<16))
static inline void MyDev_ShowRange(VL6180xDev_t dev, int range, int duration){
#ifdef TEMP
    static char str[8]; // must remain valid until next display is done : this is why it cannot be a local variable
    static int Range; // must remain valid for leaky integrator
    // Apply leaky to smooth the display
    Range = (Range * ALPHA + range * ((1 << 16) - ALPHA)) >> 16;
    sprintf(str,"r%3d",Range);
    SetDisplayString(str);
    if(duration>0){
        // Keep message displayed during duration before returning
        TimeStarted=g_TickCnt;
        do {
            DISP_ExecLoopBody();
        } while (g_TickCnt - TimeStarted < duration);  
    }
#else
    printf("%d mm\r\n", range);
#endif
}

static inline void MyDev_ShowErr(VL6180xDev_t dev, int err){
    static char str[8];
    sprintf(str,"E-%02d",err);
    SetDisplayString(str);
}


static inline void MyDev_ShowLux(VL6180xDev_t dev, int lux){
    static char str[8];
    if( lux >= 1000)
        sprintf(str,"L%3d ",lux/1000);
    else
        sprintf(str,"l%3d ",lux);
    SetDisplayString(str);
}


static inline void MyDev_ShowIntr(char c){
	  static char IntStr[8];
    IntStr[0]='I';
    IntStr[1]=c;
    IntStr[2]=c;
    IntStr[3]=c;
    IntStr[4]=0;
    SetDisplayString(IntStr);
}

static inline int MyDev_UserSayStop(VL6180xDev_t dev){
    int stop=0;
    if( !BSP_GetPushButton() ){
        stop=1;
        while( !BSP_GetPushButton() ){
            //Do struff
        }
    }
    return stop;
}


static inline void MyDev_UserWait(VL6180xDev_t dev , const char * msg)
{
    int char_time=250;
    int len;
    int offset=0;
    int last_wait=0;

    uint32_t started;
    len=strlen(msg);

    started=HAL_GetTick();
    while( BSP_GetPushButton()  ){
        SetDisplayString(msg+offset);
        DISP_ExecLoopBody();
        if(HAL_GetTick()-started > char_time ){
            started=HAL_GetTick();
            if( offset+4>=len ){
                if( last_wait++ < 4 ){
                    continue;
                }
                last_wait=0;
                offset=0;
            }
            else{
                offset++;
            }
        }
    }
    SetDisplayString(" rb ");
    while( !BSP_GetPushButton() ){
        DISP_ExecLoopBody();
    }
}

static inline void MyDev_Printf(VL6180xDev_t dev , const char *fmt, ...)  {
    static char buffer[128];
    va_list ap;
    va_start(ap, fmt);
    vsprintf(buffer, fmt, ap);
    MyDev_UserWait(dev,buffer);
}

static inline void MyDev_SetChipEnable(VL6180xDev_t dev) {
        XNUCLEO6180XA1_Reset(0);
        XNUCLEO6180XA1_WaitMilliSec(5);
        XNUCLEO6180XA1_Reset(1);
        XNUCLEO6180XA1_WaitMilliSec(5);
}

/* enable interrupt at CPU level */
#define MyDev_EnableInterrupt(...) XNUCLEO6180XA1_EnableInterrupt()
#define MyDev_RestartInterrupt(...) IntrFired=0
extern volatile int IntrFired;

static inline void MyDev_WaitForEvent(VL6180xDev_t dev){
    uint32_t started=HAL_GetTick();
    do{
        DISP_ExecLoopBody();
        if( HAL_GetTick()-started > 200){
            SetDisplayString("----");
        }
        debug_stuff();
    } while(!IntrFired);
}


#define HandleError(...) \
	do{ \
		SetDisplayString("ErrF"); \
		while(1){ DISP_ExecLoopBody();} \
	}while(0)

#endif /* VL6180X_SAMPLE_PLAT_H_ */
