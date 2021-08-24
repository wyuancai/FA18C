#include "key.h"

uint32_t s_key_timer, s_key_delay;//长按时间，防抖延时

uint8_t s_is_key_press = 0;

//准备扫描行
void switch_row_scan(void)
{
#ifdef KEY_PIN_OUTPUT_OD                  
    WRITE_KEY_ROW(0xF);
    WRITE_KEY_COLUMN(0x0);
#else
    SCAN_KEY_ROW;
#endif 
}

//准备扫描列
void switch_column_scan(void)
{
#ifdef KEY_PIN_OUTPUT_OD  
    WRITE_KEY_ROW(0x0);
    WRITE_KEY_COLUMN(0xF);
#else
    SCAN_KEY_COLUMN;
#endif    
}

void key_init(void)
{
    switch_row_scan();  
}

/**
* //键值=行值*16+列值
* //输出-1无按键按下
*/
int key_scan(void)
{
    static int s_key_value = -1, s_key_output = -1;
    uint16_t temp;
    
    if(s_key_delay == 0) //延时时间到
    {
        s_key_delay = KEY_SCAN_TIME;
        
        if(s_is_key_press == 0) 
        {/*刚刚按下*/
            if(READ_KEY_ROW != KEY_ROW_IDLE)
            {
                s_is_key_press = 1; //有按键按下
            }
            s_key_timer = 0;//长按时间复位
            s_key_value = -1;
            s_key_output = -1;
            return -1;
        }
        else if(s_is_key_press == 1)
        {/*读行*/
            temp = READ_KEY_ROW;
            
            if(temp != KEY_ROW_IDLE) 
            {//按着的
                switch(temp)//判断已读的行
                {
                    case(KEY_ROW_IDLE ^ 0x01): s_key_value = 0x00; break;
                    case(KEY_ROW_IDLE ^ 0x02): s_key_value = 0x10; break;
                    case(KEY_ROW_IDLE ^ 0x04): s_key_value = 0x20; break;
                    case(KEY_ROW_IDLE ^ 0x08): s_key_value = 0x30; break;
                    case(KEY_ROW_IDLE ^ 0x10): s_key_value = 0x40; break;
                    case(KEY_ROW_IDLE ^ 0x20): s_key_value = 0x50; break;
                    case(KEY_ROW_IDLE ^ 0x40): s_key_value = 0x60; break;
                    case(KEY_ROW_IDLE ^ 0x80): s_key_value = 0x70; break;
                    default:   
                        s_is_key_press = 0; 
                        s_key_output = -1;
                        return -1;//错误
                }
                
                //准备读列
                s_is_key_press = 2;
                switch_column_scan();
            }
        }
        else
        {/*读列*/
            temp = READ_KEY_COLUMN;
            
            //准备读行 
            switch_row_scan();
            
            if(temp != KEY_COLUMN_IDLE)
            {
                switch(temp) //判断已读的列
                {
                    case(KEY_COLUMN_IDLE ^ 0x0001): s_key_value = s_key_value + 0; break;
                    case(KEY_COLUMN_IDLE ^ 0x0002): s_key_value = s_key_value + 1; break;
                    case(KEY_COLUMN_IDLE ^ 0x0004): s_key_value = s_key_value + 2; break;
                    case(KEY_COLUMN_IDLE ^ 0x0008): s_key_value = s_key_value + 3; break;
                    case(KEY_COLUMN_IDLE ^ 0x0010): s_key_value = s_key_value + 4; break;
                    case(KEY_COLUMN_IDLE ^ 0x0020): s_key_value = s_key_value + 5; break;
                    case(KEY_COLUMN_IDLE ^ 0x0040): s_key_value = s_key_value + 6; break;
                    case(KEY_COLUMN_IDLE ^ 0x0080): s_key_value = s_key_value + 7; break;
                    case(KEY_COLUMN_IDLE ^ 0x0100): s_key_value = s_key_value + 8; break;
                    case(KEY_COLUMN_IDLE ^ 0x0200): s_key_value = s_key_value + 9; break;
                    case(KEY_COLUMN_IDLE ^ 0x0400): s_key_value = s_key_value + 10; break;
                    case(KEY_COLUMN_IDLE ^ 0x0800): s_key_value = s_key_value + 11; break;
                    case(KEY_COLUMN_IDLE ^ 0x1000): s_key_value = s_key_value + 12; break;
                    case(KEY_COLUMN_IDLE ^ 0x2000): s_key_value = s_key_value + 13; break;
                    case(KEY_COLUMN_IDLE ^ 0x4000): s_key_value = s_key_value + 14; break;
                    case(KEY_COLUMN_IDLE ^ 0x8000): s_key_value = s_key_value + 15; break;
                    default:   
                        s_is_key_press = 0; 
                        s_key_output = -1;
                        return -1;//错误
                }
                
                s_is_key_press = 1;
                
                if(s_key_output != -1 && s_key_output != s_key_value)
                {
                    //按了另一个键
                    s_key_output = -1;
                    s_is_key_press = 0;
                    return -1;
                }
                
                s_key_output = s_key_value; //一遍扫描完成记录键值
            }         
        }
    }
    
    //持续按下时间超出防抖时间
    if(s_key_timer >= KEY_DEBOUNCE_TIME && s_is_key_press != 0)
    {/*正常按下*/
        return s_key_output;
    }else{
        return -1;
    }
}

/* 定时器 */
void key_timer(uint16_t ms)
{
    if(s_key_timer < KEY_LONG_PRESS_TIME)
        s_key_timer += ms;
    
    if(s_key_delay >= ms)
        s_key_delay -= ms;
    else if(s_key_delay > 0)
        s_key_delay = 0;
}

