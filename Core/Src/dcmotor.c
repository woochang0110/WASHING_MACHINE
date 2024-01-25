#if 0
#include "dcmotor.h"
#include "button.h"
#include "i2c_lcd.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

//============function
void dcmotor_auto_manual_control(void);
void dcmotor_pwm_manual_control(void);
void dcmotor_pwm_auto_control(void);
void ledbar_display(void);
void lcd_display(void);
void led_display(void);

//main.c
extern TIM_HandleTypeDef htim4;
//led.c
extern volatile int t1ms_counter; //volatile: for disable optimize
extern void HAL_SYSTICK_Handler(void);//interrupt handler
//lcd.c
extern void delay_us (unsigned long us);//func from main.c
extern void move_cursor(uint8_t row, uint8_t column);
extern void lcd_string(uint8_t *str);
extern void lcd_command(uint8_t command);
//============variable
uint8_t led_bar[LEDBAR]={0x01,0x03,0x07,0x0f,0x1f,0x3f,0x7f,0xff};
uint8_t pwm_start_flag=0;//press btn0-> start/stop indicator
volatile int CCR_REVERSE_flag=0;//초기상태 정회전
volatile uint16_t CCR_Value=69;
volatile int FANRunningTime=10;
volatile int auto_manual_select=0;
//main.c
extern volatile int TIM10_1s_counter;

//uint16_t CCR_UP_Value=0;//16bit timer의 channel control register /  press btn1-> pwm up control
//uint16_t CCR_DOWN_Value=0;//16bit timer의 channel control register /  press btn1-> pwm DOWN control
/*
 * BTN0: start/stop   LED1 Toggle Yellow
 * BTN1: speed up	LED2 Toggle Blue
 * BTN2: speed down	LED1 Toggle Red
 * */


void dcmotor_auto_manual_control(void)
{
	if(get_button(BUTTON_4_GPIO_Port,BUTTON_4_Pin,4)==0)//PC13 BTN4-STM32 Blue button
	{
		auto_manual_select=(auto_manual_select+1)%2;//0: manual(default) / 1: auto
		printf("BTN4: %d",auto_manual_select);
		lcd_command(CLEAR_DISPLAY);//lcd 화면 clear
		FANRunningTime=10;
	}

	if(!auto_manual_select)//0 manual
	{
		dcmotor_pwm_manual_control();
		HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);//PB0이 stm32LED1
	}
	else if(auto_manual_select) //  1 auto
	{
		dcmotor_pwm_auto_control();
		HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);//PB7이 stm32LED2

	}
	ledbar_display();
	lcd_display();
	led_display();

}

void dcmotor_pwm_manual_control(void)
{
	//default 동작 로직
	if(pwm_start_flag)//동작상태라면
	{
		HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);//4번타이머의 PWM CH1번에 연결된 모터를 돌리는 거임.
		if(CCR_REVERSE_flag)//정회전 상태
		{
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 0);//모터드라이버 IN1 =0
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 1);//모터드라이버 IN2 =1
		}

		else//역회전 상태
		{
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 1);//모터드라이버 IN1 =0
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 0);//모터드라이버 IN =1
		}

	}
	else//정지상태라면
	{
		HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_1);//모터를 멈추는 거임.
	}

	//버튼0 로직
	if(get_button(BUTTON_0_GPIO_Port,BUTTON_0_Pin,0)==0)//PE15 BTN0
	{

		if(!pwm_start_flag)//이전 상태가 정지상태였다면
		{
			printf("start! flag: %d\n",pwm_start_flag);
			pwm_start_flag=1;//pwm flag 1로

		}
		else
		{
			printf("stop! flag: %d\n",pwm_start_flag);
			pwm_start_flag=0;//pwm flag 0으로

		}

	}
	//버튼1 로직
	if(get_button(BUTTON_1_GPIO_Port,BUTTON_1_Pin,1)==0)//PE14 BTN1
	{
		//CCR_Value=__HAL_TIM_GET_COMPARE(&htim4, TIM_CHANNEL_1);
		if(CCR_Value < 99)//over the threshold
		{
			CCR_Value+=10;
			printf("CCRUP:%d\n",CCR_Value);
			__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1,CCR_Value);//값을 넣어주는거임.

		}
	}
	//버튼2 로직
	if(get_button(BUTTON_2_GPIO_Port,BUTTON_2_Pin,2)==0)//PE12 BTN2
	{
		//CCR_Value=__HAL_TIM_GET_COMPARE(&htim4, TIM_CHANNEL_1);
		HAL_GPIO_WritePin(GPIOD, 0xff, 0);

		if(CCR_Value > 29)//under the threshold
		{
			CCR_Value-=10;
			printf("CCRDOWN:%d\n",CCR_Value);
			__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1,CCR_Value);//값을 넣어주는거임.
		}
	}
	//버튼3 로직
	if(get_button(BUTTON_3_GPIO_Port,BUTTON_3_Pin,3)==0)//PE10 BTN3
	{

		if(!CCR_REVERSE_flag)//정회전 상태였다면 역회전으로
		{
			CCR_REVERSE_flag=1;//현재 상태 역회전
			printf("reverse flag: %d\n",CCR_REVERSE_flag);
		}

		else//역회전 상태였다면 정회전으로
		{
			CCR_REVERSE_flag=0;//현재 상태 정회전
			printf("forward flag: %d\n",CCR_REVERSE_flag);
		}
	}
}

void dcmotor_pwm_auto_control(void)
{
	CCR_Value=69;//auto 모드에서 CCR 값 고정
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);//4번타이머의 PWM CH1번에 연결된 모터를 돌리는 거임.
	__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1,CCR_Value);//값을 넣어주는거임.
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);//4번타이머의 PWM CH1번에 연결된 모터를 돌리는 거임.
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 0);//모터드라이버 IN1 =0
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 1);//모터드라이버 IN2 =1

	if(TIM10_1s_counter>=FANRunningTime)//사용자가 설정한 시간만큼 동작을 완료하였다면
	{
		TIM10_1s_counter=0;
		HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_1);//모터를 멈추는 거임.
	}
/*
	if(get_button(BUTTON_0_GPIO_Port,BUTTON_0_Pin,0)==0)//PE15 BTN0
	{

		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 0);//모터드라이버 IN1 =0
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 1);//모터드라이버 IN2 =1

		if(!pwm_start_flag)//처음 눌렀다면
		{
			printf("start! flag: %d\n",pwm_start_flag);
			pwm_start_flag=1;//pwm flag 1로
			t1ms_counter=0; //1ms 카운터 초기화
			CCR_Value=69;//CCR 값 초기화
			HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);//4번타이머의 PWM CH1번에 연결된 모터를 돌리는 거임.

		}
		else
		{
			printf("stop! flag: %d\n",pwm_start_flag);
			pwm_start_flag=0;//pwm flag 0으로
			HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_1);//모터를 멈추는 거임.
		}

	}
*/
	if(get_button(BUTTON_1_GPIO_Port,BUTTON_1_Pin,1)==0)//PE14 BTN1
	{
		FANRunningTime+=10;
	}
	if(get_button(BUTTON_2_GPIO_Port,BUTTON_2_Pin,2)==0)//PE12 BTN2
	{
		FANRunningTime-=10;
	}
}

/*
void led_display(void)
{
	if(pwm_start_flag)
	{
		if(!CCR_REVERSE_flag)//정회전
		{
			if(t1ms_counter %50 == 0)//systick handler 1ms Timer
			{
				HAL_GPIO_TogglePin(FAN_LED_GPIO_Port, FAN_LED_Pin);//PE13이 FAN_LED
			}
		}
		else//역회전
		{
			if(t1ms_counter %100 == 0)
			{
				HAL_GPIO_TogglePin(FAN_LED_GPIO_Port, FAN_LED_Pin);//PE13이 FAN_LED
			}
		}
	}
	else
	{
		HAL_GPIO_WritePin(FAN_LED_GPIO_Port, FAN_LED_Pin,0);//PE13이 FAN_LED
	}
}

void ledbar_display(void)
{
	int ind;
	if(pwm_start_flag)
	{
		ind=(CCR_Value-29)/10;//29~99사이의 pwm값을 8칸의 배열에 맞게 index화 해줌.
		//systick Handler 1ms Timer
		if(t1ms_counter %20 == 0)//200번째 인터럽트라면 아래 로직을 동작시켜라. 인터럽트 핸들러는 1ms마다 동작하기 때문에 결국 200ms HAL_Delay와 같은 동작이 되나 HW는 계속 동작하게끔 하는거임.
		{
			HAL_GPIO_WritePin(GPIOD, led_bar[ind], 1);
		}
	}
	else
	{
		HAL_GPIO_WritePin(GPIOD, 0xff, 0);
	}
}


void lcd_display(void)
{
	//char lcd0_buff[20],lcd1_buff[20];
	if(!auto_manual_select)//0 manual
	{
		if(pwm_start_flag) //FAN ON
		{
			//sprintf(lcd0_buff,"Manual Mode ON!");
		}
		else// FAN OFF
		{
			sprintf(lcd0_buff,"Manual Mode OFF!");
		}
		//sprintf(lcd1_buff,"CCR: %d %",CCR_Value);
	}
	else//1 auto
	{
		sprintf(lcd0_buff,"AUTO Mode");

		sprintf(lcd1_buff,"Time: %d sec",FANRunningTime);
	}
	move_cursor(0,0);
	lcd_string(lcd0_buff);

	move_cursor(1,0);
	lcd_string(lcd1_buff);



}
*/
#endif
