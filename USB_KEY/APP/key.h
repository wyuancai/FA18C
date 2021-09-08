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
    //A5~10上拉输入，B15~0推挽输出0  
    #define SCAN_KEY_ROW        GPIOA->CRL = GPIOA->CRL & 0x000FFFFF | 0x88800000; \
                                GPIOA->CRH = GPIOA->CRH & 0xFFFFF000 | 0x00000888; \
                                GPIOA->BSRR = 0x000007E0; \
                                GPIOB->CRL = 0x33333333; GPIOB->CRH = 0x33333333; GPIOB->BSRR = 0xFFFF0000;
    //B15~0上拉输入，A5~10推挽输出0  
    #define SCAN_KEY_COLUMN     GPIOA->CRL = GPIOA->CRL & 0x000FFFFF | 0x33300000; \
                                GPIOA->CRH = GPIOA->CRH & 0xFFFFF000 | 0x00000333; \
                                GPIOA->BSRR = 0x07E00000; \
                                GPIOB->CRL = 0x88888888; GPIOB->CRH = 0x88888888; GPIOB->BSRR = 0x0000FFFF;
#endif

#define READ_KEY_ROW        ((GPIOA->IDR & 0x07E0) >> 5)    //读行
#define READ_KEY_COLUMN     (GPIOB->IDR & 0xFFFF)           //读列

#define KEY_ROW_IDLE        0x3F    //行空闲值
#define KEY_COLUMN_IDLE     0xFFFF  //列空闲值

#define KEY_DEBOUNCE_TIME   10   //消抖时间 10ms
#define KEY_SCAN_TIME       1    //行扫描完切换到列扫描的延时时间1ms
#define KEY_LONG_PRESS_TIME 5000 //长按时间 5s     

/*==========================*/
void key_init(void);
int key_scan(void); 
void key_timer(uint16_t ms);

#endif





