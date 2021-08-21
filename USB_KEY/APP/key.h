#ifndef __KEY_H__
#define __KEY_H__
#include "gpio.h"

//#define KEY_PIN_OUTPUT_OD //管脚开漏输出

#ifdef KEY_PIN_OUTPUT_OD
    /*管脚开漏输出 并且外部上拉*/
    //行PB7~4  列PB3~0
    #define WRITE_KEY_ROW(x)    GPIOB->BSRR = ( ((~(uint32_t)x << 20) | ((uint32_t)x << 4)) & 0x00F000F0 )
    #define WRITE_KEY_COLUMN(x) GPIOB->BSRR = ( ((~(uint32_t)x << 16) | ((uint32_t)x << 0)) & 0x000F000F )
#else
    /*管脚输入输出转换*/
    //B7~4上拉输入，B3~0推挽输出0
    //#define SCAN_KEY_ROW        GPIOA->CRL = 0x88883333; GPIOA->BSRR = 0x000F00F0;
    //B3~0上拉输入，B7~4推挽输出0    
    //#define SCAN_KEY_COLUMN     GPIOB->CRL = 0x33338888; GPIOB->BSRR = 0x00F0000F;
    
    /*管脚输入输出转换*/
    //A7~0上拉输入，B15~0推挽输出0  
    #define SCAN_KEY_ROW        GPIOA->CRL = 0x88888888; GPIOA->BSRR = 0x000000FF; \
                                GPIOB->CRL = 0x33333333; GPIOB->CRH = 0x33333333; GPIOB->BSRR = 0xFFFF0000;
    //B15~0上拉输入，A7~0推挽输出0  
    #define SCAN_KEY_COLUMN     GPIOA->CRL = 0x33333333; GPIOA->BSRR = 0x00FF0000; \
                                GPIOB->CRL = 0x88888888; GPIOB->CRH = 0x88888888; GPIOB->BSRR = 0x0000FFFF;
#endif

//#define READ_KEY_ROW        ((GPIOB->IDR & 0x00F0) >> 4)
//#define READ_KEY_COLUMN     (GPIOB->IDR & 0x000F)
#define READ_KEY_ROW        (GPIOA->IDR & 0x00FF)
#define READ_KEY_COLUMN     (GPIOB->IDR & 0xFFFF)

//#define KEY_ROW_IDLE        0xF    
//#define KEY_COLUMN_IDLE     0xF
#define KEY_ROW_IDLE        0xFF    
#define KEY_COLUMN_IDLE     0xFFFF

#define KEY_DEBOUNCE_TIME   10   //消抖时间 10ms
#define KEY_SCAN_TIME       1    //行扫描完切换到列扫描的延时时间1ms
#define KEY_LONG_PRESS_TIME 5000 //长按时间 5s     

/*==========================*/
void key_init(void);
int key_scan(void); 
void key_timer(uint16_t ms);

#endif





