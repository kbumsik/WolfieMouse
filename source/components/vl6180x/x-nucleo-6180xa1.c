/*******************************************************************************
Copyright © 2015, STMicroelectronics International N.V.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of STMicroelectronics nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND
NON-INFRINGEMENT OF INTELLECTUAL PROPERTY RIGHTS ARE DISCLAIMED.
IN NO EVENT SHALL STMICROELECTRONICS INTERNATIONAL N.V. BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
********************************************************************************/

/*
 * @file x-nucleo-6180xa1.c
 * $Date: 2015-11-12 14:43:01 +0100 (Thu, 12 Nov 2015) $
 * $Revision: 2619 $
 */

#include <string.h>

/** Configure VL6180 pins  to Output */
#include "stm32f4xx_hal.h"
#ifndef HAL_I2C_MODULE_ENABLED
#define HAL_I2C_MODULE_ENABLED
#pragma message("hal conf should enable i2c")
#endif

#include "x-nucleo-6180xa1.h"

#define V1_CHIP_SWITCH_PORT GPIOA
#define V1_CHIP_SWITCH_PIN  GPIO_PIN_7
#define V1_CHIP_ENABLE_PORT GPIOA
#define V1_CHIP_ENABLE_PIN  GPIO_PIN_5

#define V1_IRQ_PIN          GPIO_PIN_6
#define V2_IRQ_PIN          GPIO_PIN_0

#ifdef __STM32F4xx_HAL_H 
    #define V1_IRQ EXTI9_5_IRQn
    #define V2_IRQ EXTI0_IRQn
#else 
#ifdef __STM32L4xx_HAL_H
    #define V1_IRQ EXTI9_5_IRQn
    #define V2_IRQ EXTI0_IRQn
#else
    #define V1_IRQ EXTI4_15_IRQn
    #define V2_IRQ EXTI0_1_IRQn
#endif
#endif



/* local private prototypes */
static void _V1_GPIO_Init(void);
static void _V1_Set7SegmentDP( int dp);
static void _V1_Set7Segment(uint8_t value);
static void _V1_DisplayOff(void);


static void _V2_GPIO_Init(void);
static int _V2_GetSwicth(void);


#define EXPANDER_I2C_ADDRESS    (0x42*2)
#define GPMR    0x10
#define GPSR    0x12
#define GPDR    0x14

#define V2_D1       (1<<7)
// second byte or word MSB
#define V2_D2       (1<<8)
#define V2_D3       (1<<9)
#define V2_D4       (1<<10)
#define V2_DISP_SEL (1<<11)
#define V2_CHIPEN   (1<<12)
#define V2_CHIPEN_B (1<<13)
#define V2_CHIPEN_L (1<<14)
#define V2_CHIPEN_R (1<<15)

static int IsV2=0;
I2C_HandleTypeDef *_hi2c;
uint16_t _V2PadVal; /* gpio SR value caching to avoid reading on each new bit set */
static int I2cExpAddr=EXPANDER_I2C_ADDRESS;

static int _err=0;

static void _V2_DisplayOff(void);
static void _V2_SetChipEn( int No, int state );



int XNUCLEO6180XA1_IsV2(void){
		return IsV2; 
}



int XNUCLEO6180XA1_EXTI_CallBackHandle(uint16_t GPIO_Pin){
    int IsVL6180XIntPin = IsV2 ?  GPIO_Pin == V2_IRQ_PIN : GPIO_Pin == V1_IRQ_PIN;
    if( IsVL6180XIntPin){
        XNUCLEO6180XA1_UserIntHandler();
    }
    return IsVL6180XIntPin;
}

void XNUCLEO6180XA1_Reset(int state){
    if( !IsV2)
        HAL_GPIO_WritePin(V1_CHIP_ENABLE_PORT, V1_CHIP_ENABLE_PIN , (GPIO_PinState)state);
    else{
        _V2_SetChipEn(0, state);
    }
}


int XNUCLEO6180XA1_ResetId(int state, int id)
{
    int status;
    if( IsV2 && id>=0 && id<4){
        _V2_SetChipEn(id, state);
        status=0;
    }
    else{
    /* Error case */
        status=-1;
    }
    return status;
}

int XNUCLEO6180XA1_GetSwitch(void){
    GPIO_PinState state ;
    if( !IsV2)
        state = HAL_GPIO_ReadPin(V1_CHIP_SWITCH_PORT, V1_CHIP_SWITCH_PIN);
    else{
        state= _V2_GetSwicth() ? GPIO_PIN_SET : GPIO_PIN_RESET ;
    }
    return state;
}

/* public API */
void XNUCLEO6180XA1_GPIO_Init(void) {
//    this must be done after i2c init _V1_GPIO_Init();
}



int V2_ExpanderRd(int  index,  uint8_t *data, int n_data){

    int status;
    uint8_t RegAddr;
    RegAddr=index;

    do{
        status=HAL_I2C_Master_Transmit(_hi2c, I2cExpAddr, &RegAddr, 1, 100);
        if( status )
            break;
        status =HAL_I2C_Master_Receive(_hi2c, I2cExpAddr, data, n_data, n_data*100);
    }while(0);
    return status;
}

int V2_ExpanderWR( int index,  uint8_t *data, int n_data){

    int status;
    uint8_t RegAddr[0x10];
    RegAddr[0]=index;
    memcpy(RegAddr+1, data, n_data);
    status=HAL_I2C_Master_Transmit(_hi2c, I2cExpAddr, RegAddr, n_data+1, 100);
    return status;
}


/**
 *
 * @param No    0= top , 1= Left, 2=Bottom 3=Right
 * @param state
 */
static void _V2_SetChipEn( int No, int state ){
    //int mask = V2_CHIPEN << No;
    int mask = (No==3) ? V2_CHIPEN_R : ((No==2) ? V2_CHIPEN_B : ((No==1) ? V2_CHIPEN_L : V2_CHIPEN));
    if( state)
        _V2PadVal|=mask ;
    else
        _V2PadVal&=~mask;

    V2_ExpanderWR(GPSR, (uint8_t*)&_V2PadVal,2);
}

int _V2_GetSwicth(){
    int status;
    uint16_t Value;
    status=V2_ExpanderRd(GPMR, (uint8_t*)&Value,2);

    if(status ==0 ){
        Value&=V2_DISP_SEL;
    }
    else{
        //TODO ("VL6180x Expander rd fail");
        _err++;
        Value=0;
    }
    return Value;
}

/**
 * F401 valid may not work for over device
 */
void XNUCLEO6180XA1_I2C1_Init(I2C_HandleTypeDef *hi2c1) {
    GPIO_InitTypeDef GPIO_InitStruct;
    uint8_t ExpanderID[2];

    int status;
    /* Peripheral clock enable */
    __GPIOB_CLK_ENABLE();
    __I2C1_CLK_ENABLE();

    /**I2C1 GPIO Configuration
     PB8     ------> I2C1_SCL
     PB9     ------> I2C1_SDA
     */
    GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    hi2c1->Instance = I2C1;
#ifdef __STM32F4xx_HAL_H
    hi2c1->Init.ClockSpeed = 400000;
    hi2c1->Init.DutyCycle = I2C_DUTYCYCLE_2;
#else 
    hi2c1->Init.Timing = 0x00300F38; /* set 400KHz fast mode i2c*/
#endif
    hi2c1->Init.OwnAddress1 = 0;
    hi2c1->Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1->Init.DualAddressMode = I2C_DUALADDRESS_DISABLED;
    hi2c1->Init.OwnAddress2 = 0;
    hi2c1->Init.GeneralCallMode = I2C_GENERALCALL_DISABLED;
    hi2c1->Init.NoStretchMode = I2C_NOSTRETCH_DISABLED;
    
    HAL_I2C_Init(hi2c1);
    
    
    /* try to detect v1 v2 by reading i2c expander register
     *  if rd i2c fail assume it is a v1 shield it may be a v2 with an i2c bus hangs (happen some time)*/
    _hi2c=hi2c1;
    status = V2_ExpanderRd( 0, ExpanderID, 2);
    if( status == 0 && ExpanderID[0]==0 && ExpanderID[1]==0x16){
        IsV2=1;
        _V2_GPIO_Init();
    }
    else{
        _V1_GPIO_Init();
    }
}

/**
 * @brief Default  do nothing interrupt callback
 */
#pragma weak XNUCLEO6180XA1_UserIntHandler
void  XNUCLEO6180XA1_UserIntHandler(void)  {
}

void XNUCLEO6180XA1_ClearInterrupt(void){
    if( !IsV2 )
        NVIC_ClearPendingIRQ(V1_IRQ);
    else
        NVIC_ClearPendingIRQ(V2_IRQ);
}


void XNUCLEO6180XA1_DisableInterrupt(void){
    if( !IsV2){
        HAL_NVIC_DisableIRQ(V1_IRQ);
        __HAL_GPIO_EXTI_CLEAR_IT(V1_IRQ_PIN);
        NVIC_ClearPendingIRQ(V1_IRQ);
    }
    else{
        HAL_NVIC_DisableIRQ(V2_IRQ);
        __HAL_GPIO_EXTI_CLEAR_IT(V2_IRQ_PIN);
        NVIC_ClearPendingIRQ(V2_IRQ);
    }

}
void XNUCLEO6180XA1_EnableInterrupt(void){
    if( !IsV2 ){
        __HAL_GPIO_EXTI_CLEAR_IT(V1_IRQ_PIN);
        NVIC_ClearPendingIRQ(V1_IRQ);
        HAL_NVIC_EnableIRQ(V1_IRQ);
    }
    else{
        __HAL_GPIO_EXTI_CLEAR_IT(V2_IRQ_PIN);
        NVIC_ClearPendingIRQ(V2_IRQ);
        HAL_NVIC_EnableIRQ(V2_IRQ);
    }

}

/**
 * Display digit enable pad/pin LUT
 * D1 is left most digit  (ms nibble) on the display
 * D3 is right most digit (ls nibble) on the display
 */
#define PA  0
#define PB  1
#define PC  2
#define MAKE_PAD(port, pin)  (((port)<<4)|pin)

/* digit enable pad/pin LUT  */
static uint8_t DisplayEn[] = {
    MAKE_PAD( PB, 0), /* D1 0 */
    MAKE_PAD( PA, 4), /* D2 1 */
    MAKE_PAD( PA, 1), /* D3 2 */
    MAKE_PAD( PA, 0), /* D4 3 */
};

/* 7 segment code to pad/pin LUT  */
static uint8_t DisplaySeg[] = {
        MAKE_PAD( PB, 6 ), /* 0 A  */
        MAKE_PAD( PC, 7 ), /* 1 B  */
        MAKE_PAD( PA, 9 ), /* 2 C  */
        MAKE_PAD( PA, 8 ), /* 3 D  */
        MAKE_PAD( PB, 10), /* 4 E  */
        MAKE_PAD( PB, 4 ), /* 5 F  */
        MAKE_PAD( PB, 5 ), /* 6 G  */
        MAKE_PAD( PB, 3 ), /* 7 DP */
        /* Note that PB3 these is conflicting wit jtag/SWO used for trace
         * activating PB3 as ooutput will prevent trace to work  */
};
#undef MAKE_PAD
#undef PA
#undef PB
#undef PC

#define pad_write(pad, state)   HAL_GPIO_WritePin( (void*)((char*)GPIOA + (pad>>4)*((char*)GPIOB-(char*)GPIOA)), 1<<(pad&0x0F), state);

/*
   --s0--
   s    s
   5    1
   --s6--
   s    s
   4    2
   --s3-- s7

 */

#define DP  (1<<7)
#define NOT_7_NO_DP( ... ) (uint8_t) ~( __VA_ARGS__ + DP )
#define S0 (1<<0)
#define S1 (1<<1)
#define S2 (1<<2)
#define S3 (1<<3)
#define S4 (1<<4)
#define S5 (1<<5)
#define S6 (1<<6)

/* refer to http://www.twyman.org.uk/Fonts/ */
static const uint8_t ascii_to_display_lut[256]={
      [' ']=           0,
      ['-']=           S6,
      ['_']=           S3,
      ['=']=           S3+S6,
      ['~']=           S0+S3+S6, /* 3 h bar */
      ['^']=           S0, /* use as top bar */

      ['?']=           NOT_7_NO_DP(S5+S3+S2),
      ['*']=           NOT_7_NO_DP(),
      ['[']=           S0+S3+S4+S5,
      [']']=           S0+S3+S2+S1,
      ['@']=           S0+S3,

      ['0']=           NOT_7_NO_DP(S6),
      ['1']=           S1+S2,
      ['2']=           S0+S1+S6+S4+S3,
      ['3']=           NOT_7_NO_DP(S4+S5),
      ['4']=           S5+S1+S6+S2,
      ['5']=           NOT_7_NO_DP(S1+S4),
      ['6']=           NOT_7_NO_DP(S1),
      ['7']=           S0+S1+S2,
      ['8']=           NOT_7_NO_DP(0),
      ['9']=           NOT_7_NO_DP(S4),
    
      ['a']=           S2+ S3+ S4+ S6 ,
      ['b']=           NOT_7_NO_DP(S0+S1),
      ['c']=           S6+S4+S3,
      ['d']=           NOT_7_NO_DP(S0+S5),
      ['e']=           NOT_7_NO_DP(S2),
      ['f']=           S6+S5+S4+S0, /* same as F */
      ['g']=           NOT_7_NO_DP(S4), /* same as 9 */
      ['h']=           S6+S5+S4+S2,
      ['i']=           S4,
      ['j']=           S1+S2+S3+S4,
      ['k']=           S6+S5+S4+S2, /* a h */
      ['l']=           S3+S4,
      ['m']=           S0+S4+S2, /* same as  */
      ['n']=           S2+S4+S6,
      ['o']=           S6+S4+S3+S2,
      ['p']=           NOT_7_NO_DP(S3+S2), // same as P
      ['q']=           S0+S1+S2+S5+S6,
      ['r']=           S4+S6,
      ['s']=           NOT_7_NO_DP(S1+S4),
      ['t']=           NOT_7_NO_DP(S0+S1+S2),
      ['u']=           S4+S3+S2+S5+S1, // U
      ['v']=           S4+S3+S2, // is u but u use U
      ['w']=           S1+S3+S5,
      ['x']=           NOT_7_NO_DP(S0+S3), // similar to H
      ['y']=           NOT_7_NO_DP(S0+S4),
      ['z']=           S0+S1+S6+S4+S3, // same as 2
      
      ['A']=           NOT_7_NO_DP(S3),
      ['B']=           NOT_7_NO_DP(S0+S1), /* as b  */
      ['C']=           S0+S3+S4+S5, // same as [
      ['E']=           NOT_7_NO_DP(S1+S2),
      ['F']=           S6+S5+S4+S0,
      ['G']=           NOT_7_NO_DP(S4), /* same as 9 */
      ['H']=           NOT_7_NO_DP(S0+S3),
      ['I']=           S1+S2,
      ['J']=           S1+S2+S3+S4,
      ['K']=           NOT_7_NO_DP(S0+S3), /* same as H */
      ['L']=           S3+S4+S5,
      ['M']=           S0+S4+S2, /* same as  m*/
      ['N']=           S2+S4+S6, /* same as n*/
      ['O']=           NOT_7_NO_DP(S6),
      ['P']=           S0+S1+S2+S5+S6, // sames as 'q'
      ['Q']=           NOT_7_NO_DP(S3+S2),
      ['R']=           S4+S6,
      ['S']=           NOT_7_NO_DP(S1+S4), /* sasme as 5 */
      ['T']=           NOT_7_NO_DP(S0+S1+S2), /* sasme as t */
      ['U']=           NOT_7_NO_DP(S6+S0),
      ['V']=           S4+S3+S2, // is u but u use U
      ['W']=           S1+S3+S5,
      ['X']=           NOT_7_NO_DP(S0+S3), // similar to H
      ['Y']=           NOT_7_NO_DP(S0+S4),
      ['Z']=           S0+S1+S6+S4+S3, // same as 2


};

#undef S0
#undef S1
#undef S2
#undef S3
#undef S4
#undef S5
#undef S6
#undef DP


static void _V1_DisplayString(const char *str, int SegDelayMs){
    int i;
    const char *pc;
    for( i=0, pc=str; i<4 && *pc!=0 ; i++, pc++){
        _V1_Set7Segment( ascii_to_display_lut[(uint8_t)*pc]);
        if( *(pc+1)== '.'){
            _V1_Set7SegmentDP(1);
            pc++;
        }
        /* digit on */
        pad_write(DisplayEn[i], GPIO_PIN_RESET); /* activate led */
        XNUCLEO6180XA1_WaitMilliSec(SegDelayMs);
        /* digit off */
        pad_write(DisplayEn[i], GPIO_PIN_SET); /* turn off led */
    }
}

static  void _V2_Set7Segment( int Leds, int digit ){
    _V2PadVal |= 0x7F; /* clear 7 seg bits */
    _V2PadVal |= V2_D1|V2_D2|V2_D3|V2_D4; /* all segment off */
    _V2PadVal &= ~(V2_D1<<digit);         /* digit on */
    _V2PadVal &= ~(Leds&0x7F);
    V2_ExpanderWR(GPSR, (uint8_t*)&_V2PadVal, 2);
}


static  void _V2_DisplayString(const char *str, int SegDelayMs){
    int i;
    const char *pc;

    for( i=0, pc=str; i<4 && *pc!=0 ; i++, pc++){
        _V2_Set7Segment( ascii_to_display_lut[(uint8_t)*pc], i);
        if( *(pc+1)== '.'){
            pc++;
        }
        XNUCLEO6180XA1_WaitMilliSec(SegDelayMs);
        _V2_DisplayOff();
    }
}

/**
 * accept . on any digit
 */
void XNUCLEO6180XA1_DisplayString(const char *str, int SegDelayMs){
    if( !IsV2)
        _V1_DisplayString(str, SegDelayMs);
    else
        _V2_DisplayString(str, SegDelayMs);
 }




static void _V1_GPIO_Init(void) {

    GPIO_InitTypeDef GPIO_InitStruct;

    /* GPIO Ports Clock Enable */
    __GPIOC_CLK_ENABLE();
    __GPIOH_CLK_ENABLE();
    __GPIOA_CLK_ENABLE();
    __GPIOB_CLK_ENABLE();


    /*Configure GPIO pins : PA0 PA1 PA4 PA5 PA8 PA9 */
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_8 | GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /*Configure GPIO pins : PA7 select  */
    GPIO_InitStruct.Pin = GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);


    /*Configure GPIO pins : PA6 interrupt */
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /*Configure GPIO pins : PB0 PB10 PB3 PB4 PB5  PB6 */
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_10 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6;
#ifndef OS_USE_TRACE_ITM
    GPIO_InitStruct.Pin |=  GPIO_PIN_3;
#endif
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);



    /*Configure GPIO pin : PC7 7 segment  B */
    GPIO_InitStruct.Pin = GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    /* turn off all 7 seg digit */
    _V1_DisplayOff();
}



static void _V2_GPIO_Init(){
    uint16_t PadDir;

    GPIO_InitTypeDef GPIO_InitStruct;

    __GPIOA_CLK_ENABLE();
    __GPIOB_CLK_ENABLE();
    
    /*Configure GPIO pins interrupt PA4 = INTB , PA5 = INTL, pA10 = INTR */
    GPIO_InitStruct.Pin = GPIO_PIN_4 | GPIO_PIN_5 |GPIO_PIN_10 ;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);


    /* PB0 = INT */
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* expender config */
    PadDir=~V2_DISP_SEL;
    V2_ExpanderWR(GPDR, (uint8_t*)&PadDir, 2);

    _V2_DisplayOff();

}

static void _V2_DisplayOff() {
    _V2PadVal |= (V2_D1|V2_D2|V2_D3| V2_D4); /* segment en off */
    V2_ExpanderWR(GPSR, (uint8_t*)&_V2PadVal, 2);
}

static void _V1_DisplayOff() {
    int i;
    for( i=0; i<4; i++ ){
        pad_write(DisplayEn[i], GPIO_PIN_SET); /* turn off led */
    }
}

/* set 7 segment bus to display decode "value" */
static void _V1_Set7Segment(const uint8_t value) {
    int i;
    GPIO_PinState state;
    for (i = 0; i < sizeof(DisplaySeg)/sizeof(DisplaySeg[0]); i++) {
        int cur_bit;
        cur_bit = value & (1 << i);
        state = cur_bit ?  GPIO_PIN_RESET: GPIO_PIN_SET; /* port clear= segment on */
        pad_write(DisplaySeg[i], state);
    }
}

/* set state of decimal point on 7 segment bus */
static void _V1_Set7SegmentDP( int dp_state) {
    GPIO_PinState state;


    state = dp_state ?  GPIO_PIN_RESET: GPIO_PIN_SET; // port clear= segment on
    pad_write(DisplaySeg[7], state);
    //HAL_GPIO_WritePin((void*)GPIOA + DisplaySeg[7].port*((void*)GPIOB-(void*)GPIOA), 1<<DisplaySeg[7].pin, state);
}



