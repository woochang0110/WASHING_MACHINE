#include "ultrasonic.h"

/*
 * 1.this Func FROM Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim.c
 * 2. when UltraSonic module ECHO PIN's Rising and Falling Edge occur, will come into this func
 */

void ultrasonic_processing(void);
void make_trigger(void);

extern void move_cursor(uint8_t row, uint8_t column);
extern void lcd_string(uint8_t *str);

extern volatile int TIM10_10ms_ultrasonic;
extern void delay_us(unsigned long us);
extern volatile int ultrasonic_func_flag;
extern uint8_t lcd_display_mode_flag;//0: RTC display   1: Humid&Temp  2: UltraSonic Distance


volatile uint8_t is_first_capture=0;	//0: rising edge, 1: falling edge
volatile int distance;//거리를 측정할 펄스 갯수를 저장하는 변수
volatile int ic_cpt_finish_flag=0;//input capture가 끝났을 때를 구분해줌.
volatile int dist;

__weak void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{

	if(htim->Instance == TIM3)
	{
		if(is_first_capture==0)//when US ECHO pin is rising edge
		{
			__HAL_TIM_SET_COUNTER(htim,0);// 해당 타이머의 HW Counter Register clear
			is_first_capture=1;	//rising edge.
		}
		else if(is_first_capture==1)//when US ECHO pin is falling edge
		{
			is_first_capture=0;//falling edge.
			distance = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
			//현재까지 카운트한 해당 타이머의 1번 채널의 값을 읽어오는 것
			ic_cpt_finish_flag=1;
		}


	}
  //UNUSED(htim);//
}

void ultrasonic_processing(void)
{

	char lcd3_buff[20];

	if(TIM10_10ms_ultrasonic>=100)// 1초가 되면
	{
		TIM10_10ms_ultrasonic=0;
		make_trigger();
		if(ic_cpt_finish_flag)//초음파 측정이 완료되었다면
		{
			ic_cpt_finish_flag=0;
			dist=distance;
			dist = dist*0.034 /2; // 1us가 0.034cm를 이동함& 왕복이 아니라 편도 거리를 구하려면 절반
			/*
			if(ultrasonic_func_flag)
			{
				printf("distance: %d cm\n",dist);
			}
			*/
			printf("dist: %d cm\n",dist);
			/*
			if(lcd_display_mode_flag==2)
			{
			sprintf(lcd3_buff,"distance: %d cm",dist);
			move_cursor(0,0);
			lcd_string(lcd3_buff);
			}
			*/
		}

	}
}

void make_trigger(void)//구형파를 날린거
{
	HAL_GPIO_WritePin(ULTRASONIC_TRIG_GPIO_Port, ULTRASONIC_TRIG_Pin, 0);//PF12 TRIG->0
	delay_us(2);
	HAL_GPIO_WritePin(ULTRASONIC_TRIG_GPIO_Port, ULTRASONIC_TRIG_Pin, 1);//PF12 TRIG->1
	delay_us(10);
	HAL_GPIO_WritePin(ULTRASONIC_TRIG_GPIO_Port, ULTRASONIC_TRIG_Pin, 0);//PF12 TRIG->0
	return;
}

