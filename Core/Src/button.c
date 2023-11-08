#include"button.h"
#include "i2c_lcd.h"

extern unsigned char bin2dec(unsigned char byte);
char button_status[BUTTON_NUMBER] =
{BUTTON_RELEASE,BUTTON_RELEASE,BUTTON_RELEASE,BUTTON_RELEASE,BUTTON_RELEASE,BUTTON_RELEASE,BUTTON_RELEASE,BUTTON_RELEASE};//각 버튼의 상태 (초기값들은 1 IDLE로 세팅)
uint8_t lcd_display_mode_flag=0;//0: RTC display   1: Humid&Temp  2: UltraSonic Distance

extern void get_rtc_time(void);


extern RTC_HandleTypeDef hrtc;
RTC_TimeTypeDef mTime; // time information

void lcd_display_mode_select(void);
int get_button(GPIO_TypeDef *GPIO,uint16_t GPIO_PIN,uint8_t button_number);//GPIO의 포트 type(PE),그 Type에서 어떤 PIN인지, 몇번 버튼인지

//get_button(gpio,pin,button) Argument check


//완전히 눌렀다 떼면 BUTTON_RELEASE(1)을 return


void lcd_display_mode_select(void)
{

	if(get_button(BUTTON_4_GPIO_Port,BUTTON_4_Pin,4)==BUTTON_PRESS)//PC13 BTN4-STM32 Blue button
	{
		for(int i=0; i<5; i++)
		{
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3,1);//부저 on
			HAL_Delay(100);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, 0);//부저 on
			HAL_Delay(100);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3,1);//부저 on
			HAL_Delay(300);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, 0);//부저 on
			HAL_Delay(200);
		}

		lcd_command(CLEAR_DISPLAY);
		lcd_display_mode_flag=(lcd_display_mode_flag+1)%4;
		if(lcd_display_mode_flag==3)
		{
			HAL_RTC_GetTime(&hrtc, &mTime, RTC_FORMAT_BCD);
		}

	}


}


int get_button(GPIO_TypeDef *GPIO,uint16_t GPIO_PIN,uint8_t button_number)
{
	unsigned char curr_state;//현재 PIN의 상태 UC로 저장
	curr_state=HAL_GPIO_ReadPin(GPIO,GPIO_PIN);
	//pin의 상태를 uc로 return 해줌 0,1
	if(curr_state==BUTTON_PRESS&&button_status[button_number]==BUTTON_RELEASE)
		//처음 Press라면 Delay로 noise 넘기기
	{
		HAL_Delay(100);//noise 지나가기를 기다림.
		button_status[button_number]=BUTTON_PRESS;///다음에 눌리면 동작할 수 있게 PRESS상태
		return BUTTON_RELEASE;
	}
	else if(curr_state==BUTTON_RELEASE&&button_status[button_number]==BUTTON_PRESS)
		//두번째 Press상태로 정상적으로 LED 점등
	{
		button_status[button_number]=BUTTON_RELEASE;//다음
		return BUTTON_PRESS; // 버튼을 한번 눌렀다 뗀것으로 인정
	}
	return BUTTON_RELEASE;

}



