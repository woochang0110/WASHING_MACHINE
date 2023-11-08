#include"servomotor.h"
#include "main.h"
#define Degree180 125
#define Degree90 85
#define Degree0 45
void servo_motor_test_main(void);

extern TIM_HandleTypeDef htim2;
//84MHz를 1680-1로 분주하면 50kHz
//T= 20us를 PWM
//2ms(180도): 20us*100개 카운트
//1.5ms(90도): 20us*75개 카운트
//1ms(0도): 20us*50개 카운트





void servo_motor_test_main(void)
{
	 HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_1);
	while(1)
	{

		//__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,Degree90);//180도 회전 3번째 값이 pwm value
		HAL_Delay(1000);
//		__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,Degree90);//180도 회전 3번째 값이 pwm value
//		HAL_Delay(1000);
		__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,Degree0);//180도 회전
		//HAL_Delay(1000);

	}



}
