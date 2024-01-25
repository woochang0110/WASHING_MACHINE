/*
 * led.c for HAL_SYSTIC_Timer
 *
 *  Created on: Oct 5, 2023
 *      Author: USER
 */

#include <led_HALSYSTICK_TIM.h>
#include "button.h"

enum NUM_OF_FUNCTION{
    LED_ALL_ON,
	LED_ALL_OFF,
	FLOWER_ON,
	FLOWER_OFF,
	LED_KEEPON_UP,
	LED_KEEPON_DOWN,
	LED_ON_UP,
	LED_ON_DOWN
};


void led_main(void);
void led_all_on(void);
void led_all_off(void);
void led_keepon_up(void);
void led_keepon_down(void);
void led_on_up(void);
void led_on_down(void);
void flower_on(void);
void flower_off(void);
void button0_toggle(void);

void (*funcp[FUNC_NUM])(void) =
{
led_all_on,led_all_off,flower_on,flower_off,led_keepon_up,led_keepon_down,led_on_up,led_on_down
};

extern volatile int t1ms_counter; //volatile: for disable optimize
extern void HAL_SYSTICK_Handler(void);//interrupt handler

int func_index=0;

void led_main(void)
{
	while(1)
	{

#if 1
		button0_toggle();

#else
		switch(func_index)
		{
		case LED_ON_UP:led_on_up(); break;
		case LED_ON_DOWN:led_on_down(); break;
		default: break;



		}
#endif
#if 0
		if(t1ms_counter >= 200)
		{
			t1ms_counter=0;
			HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_0);
			HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_7);
			HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_14);

		}
#endif
#if 0
		HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_0);
		HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_7);
		HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_14);
		HAL_Delay(200);//HAL Delay 하는 동안에는 버튼이 안먹는다.
#endif
	}

}






//1번 누르면 전체 led켜지고 , 2번 누르면 전체 led 꺼지고 , 3번째 눌리면 flower_on, 4번째 눌리면 flower_off 5번째 눌리면 led keepon_up 6번째 눌리면 led keepon_down 7번째 눌리면 led on_up 8번째 눌리면 led on_down
void button0_toggle(void)
{
	static int button0_count=-1; //static으로 함수 return 후에도 값 저장(전역변수처럼 동작)
	if(button0_count==-1)//200번째 인터럽트라면 아래 로직을 동작시켜라. 인터럽트 핸들러는 1ms마다 동작하기 때문에 결국 200ms HAL_Delay와 같은 동작이 되나 HW는 계속 동작하게끔 하는거임.
	{
		HAL_GPIO_WritePin(GPIOD, 0xff, 0);//PD값들 0으로
	}
	if(get_button(BUTTON_0_GPIO_Port,BUTTON_0_Pin,0)==BUTTON_PRESS)
	{

		button0_count++;
		button0_count=button0_count%8;
		func_index=button0_count;
		printf("%d\n",func_index);
	}

	funcp[func_index]();
}

void led_all_on(void)
{
	if(t1ms_counter >= 200)//200번째 인터럽트라면 아래 로직을 동작시켜라. 인터럽트 핸들러는 1ms마다 동작하기 때문에 결국 200ms HAL_Delay와 같은 동작이 되나 HW는 계속 동작하게끔 하는거임.
	{					   //동작 구조 때문에 for문을 없애줘야 하고 i를 기억하고 있어야 함.
		t1ms_counter=0;
		HAL_GPIO_WritePin(GPIOD, 0xff, 1);//PD값들 1로
	}
}


void led_all_off(void)
{
	if(t1ms_counter >= 200)//200번째 인터럽트라면 아래 로직을 동작시켜라. 인터럽트 핸들러는 1ms마다 동작하기 때문에 결국 200ms HAL_Delay와 같은 동작이 되나 HW는 계속 동작하게끔 하는거임.
	{			   //동작 구조 때문에 for문을 없애줘야 하고 i를 기억하고 있어야 함.
		t1ms_counter=0;
		HAL_GPIO_WritePin(GPIOD, 0xff, 0);//PD값들 0으로
	}
}

void led_keepon_up(void)
{


	static int i=0;
	if(t1ms_counter >= 200)//200번째 인터럽트라면 아래 로직을 동작시켜라. 인터럽트 핸들러는 1ms마다 동작하기 때문에 결국 200ms HAL_Delay와 같은 동작이 되나 HW는 계속 동작하게끔 하는거임.
	{					   //동작 구조 때문에 for문을 없애줘야 하고 i를 기억하고 있어야 함.
		t1ms_counter=0;
		if(i%8==0)
		{
			HAL_GPIO_WritePin(GPIOD, 0xff, 0);
			i=0;
		}
		HAL_GPIO_WritePin(GPIOD, 0xff, 0);
		HAL_GPIO_WritePin(GPIOD, 0x00|(0x01<<i), 1);
		i++;


	}


}

void led_keepon_down(void)
{


	static int i=0;
		if(t1ms_counter >= 200)//200번째 인터럽트라면 아래 로직을 동작시켜라. 인터럽트 핸들러는 1ms마다 동작하기 때문에 결국 200ms HAL_Delay와 같은 동작이 되나 HW는 계속 동작하게끔 하는거임.
		{					   //동작 구조 때문에 for문을 없애줘야 하고 i를 기억하고 있어야 함.
			t1ms_counter=0;
			if(i%8==0)
			{

				HAL_GPIO_WritePin(GPIOD, 0xff, 0);
				i=0;
			}
			HAL_GPIO_WritePin(GPIOD, 0xff, 0);
			HAL_GPIO_WritePin(GPIOD, 0x00|(0x80>>i), 1);
			i++;
		}



}

void led_on_up(void)
{
	static int i=0;
	if(t1ms_counter >= 200)//200번째 인터럽트라면 아래 로직을 동작시켜라. 인터럽트 핸들러는 1ms마다 동작하기 때문에 결국 200ms HAL_Delay와 같은 동작이 되나 HW는 계속 동작하게끔 하는거임.
	{					   //동작 구조 때문에 for문을 없애줘야 하고 i를 기억하고 있어야 함.
		t1ms_counter=0;
		if(i%8==0)
		{
			HAL_GPIO_WritePin(GPIOD, 0xff, 0);
			i=0;
		}
		HAL_GPIO_WritePin(GPIOD, 0x01<<i, 1);
		i++;

	}

}

void led_on_down(void)
{

	static int i=0;
	if(t1ms_counter >= 200)//200번째 인터럽트라면 아래 로직을 동작시켜라. 인터럽트 핸들러는 1ms마다 동작하기 때문에 결국 200ms HAL_Delay와 같은 동작이 되나 HW는 계속 동작하게끔 하는거임.
	{					   //동작 구조 때문에 for문을 없애줘야 하고 i를 기억하고 있어야 함.
		t1ms_counter=0;
		if(i%8==0)
		{
			HAL_GPIO_WritePin(GPIOD, 0xff, 0);
			i=0;
		}
		HAL_GPIO_WritePin(GPIOD, 0x80>>i, 1);
		i++;


	}


}


//0->7까지 해당되는 bit의 LED만 ON
void flower_on(void)
{
	static int i=0;
	if(t1ms_counter >= 200)//200번째 인터럽트라면 아래 로직을 동작시켜라. 인터럽트 핸들러는 1ms마다 동작하기 때문에 결국 200ms HAL_Delay와 같은 동작이 되나 HW는 계속 동작하게끔 하는거임.
	{					   //동작 구조 때문에 for문을 없애줘야 하고 i를 기억하고 있어야 함.
		t1ms_counter=0;
		if(i%4==0)
		{
			HAL_GPIO_WritePin(GPIOD, 0xff, 0);
			i=0;
		}
		HAL_GPIO_WritePin(GPIOD, 0x08>>i, 1);
		HAL_GPIO_WritePin(GPIOD, 0x10<<i, 1);
		i++;


	}


}
//7->0까지 해당되는 bit의 LED만 ON
void flower_off(void)
{

	static int i=0;
	if(t1ms_counter >= 200)//200번째 인터럽트라면 아래 로직을 동작시켜라. 인터럽트 핸들러는 1ms마다 동작하기 때문에 결국 200ms HAL_Delay와 같은 동작이 되나 HW는 계속 동작하게끔 하는거임.
	{					   //동작 구조 때문에 for문을 없애줘야 하고 i를 기억하고 있어야 함.
		t1ms_counter=0;
		if(i%4==0)
		{
			HAL_GPIO_WritePin(GPIOD, 0xff, 1);
			i=0;
		}
		HAL_GPIO_WritePin(GPIOD, 0x01<<i, 0);//PD값들 0으로
		HAL_GPIO_WritePin(GPIOD, 0x80>>i, 0);//PD값들 0으로
		i++;

	}


}
