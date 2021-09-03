#include "tim.h"
#include "usbd_hid.h"
#include "key.h"

uint8_t KeyBoard[16] = {0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0};
extern USBD_HandleTypeDef hUsbDeviceFS;

void app_init(void)
{
    //启动TIME
    HAL_TIM_Base_Start_IT(&htim4); 
    key_init();
}

void app_loop(void)
{
    static int key_value = -1, key_value_old = -1;
    key_value = key_scan();
    
    if(key_value != key_value_old)
    {
        //按键值改变
        key_value_old = key_value;
        if(key_value == -1)
        {
            //按键松开了
            memset(KeyBoard, 0, sizeof(KeyBoard));
            USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t*)&KeyBoard, sizeof(KeyBoard));
        }
        else
        {
            //发送键值
            uint16_t bit = key_value % 8; //位   位置
            uint16_t byte = key_value / 8;//字节 位置
            
            KeyBoard[12 + byte] = 0x01 << bit; //按键对应的位
            
            USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t*)&KeyBoard, sizeof(KeyBoard));
        }
    }
    
    HAL_Delay(1);
}

//------------定时器----------------
/**
* @brief	定时器溢出回调
* @param	
* @retval	
* @note		定时器6为1ms
*/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim == (&htim4))
    {
        key_timer(1);
    }
}
