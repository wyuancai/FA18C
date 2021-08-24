#include "tim.h"
#include "usbd_hid.h"
#include "key.h"

uint8_t KeyBoard[8] = {0,0,4,0,0,0,0,0};
extern USBD_HandleTypeDef hUsbDeviceFS;

void app_init(void)
{
    //启动TIME
    HAL_TIM_Base_Start_IT(&htim4); 
    key_init();
}

void app_loop(void)
{
    int key_value = -1, key_value_old = -1;
    key_value = key_scan();
    
    if(key_value != key_value_old)
    {
        //按键值改变
        key_value_old = key_value;
        if(key_value == -1)
        {
            //按键松开了
            memset(KeyBoard, 0, 8);
            USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t*)&KeyBoard, sizeof(KeyBoard));
        }
        else
        {
            //发送键值
            uint16_t residual = key_value % 36; //(a-z 1-9 0)
            uint16_t divide = key_value / 36;
            
            if(divide == 0){
                KeyBoard[0] = 0x07; //左Control Shift Alt
            }else if(divide == 1){
                KeyBoard[0] = 0x70; //右Control Shift Alt
            }
            
            KeyBoard[2] = residual + 4; //从4开始对应 [a-z 1-9 0]
            
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
