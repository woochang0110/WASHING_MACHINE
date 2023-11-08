#include "main.h"
#include "DHT11.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


extern void delay_us (unsigned long us);//func from main.c
extern void move_cursor(uint8_t row, uint8_t column);
extern void lcd_string(uint8_t *str);

extern volatile int TDHT11_timer_counter;
extern volatile int TIM10_10ms_counter;
extern volatile int dht11time;

extern volatile int dht_humid_func_flag;
extern volatile int dht_temperature_flag;

extern uint8_t lcd_display_mode_flag;//0: RTC display   1: Humid&Temp  2: UltraSonic Distance

void DHT11_main(void);
void DHT11_Init(void);
void DHT11_processing(void);





void DHT11_main(void)
{
	uint8_t i_RH, d_RH, i_Tmp, d_Tmp;



	// HAL_TIM_Base_Start_IT(&htim2);
	DHT11_Init();//현재 상태가 high 인지 low인지 모르므로 high상태로 만들어주고 3초 대기

	while(1)
	{
		//핸드쉐이킹 시작
		DHT11_trriger();//LOW(20ms)&HIGH(7ms)
		DHT11_DataLine_Input();//PA0 입력 모드로 변환
		DHT11_dumi_read();//1쭉받고 0쭉받고 1쭉받고나서 1들어오면 시작임.
		//핸드쉐이킹 끝

		//습도 정보 입력
		i_RH = DHT11_rx_Data();
		d_RH = DHT11_rx_Data();
		//온도 정보 입력
		i_Tmp = DHT11_rx_Data();
		d_Tmp = DHT11_rx_Data();
		//입력 받은 데이터 출력
		DHT11_DataLine_Output();
		HAL_GPIO_WritePin(DHT11_PORT, DHT11_DATA_RIN, GPIO_PIN_SET);
		printf("[Tmp]%d\n",(int)i_Tmp);
		printf("[Wet]%d\n",(int)i_RH);

		// FND_Update(i_Tmp*100 + i_RH);
		HAL_Delay(1500);//loop after 1500ms Wait for Read until 1500ms
	}

}


void DHT11_processing(void)
{
	uint8_t i_RH,d_RH,i_Tmp,d_Tmp;
	char lcd2_buff[20];

	if(TIM10_10ms_counter>=dht11time)//1500ms delay
	{
		TIM10_10ms_counter=0;
	//핸드쉐이킹 시작
		DHT11_trriger();
		DHT11_DataLine_Input();
		DHT11_dumi_read();
	//핸드쉐이킹 끝
		i_RH = DHT11_rx_Data();//습도 정보 입력
		d_RH = DHT11_rx_Data();//습도 정보 입력 끝

		i_Tmp = DHT11_rx_Data();	//온도 정보 입력
		d_Tmp = DHT11_rx_Data();	//온도 정보 입력 끝

	//입력 받은 데이터 출력
		DHT11_DataLine_Output();
		HAL_GPIO_WritePin(DHT11_PORT, DHT11_DATA_RIN, GPIO_PIN_SET);
		/*
		if(dht_temperature_flag)
		{
			printf("[Tmp]%d\n",(int)i_Tmp);
		}
		if(dht_humid_func_flag)
		{
			printf("[Wet]%d\n",(int)i_RH);
		}
		*/
		if(lcd_display_mode_flag==1)
		{
			sprintf(lcd2_buff,"[Tmp]%d [Wet]%d",(int)i_Tmp,(int)i_RH);
			move_cursor(0,0);
			lcd_string(lcd2_buff);
		}

		//move_cursor(0,0);
		//lcd_string(lcd_buff);
	}
}

void DHT11_Init(void)//현재 상태가 high 인지 low인지 모르므로 high상태로 만들어줌.
{

	printf("DHT11_init\n");
	HAL_GPIO_WritePin(DHT11_PORT, DHT11_DATA_RIN, GPIO_PIN_SET);//GPIO_PIN_SET=1
	HAL_Delay(3000);
	return;
}


void DHT11_trriger(void)
{
	HAL_GPIO_WritePin(DHT11_PORT, DHT11_DATA_RIN, GPIO_PIN_RESET);
	HAL_Delay(20);

	HAL_GPIO_WritePin(DHT11_PORT, DHT11_DATA_RIN, GPIO_PIN_SET);
	delay_us(7);
	return;
}


void DHT11_DataLine_Input(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	/*Configure GPIO pin : PA0 */
  GPIO_InitStruct.Pin = DHT11_DATA_RIN;	//PA0
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;//Change Output to Input
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(DHT11_PORT, &GPIO_InitStruct);

	return;
}


void DHT11_DataLine_Output(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	/*Configure GPIO pin : PA0 */
  GPIO_InitStruct.Pin = DHT11_DATA_RIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;			//Change Input to Output
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(DHT11_PORT, &GPIO_InitStruct);

	return;
}


uint8_t DHT11_rx_Data(void)
{
	uint8_t rx_data = 0;

	for(int i = 0; i < 8; i++)
	{//for 시작
		//PA0의 다음 입력이 1이 들어올때까지 대기(여기안에 2개의 while문의 대기시간의 합은 데이터시트 상 50us)
		while( 0 == HAL_GPIO_ReadPin(DHT11_PORT, DHT11_DATA_RIN) );
#if 1
		delay_us(40);//0과 1의 시간차이 만큼 기다려줌(1은 70us 0은 28us정도)
#else  // org
		delay_us(16);
#endif
		rx_data<<=1;//좌측으로 1비트씩 시프트

		//40us가 지났는데도 여전히 PA0의 input이 1을 유지하고 있다면
		if(HAL_GPIO_ReadPin(DHT11_PORT, DHT11_DATA_RIN))
		{
			rx_data |= 1;//최하위 비트 1
		}
		//아니라면 어차피 0으로 초기화해서 상관없고
		//PA0의 다음 입력이 0이 들어올 때까지 대기
		while( 1 == HAL_GPIO_ReadPin(DHT11_PORT, DHT11_DATA_RIN) );
	}//for 종료
	return rx_data;//8비트 다 채우고 리턴
}


void DHT11_dumi_read(void)
{
	while( 1 == HAL_GPIO_ReadPin(DHT11_PORT, DHT11_DATA_RIN) );
	while( 0 == HAL_GPIO_ReadPin(DHT11_PORT, DHT11_DATA_RIN) );
	while( 1 == HAL_GPIO_ReadPin(DHT11_PORT, DHT11_DATA_RIN) );
	return;
}
