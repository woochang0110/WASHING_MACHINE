#include "uart.h" //HAL,GPIO func
#include<string.h>
#include<stdlib.h>
#include <stdio.h>

///move from HAL_UART_RxCpltCallback of stm32f4xx_hal_uart.c to here
//UART로부터 1Byte가 수신되면 HW가 Call 해준다.
//(RX_interrupt가 발생하면 이곳으로 들어온다.)
//comport master에서 ledallon\n ledalloff\n ledtoggle\n 보낼거임
//l할때 한번 e할때 한번 d할때 한번 1바이트당 한번씩 인터럽트 들어옴.
//start bit 수신이 완료되면 그때 이 함수를 호출하는거

#define BUFFER_NUM 40
#define COMMAND_LENGTH 40

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void pc_command_processing(void);
char number[4];

extern uint8_t rx_data;//main.c에 있는 rx_data 선언
extern uint8_t bt_rx_data;//main.c에 있는 rx_data 선언

extern UART_HandleTypeDef huart3; // main.c 51라인 UART3(uart)
extern UART_HandleTypeDef huart6; // UART6(BT)
extern volatile int dht11time;

extern volatile int dht_humid_func_flag;
extern volatile int dht_temperature_flag;
extern volatile int ultrasonic_func_flag;



//func from led.c
extern void led_main(void);
extern void led_all_on(void);
extern void led_all_off(void);
extern void led_keepon_up(void);
extern void led_keepon_down(void);
extern void led_on_up(void);
extern void led_on_down(void);
extern void flower_on(void);
extern void flower_off(void);
extern void button0_toggle(void);

//func from internal_rtc
extern void set_rtc_time(char *date_time);

void pc_command_processing(void);
void bt_command_processing(void);




extern volatile RTC_TimeTypeDef oldTime;


//interrupt 루틴이므로 optimize 방지를 위해 volatile 선언해줘야됨.
//pc UART variable
volatile unsigned char rx_buff[BUFFER_NUM][COMMAND_LENGTH]; //new라인을 만날때 까지 UART3으로부터 수신된 char를 저장하는 공간
volatile int rx_index=0;	//rx버퍼의 위치 index
volatile int rear=0;//현재 들어온 입력 위치
//volatile int newline_detect_flag=0;	//newline 만났는지 체크하는 flag
volatile int front=0;//LED에 출력된 출력 위치
//BT UART variable
volatile unsigned char bt_rx_buff[BUFFER_NUM][COMMAND_LENGTH]; //new라인을 만날때 까지 UART3으로부터 수신된 char를 저장하는 공간
volatile int bt_rx_index=0;	//rx버퍼의 위치 index
volatile int bt_rear=0;
//volatile int bt_newline_detect_flag=0;	//newline 만났는지 체크하는 flag
volatile int bt_front=0;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart==&huart3)//comport master와 연결된 huart3번인지 체크(UART)
	{

		if(((rear+1)%BUFFER_NUM)!=front)
		{
			if(rx_index<COMMAND_LENGTH)//rx_buff가 꽉차지 않았다면 즉 현재까지 들어온 데이터가 40바이트 아래라면
			{
				if(rx_data == '\n' || rx_data == '\r')//Unix(\r)와 Linux(\n)의 차이
				{
					rx_buff[rear][rx_index]=0;//'\0'를 넣어줌
					rx_index=0;	//다음 message 저장을 위해 rx_index값을 0으로 초기화
					rear++;
					rear %= BUFFER_NUM;
				}
				else
				{
					rx_buff[rear][rx_index++]=rx_data;//rx_buff의 rx_index번방에 rx_data 1바이트 넣어줌
				}
			}
			else
			{
				rx_index=0;
				printf("Message Overflow!!!\n");
			}
		}
		else
		{
			printf("Buffer Overflow!!!\n");
		}

		//주의: 반드시 HAL_UART_Receive_IT를 call 해줘야 다음 INT가 발생이 된다.
		HAL_UART_Receive_IT(&huart3, &rx_data, 1);//다음 INT 받을 준비.
	}

	if(huart==&huart6)//comport master와 연결된 huart6번인지 체크(BT)
		{

		if(((bt_rear+1)%BUFFER_NUM)!=bt_front)
		{
			if(bt_rx_index<COMMAND_LENGTH)//rx_buff가 꽉차지 않았다면 즉 현재까지 들어온 데이터가 40바이트 아래라면
			{
				if(bt_rx_data == '\n' || bt_rx_data == '\r')//Unix(\r)와 Linux(\n)의 차이
				{
					bt_rx_buff[bt_rear][bt_rx_index]=0;//'\0'를 넣어줌
					bt_rx_index=0;	//다음 message 저장을 위해 rx_index값을 0으로 초기화
					bt_rear++;
					bt_rear %= BUFFER_NUM;
				}
				else
				{
					bt_rx_buff[bt_rear][rx_index++]=bt_rx_data;//rx_buff의 rx_index번방에 rx_data 1바이트 넣어줌
				}
			}
			else
			{
				bt_rx_index=0;
				printf("Message Overflow!!!\n");
			}
		}
		else
		{
			printf("Buffer Overflow!!!\n");
		}
			//주의: 반드시 HAL_UART_Receive_IT를 call 해줘야 다음 INT가 발생이 된다.
			HAL_UART_Receive_IT(&huart6, &bt_rx_data, 1);//다음 INT 받을 준비.
		}

}




void pc_command_processing(void)
{

	if(front!=rear)
	{

		printf("%s\n",rx_buff[front]);
		if(!strncmp(rx_buff[front],"setrtc",strlen("setrtc")))
		{
			set_rtc_time(rx_buff[front]);
		}
		/*
		if(!strncmp(rx_buff[front],"dht_humid_func_on",strlen("dht_humid_func_on")))
		{
			dht_humid_func_flag=1;
		}
		if(!strncmp(rx_buff[front],"dht_humid_func_off",strlen("dht_humid_func_off")))
		{
			dht_humid_func_flag=0;
		}
		if(!strncmp(rx_buff[front],"dht_temperature_on",strlen("dht_temperature_on")))
		{
			dht_temperature_flag=1;
		}
		if(!strncmp(rx_buff[front],"dht_temperature_off",strlen("dht_temperature_off")))
		{
			dht_temperature_flag=0;
		}
		if(!strncmp(rx_buff[front],"ultrasonic_on",strlen("ultrasonic_on")))
		{
			ultrasonic_func_flag=1;
		}
		if(!strncmp(rx_buff[front],"ultrasonic_off",strlen("ultrasonic_off")))
		{
			ultrasonic_func_flag=0;
		}
		 */
		front++;
		front%=BUFFER_NUM;
		return;

	}


#if 0
		if(!strncmp(rx_buff[front],"dht11time",strlen("dht11time")))
		{
			dht11time=atoi(rx_buff[front]+9);//9번째부터 숫자니까 거기부터 \0까지 int로 변환
			/*
			number[0]=rx_buff[front][9];
			number[1]=rx_buff[front][10];
			number[2]=rx_buff[front][11];
			number[3]=0;
			dht11time=atoi(number);
			*/

		}



		printf("%s\n",rx_buff[front]);
		if(!strncmp(rx_buff[front],"dht11time150",strlen("dht11time150")))
		{
			dht11time=150;
		}
		if(!strncmp(rx_buff[front],"dht11time200",strlen("dht11time200")))
		{
			dht11time=200;
		}
		if(!strncmp(rx_buff[front],"dht11time300",strlen("dht11time300")))
		{
			dht11time=300;
		}
#endif



	/*
	if(front!=rear)
	{
		printf("%s\n",rx_buff[front]);
		if(!strncmp(rx_buff[front],"led_all_on",strlen("led_all_on")))
		{
			led_all_on();
		}
		if(!strncmp(rx_buff[front],"led_all_off",strlen("led_all_off")))
		{
			led_all_off();
		}
		if(!strncmp(rx_buff[front],"led_on_up",strlen("led_on_up")))
		{
			led_on_up();
		}
		if(!strncmp(rx_buff[front],"led_on_down",strlen("led_on_down")))
		{
			led_on_down();
		}
		if(!strncmp(rx_buff[front],"flower_on",strlen("flower_on")))
		{
			flower_on();
		}
		if(!strncmp(rx_buff[front],"flower_off",strlen("flower_off")))
		{
			flower_off();
		}
		if(!strncmp(rx_buff[front],"led_keepon_up",strlen("led_keepon_up")))
		{
			led_keepon_up();
		}
		if(!strncmp(rx_buff[front],"led_keepon_down",strlen("led_keepon_down")))
		{
			led_keepon_down();
		}
		front++;
		front%=BUFFER_NUM;
		return;

	}
	*/
}

void bt_command_processing(void)
{
	if(bt_front!=bt_rear)
	{
		printf("%s\n",bt_rx_buff[front]);
		if(!strncmp(bt_rx_buff[front],"dht11time150",strlen("dht11time150")))
		{
			dht11time=150;
		}
		if(!strncmp(bt_rx_buff[front],"dht11time200",strlen("dht11time200")))
		{
			dht11time=200;
		}
		if(!strncmp(bt_rx_buff[front],"dht11time300",strlen("dht11time300")))
		{
			dht11time=300;
		}

		bt_front++;
		bt_front%=BUFFER_NUM;
		return;

	}

	/*
	if(bt_front!=bt_rear)
	{
		printf("%s\n",bt_rx_buff[bt_front]);
		if(!strncmp(bt_rx_buff[bt_front],"led_all_on",strlen("led_all_on")))
		{
			led_all_on();
		}
		if(!strncmp(bt_rx_buff[bt_front],"led_all_off",strlen("led_all_off")))
		{
			led_all_off();
		}
		if(!strncmp(bt_rx_buff[bt_front],"led_on_up",strlen("led_on_up")))
		{
			led_on_up();
		}
		if(!strncmp(bt_rx_buff[bt_front],"led_on_down",strlen("led_on_down")))
		{
			led_on_down();
		}
		if(!strncmp(bt_rx_buff[bt_front],"flower_on",strlen("flower_on")))
		{
			flower_on();
		}
		if(!strncmp(bt_rx_buff[bt_front],"flower_off",strlen("flower_off")))
		{
			flower_off();
		}
		if(!strncmp(bt_rx_buff[bt_front],"led_keepon_up",strlen("led_keepon_up")))
		{
			led_keepon_up();
		}
		if(!strncmp(bt_rx_buff[bt_front],"led_keepon_down",strlen("led_keepon_down")))
		{
			led_keepon_down();
		}
		bt_front++;
		bt_front%=BUFFER_NUM;
		return;
	}
	*/

}
