#include "main.h"
#include "fnd4digit.h"
#include "button.h"

//extern volatile int last_time;	//남은 동작시간 from washing machine.c
extern volatile int TIM10_fnd1ms_counter;  // fnd1ms from main.c TIM INT
extern volatile unsigned int last_time;	//남은 동작시간 from washing machine.c
volatile int CCR_REVERSE_flag=0;
extern volatile int run_flag;

void FND_update(unsigned int value);
void FND4digit_off(void);
void FND4digit_on(void);
void fnd4digit_sec_min_clock();



//자릿수 선택
uint16_t FND_digit[4] =
{
	FND_d1, FND_d10, FND_d100, FND_d1000
};

//0~9까지의 값 표현
uint32_t FND_font[10] =
{
  FND_a|FND_b|FND_c|FND_d|FND_e|FND_f,   // 0
  FND_b|FND_c,                           // 1
  FND_a|FND_b|FND_d|FND_e|FND_g,         // 2
  FND_a|FND_b|FND_c|FND_d|FND_g,         // 3
  FND_b|FND_c|FND_f|FND_g,   // 4
  FND_a|FND_c|FND_d|FND_f|FND_g,  // 5
  FND_a|FND_c|FND_d|FND_e|FND_f|FND_g,  // 6
  FND_a|FND_b|FND_c,      // 7
  FND_a|FND_b|FND_c|FND_d|FND_e|FND_f|FND_g,   // 8
  FND_a|FND_b|FND_c|FND_d|FND_f|FND_g   // 9
};

uint16_t FND[4];    // FND에 쓰기 위한 값을 준비하는 변수


//감소하는 fnd4digit
void fnd4digit_sec_min_clock()
{

	static unsigned int msec=0;    // ms counter
	static int i=0;     // FND position indicator



	if (TIM10_fnd1ms_counter >= 2)   // 2ms reached
	{
		TIM10_fnd1ms_counter=0;
		msec += 2;   // 2ms
		if(run_flag)
		{
			if (msec > 400)   // 1000ms=1sec reached
			{
				msec = 0;
				if(last_time>0)
				{
					last_time--;       // sec count를 감소
				}
				FND_update(last_time);
				CCR_REVERSE_flag=(CCR_REVERSE_flag+1)%2;
			}
		}


		FND4digit_off();
#if 0 // common 애노우드  WCN4-
			HAL_GPIO_WritePin(FND_COM_PORT,FND_digit[i], GPIO_PIN_SET);
			HAL_GPIO_WritePin(FND_DATA_PORT, FND[i], GPIO_PIN_RESET);
#else // common 캐소우드 CL5642AH30
			HAL_GPIO_WritePin(FND_COM_PORT,FND_digit[i], GPIO_PIN_RESET);
			HAL_GPIO_WritePin(FND_DATA_PORT, FND[i], GPIO_PIN_SET);
#endif
			i++;   // 다음 display할 FND를 가리킨다.
			i %= 4;
	}

}



void fnd4digit_main(void)
{
	unsigned int value=0;   // 1초가 되었을때 up count
	unsigned int msec=0;    // ms counter
	static int i=0;     // FND position indicator

	FND4digit_off();

	while(1)
	{
#if 1   // SYSTICK interrupt 방식
		if (TIM10_fnd1ms_counter >= 2)   // 2ms reached
		{
			TIM10_fnd1ms_counter=0;
			msec += 2;   // 2ms
			if (msec > 1000)   // 1000ms=1sec reached
			{
				msec = 0;
				value++;       // sec count를 증가
				FND_update(value);
				CCR_REVERSE_flag=(CCR_REVERSE_flag+1)%2;
			}

			FND4digit_off();
#if 0 // common 애노우드  WCN4-
			HAL_GPIO_WritePin(FND_COM_PORT,FND_digit[i], GPIO_PIN_SET);
			HAL_GPIO_WritePin(FND_DATA_PORT, FND[i], GPIO_PIN_RESET);
#else // common 캐소우드 CL5642AH30
			HAL_GPIO_WritePin(FND_COM_PORT,FND_digit[i], GPIO_PIN_RESET);
			HAL_GPIO_WritePin(FND_DATA_PORT, FND[i], GPIO_PIN_SET);
#endif
			i++;   // 다음 display할 FND를 가리킨다.
			i %= 4;
		}
#else   // orginal delay 방식
		msec += 8;   // 8ms
		if (msec > 1000)   // 1000ms reached
		{
			msec = 0;
			value++;       // sec count를 증가
			FND_update(value);
		}
		for (int i=0; i < 4; i++)
		{
			FND4digit_off();
			HAL_GPIO_WritePin(FND_COM_PORT,FND_digit[i], GPIO_PIN_RESET);
			HAL_GPIO_WritePin(FND_DATA_PORT, FND[i], GPIO_PIN_SET);
			HAL_Delay(2);
		}
#endif
	}
}


//2ms 주기로 FND 값 업데이트
void FND_update(unsigned int value)
{
	FND[0] = FND_font[value % 10];		  //d1값
	FND[1] = FND_font[value / 10 % 10];   //d2값
	FND[2] = FND_font[value / 100 % 10];  //d3값
	FND[3] = FND_font[value / 1000 % 10]; //d4값

	return;
}
//아예 off
void FND4digit_off(void)
{
#if 0 // common 에노우드 WCN4-
	HAL_GPIO_WritePin(FND_COM_PORT, GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(FND_DATA_PORT,FND_font[8]|FND_p, GPIO_PIN_SET);
#else // common 캐소우드 CL5642AH30
	HAL_GPIO_WritePin(FND_COM_PORT, GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11, GPIO_PIN_SET);
	HAL_GPIO_WritePin(FND_DATA_PORT,FND_font[8]|FND_p, GPIO_PIN_RESET);
#endif
	return;
}
//8888
void FND4digit_on(void)
{
#if 0  // common 에노우드
	HAL_GPIO_WritePin(FND_COM_PORT, GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11, GPIO_PIN_SET);
	HAL_GPIO_WritePin(FND_DATA_PORT,FND_font[8]|FND_p, GPIO_PIN_RESET);
#else  // CL5642AH30
	HAL_GPIO_WritePin(FND_COM_PORT, GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(FND_DATA_PORT,FND_font[8]|FND_p, GPIO_PIN_SET);
#endif
	return;
}













