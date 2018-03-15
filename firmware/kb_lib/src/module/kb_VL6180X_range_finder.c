/*
 * kb_VL6180X_range_finder.c
 *
 *  Created on: Nov 3, 2016
 *      Author: Bumsik Kim
 */

#include "kb_VL6180X_range_finder.h"
#include "kb_tick.h"
#include "kb_i2c.h"

#include "vl6180x/vl6180x_api.h"
#include "vl6180x/vl6180x_platform.h" /* contain all device/platform specific code */

// base name change. Used with kb_msg(). See @kb_base.h
#ifdef KB_MSG_BASE
    #undef KB_MSG_BASE
    #define KB_MSG_BASE "VL6180X"
#endif

static const VL6180xDev_t dev_addr_ = 0x29;
#define TIMEOUT_	(100)

inline static void init_device_variable_(VL6180xDev_t dev)	//MyDev_Init(myDev);
{
    return;	// not used becasue we use single driver now
}
inline static void sleep_us_(uint32_t time) //MyDev_uSleep(1000);
{
    kb_delay_us(time);
}
inline static void chip_enable_(VL6180xDev_t dev) //MyDev_SetChipEnable(myDev);
{
    return;	// not used because CE pin is not used
}


int VL6180x_I2CRead(VL6180xDev_t addr, uint8_t *buff, uint8_t len) {
    int status;
    status = kb_i2c_receive_timeout(VL6180X_I2C, addr, buff, len, TIMEOUT_);
    return status;
}


int VL6180x_I2CWrite(VL6180xDev_t addr, uint8_t *buff, uint8_t len) {
    int status;
    status = kb_i2c_send_timeout(VL6180X_I2C, addr, buff, len, TIMEOUT_);
    return status;
}


int  vl6180x_init(void)
{
    kb_i2c_sda_pin(VL6180X_I2C, VL6180X_SDA_PORT, VL6180X_SDA_PIN, PULLUP);
    kb_i2c_scl_pin(VL6180X_I2C, VL6180X_SCL_PORT, VL6180X_SCL_PIN, PULLUP);

    kb_i2c_init_t i2c_setting = {
            .frequency = 400000
    };
    kb_status_t result = kb_i2c_init(VL6180X_I2C, &i2c_setting);
    if (result != KB_OK)
    {
        return result;
    }

    //MyDev_Init(myDev);           // your code init device variable
    init_device_variable_(dev_addr_);
    //MyDev_SetChipEnable(myDev);  // your code assert chip enable
    chip_enable_(dev_addr_);
    //MyDev_uSleep(1000);          // your code sleep at least 1 msec
    sleep_us_(1000);

    VL6180x_InitData(dev_addr_);
    VL6180x_Prepare(dev_addr_);

    VL6180x_RangeClearInterrupt(dev_addr_); // make sure no interrupt is pending
    return 0;
}


int32_t  vl6180x_range_mm(void)
{
    VL6180x_RangeData_t Range;
    VL6180x_RangePollMeasurement(dev_addr_, &Range);
    if (Range.errorStatus == 0 )
    {
        // Do something?
    }
    else
    {
        KB_DEBUG_MSG("Range error msg:%lu", Range.errorStatus);
    }
    return Range.range_mm;
}


uint32_t vl6180x_als_lux(void)
{
    VL6180x_AlsData_t Als;
    VL6180x_AlsPollMeasurement(dev_addr_, &Als);
    if (Als.errorStatus == 0 )
    {
        // Do something?
    }
    else
    {
        KB_DEBUG_MSG("ALS error msg:%lu", Als.errorStatus);
    }
    return Als.lux;
}


void  vl6180x_start_freerun(void)
{
    /* kick off the first measurement */
    VL6180x_RangeStartSingleShot(dev_addr_);
}



int32_t  vl6180x_freerun_range_mm(void)
{
    VL6180x_RangeData_t Range;
    int status;
    int WaitedLoop;
    do {
        // TODO add your code anything in a loop way
        VL6180x_PollDelay(dev_addr_); // simply  run default API poll delay that handle display in demo
        // check for range measure availability
        status= VL6180x_RangeGetMeasurementIfReady(dev_addr_, &Range);
        if( status == 0 )
        {
            /* we have the new measure that was ready */
            if (Range.errorStatus == 0 )
            {
                // Do something?
            }
            else
            {
                KB_DEBUG_MSG("Range error msg:%lu", Range.errorStatus);
            }
            /* re-arm next measurement */
            VL6180x_RangeStartSingleShot(dev_addr_);

            // Then return
            if(WaitedLoop > 0)
            {
                KB_DEBUG_MSG("%d loops waited", WaitedLoop);
            }
            WaitedLoop=0;
            return Range.range_mm;
        }
        else if( status ==  NOT_READY)
        {
            /* measure was not ready  */
            WaitedLoop++;
        }
        else if( status <0 )
        {
            KB_DEBUG_ERROR("Critical Error:%d", status);
            return 0;
        }
    } while (1); // your code to stop looping
}

