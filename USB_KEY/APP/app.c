#include "tim.h"
#include "adc.h"
#include "usbd_hid.h"
#include "key.h"

//ADC
#define ADC_TIMER_MS        10//ADC定时采样
#define ADC_CHANNEL_NUM     5

//USB
#define USB_SEND_TIMER_MS   15//USB发送定时时间

//旋转开关
#define ROTARY_SWITCH_READ_TIME_MS      5 //读取时间

//USB
extern USBD_HandleTypeDef hUsbDeviceFS;
uint8_t USB_send_buff[HID_EPIN_SIZE]; //发送缓存
uint32_t s_usb_timer; //时间

//ADC
enum{
    ADC_DMA_IDLE,
    ADC_DMA_RUN,
    ADC_DMA_HALF_DONE,
    ADC_DMA_ALL_DONE,
};
uint32_t s_adc_timer; //时间
uint16_t s_adc_dma_buff[ADC_CHANNEL_NUM];    //DMA缓存
int s_adc_status = ADC_DMA_IDLE;//ADC状态

//旋转开关
uint32_t s_rotary_switch_time = 0;

//------------函数---------------
void app_init(void)
{
    HAL_TIM_Base_Start_IT(&htim4); //启动TIME
    HAL_ADCEx_Calibration_Start(&hadc1); //校准ADC
    key_init();
}

void app_loop(void)
{
    static int key_value = -1, key_value_old = -1;
    static uint16_t usb_key_buff_len = HID_EPIN_SIZE - ADC_CHANNEL_NUM - 1; //5个字节旋钮，1个字节旋转开关，剩余为按键
    
    //按键
    key_value = key_scan();
    if(key_value != key_value_old)
    {
        //按键值改变
        key_value_old = key_value;
        if(key_value == -1){
            //按键松开了
            memset(USB_send_buff, ADC_CHANNEL_NUM + 1, usb_key_buff_len); //清除键值
        }else{
            //按键按下
            uint16_t bit = key_value % 8; //位   位置
            uint16_t byte = key_value / 8;//字节 位置
            
            uint16_t index = ADC_CHANNEL_NUM + 1 + byte;
            if(index < HID_EPIN_SIZE){
                USB_send_buff[index] = 0x01 << bit; //按键对应的位
            }
        }
    }
    
    //旋转开关
    if(s_rotary_switch_time == 0)
    {
        s_rotary_switch_time = ROTARY_SWITCH_READ_TIME_MS;
        
        uint8_t temp = 0;
        if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_15) == 0){
            temp &= 0xF8;
            temp |= 0x01;
        }else if(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_13) == 0){
            temp &= 0xF8;
            temp |= 0x02;
        }else{
            temp &= 0xF8;
            temp |= 0x04;
        }
        
        if(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_14) == 0){
            temp &= 0xC7;
            temp |= 0x08;
        }else if(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_15) == 0){
            temp &= 0xC7;
            temp |= 0x10;
        }else{
            temp &= 0xC7;
            temp |= 0x20;
        }
        
        USB_send_buff[ADC_CHANNEL_NUM] = temp;
    }
    
    //ADC
    if(s_adc_timer == 0 && s_adc_status == ADC_DMA_IDLE)
    {
        //启动一次AD采样
        HAL_ADC_Start_DMA(&hadc1, (uint32_t *)s_adc_dma_buff, ADC_CHANNEL_NUM);
        s_adc_timer = ADC_TIMER_MS;
    }
    if(s_adc_status == ADC_DMA_ALL_DONE)
    {
        //采样完成
        s_adc_status = ADC_DMA_IDLE;
        
        for(uint8_t i=0; i<ADC_CHANNEL_NUM; i++){
            USB_send_buff[i] = s_adc_dma_buff[i] >> 2;
        }
    }
    
    //USB
    if(s_usb_timer == 0)
    {
        s_usb_timer = USB_SEND_TIMER_MS;
        USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t*)&USB_send_buff, sizeof(USB_send_buff));
    }
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
        
        if(s_adc_timer > 0) 
            s_adc_timer--;
        
        if(s_usb_timer > 0) 
            s_usb_timer--;
        
        if(s_rotary_switch_time > 0)
            s_rotary_switch_time--;
    }
}

//------------DMA中断---------------
//DMA缓存满回调
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    s_adc_status = ADC_DMA_ALL_DONE;
}
