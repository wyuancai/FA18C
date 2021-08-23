#include "tim.h"
#include "usbd_hid.h"
#include "key.h"

uint8_t KeyBoard[8] = {0,0,4,0,0,0,0,0};
uint8_t KeyBoard01[8] = {0,0,0,0,0,0,0,0};
extern USBD_HandleTypeDef hUsbDeviceFS;

void app_init(void)
{
    //启动TIME
    HAL_TIM_Base_Start_IT(&htim4); 
    key_init();
}

void app_loop(void)
{
    //key_scan();
    if(KeyBoard[2] >= 29)
    {
        KeyBoard[2] = 4;
    }
    else
    {
        KeyBoard[2]++;
    }
    USBD_HID_SendReport(&hUsbDeviceFS,(uint8_t*)&KeyBoard01,sizeof(KeyBoard));
    HAL_Delay(15);
    USBD_HID_SendReport(&hUsbDeviceFS,(uint8_t*)&KeyBoard,sizeof(KeyBoard01));
    HAL_Delay(15);
    USBD_HID_SendReport(&hUsbDeviceFS,(uint8_t*)&KeyBoard01,sizeof(KeyBoard));
    HAL_Delay(1000);

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
