#include "main.h"
#include"button.h"
#include "i2c_lcd.h"
#include "fnd4digit.h"


#define LEDBAR 9

//======함수
extern void move_cursor(uint8_t row, uint8_t column);
extern void lcd_string(uint8_t *str);
extern void lcd_command(uint8_t command);

extern void FND_update(unsigned int value);
extern void FND4digit_off(void);
extern void FND4digit_on(void);
extern void fnd4digit_sec_min_clock();
//extern void ultrasonic_processing(void);
//extern void make_trigger(void);


void washing_machine_processing(void);
void washing_machine_sensing(void);
void washing_machine_running(void);
void wm_ledbar_display(void);
//void wm_lcd_display(void);


//======변수
//main.c
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim4;
extern volatile int t1ms_counter;	//main.c systick timer int counter
extern volatile int TIM10_fnd1ms_counter;  // fnd1ms
extern uint16_t FND_digit[4];
extern uint32_t FND_font[10];
uint8_t led_bar[LEDBAR]={0x01,0x03,0x07,0x0f,0x1f,0x3f,0x7f,0xff};
extern TIM_HandleTypeDef htim5;
extern volatile int CCR_REVERSE_flag;
extern volatile int dist;
extern volatile int TIM10_20ms_servomotor;
	//------Flag
volatile int button_flag=7;//0~6까지 어떤 버튼이 눌렸는지
volatile int power_on_flag=0;//0은 off 1은 on
volatile int machine_state_flag=0; // 0은 메뉴선택, 1은 동작 2는 종료
volatile int menu_flag=0;//0은 물높이 1은 세탁 2는 헹굼 3은 탈수
volatile int auto_flag=0;//0은 manual 1은 auto
volatile int run_flag=0; //0은 일시정지, 1은 run
volatile unsigned int last_time=90;	//남은 동작시간
volatile unsigned int pause_time=90;//일시정지 시점에서 남은 동작시간
volatile int auto_menu_select=0;	//auto mode menu선택 0은 표준모드 1은 소량 2는 강력 3은 이불

	//------Data





volatile uint8_t water_level=4;//0~8 9개값 1씩 증가
volatile int washing=15;//6~30 3씩 증가
volatile int washing_time=2;//1~5회 1씩증가
volatile int dehydration=5;//1~9분 1씩증가








//여기부터 시작하세여
//작동부
void washing_machine_running(void)
{

	if(power_on_flag==0)//전원 off 상태라면
	{
		//lcd_command(DISPLAY_OFF);
		//lcd_command(CLEAR_DISPLAY);
		//FND4digit_off();
		//led all off
		HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, 0);
		HAL_GPIO_WritePin(LED_YELLOW_GPIO_Port, LED_YELLOW_Pin, 0);
		HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, 0);

		//led bar off
		wm_ledbar_display();//power_on_flag==0이면 다 off


	}
	else//전원 on 상태라면
	{//전원 on 상태 시작
		wm_ledbar_display();
		//wm_lcd_display();
		fnd4digit_sec_min_clock();
		//ultrasonic_processing();
		/*
		if(door_close_flag)//0: 열린상태, 1:닫힌상태
		{
			if(dist<10)//초음파 거리가 10미만이면
			{//led on
				HAL_GPIO_WritePin(DOOR_LED_GPIO_Port, DOOR_LED_Pin, 1);
			}
		}*/

		if(machine_state_flag==1)//세탁기 동작 상태
		{
			//메뉴에 따라서 lcd 출력
			if(!run_flag)//일시정지 상태라면
			{
				last_time=pause_time;
				HAL_GPIO_TogglePin(LED_YELLOW_GPIO_Port, LED_YELLOW_Pin);//yellow led 토글
				//dc motor 멈춤
				HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_1);//모터를 멈추는 거임.
//				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 1);//모터드라이버 IN1 =0
//				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 0);//모터드라이버 IN =1
			}
			else
			{
				/*
				if(dist<5)//초음파 거리가 10미만이면
				{//led on
					//HAL_GPIO_WritePin(DOOR_LED_GPIO_Port, DOOR_LED_Pin, 1);
					HAL_GPIO_WritePin(GPIOF, GPIO_PIN_3, 1);
					//printf("dist<5\n");
				}
		*/
				//dc motor 동작 +pwm제어
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
		}//세탁기 동작상태 끝


	}//전원 on 상태 끝


}


//센싱부
void washing_machine_sensing(void)
{
	//==버튼 센싱부
	//lcd 표현할 lcd_buff
	volatile char lcd0_buff[20]={0},lcd1_buff[20]={0};
	volatile char lcd2_buff[20]={0},clear_buff[20]={0};
	if(!power_on_flag)//전원 off 상태일때
	{
		lcd_command(CLEAR_DISPLAY);
		//fnd off
		//HAL_GPIO_WritePin(FND_COM_PORT, GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11, GPIO_PIN_SET);//D1~D4 1로 해줌으로써 아무값도 표시안됨
		//HAL_GPIO_WritePin(FND_DATA_PORT,FND_font[8]|FND_p, GPIO_PIN_RESET);	//segment 값들도 0으로 세팅해서 아무거도 안나오게
		//전원 off 상태에서 BTN6 눌렸다면
		if(get_button(BUTTON_6_GPIO_Port,BUTTON_6_Pin,6)==BUTTON_PRESS)//전원 버튼 PF2 BTN6
		{
			power_on_flag=1;
			HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, 1);//led green on
			HAL_TIM_PWM_Start(&htim5,TIM_CHANNEL_4);//buzzer pwm start
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, 1);//buzzer on
			//make_trigger();
			i2c_lcd_init();
			FND4digit_off();
			machine_state_flag=0;
			menu_flag=0;
			auto_flag=0;
			run_flag=0;
			last_time=0;
			auto_menu_select=0;
			water_level=4;
			washing=15;
			washing_time=2;
			dehydration=5;
			HAL_TIM_PWM_Stop(&htim5,TIM_CHANNEL_4);	//buzzer off
		}
	}//전원 off 상태 끝
	else//전원 on 상태일때
	{
		fnd4digit_sec_min_clock();
		//

		if(!auto_flag)
		{//수동모드 센싱
			move_cursor(0,0);
			lcd_string(" Manual Mode");
			if(machine_state_flag==0)//세탁기 메뉴선택 상태
			{
				if(get_button(BUTTON_0_GPIO_Port,BUTTON_0_Pin,0)==BUTTON_PRESS)//수위 버튼 PE15 BTN0
				{
					lcd_command(CLEAR_DISPLAY);
					if(water_level>7)//끝까지 돌았으면
					{
						HAL_GPIO_WritePin(GPIOD, 0xff, 0);//led bar 초기화해주고
						water_level=1;//최저값
					}
					else
					{
						water_level++;
					}
					strcpy(lcd1_buff,clear_buff);
					strcpy(lcd1_buff,"Water Level:");
					sprintf(lcd2_buff,"%d0%",water_level);
					move_cursor(1,0);
					lcd_string(lcd1_buff);
					move_cursor(1,12);
					lcd_string(lcd2_buff);

					menu_flag=0;//물높이		  //0은 물높이 1은 세탁 2는 헹굼 3은 탈수
				}
				if(get_button(BUTTON_1_GPIO_Port,BUTTON_1_Pin,1)==BUTTON_PRESS)//세탁 버튼 PE14 BTN1
				{
					lcd_command(CLEAR_DISPLAY);
					washing+=3;
					if(washing>30)//끝까지 돌았으면
					{
						washing=6;//최저값
					}
					strcpy(lcd1_buff,clear_buff);
					strcpy(lcd1_buff,"Washing: ");
					sprintf(lcd2_buff,"%d",washing);
					move_cursor(1,0);
					lcd_string(lcd1_buff);
					move_cursor(1,9);
					lcd_string(lcd2_buff);

					menu_flag=1;//세탁		  //0은 물높이 1은 세탁 2는 헹굼 3은 탈수
				}
				if(get_button(BUTTON_2_GPIO_Port,BUTTON_2_Pin,2)==BUTTON_PRESS)//헹굼 버튼 PE12 BTN2
				{
					lcd_command(CLEAR_DISPLAY);
					washing_time++;
					if(washing_time>5)//끝까지 돌았으면
					{
						washing_time=1;//최저값
					}
					strcpy(lcd1_buff,clear_buff);
					strcpy(lcd1_buff,"Washing Time: ");
					sprintf(lcd2_buff,"%d",washing_time);
					move_cursor(1,0);
					lcd_string(lcd1_buff);
					move_cursor(1,14);
					lcd_string(lcd2_buff);
					menu_flag=2;//헹굼		  //0은 물높이 1은 세탁 2는 헹굼 3은 탈수
				}
				if(get_button(BUTTON_3_GPIO_Port,BUTTON_3_Pin,3)==BUTTON_PRESS)//탈수 버튼 PE10 BTN3
				{
					lcd_command(CLEAR_DISPLAY);
					dehydration++;
					if(dehydration>9)//끝까지 돌았으면
					{
						dehydration=1;//최저값
					}
					strcpy(lcd1_buff,clear_buff);
					strcpy(lcd1_buff,"dehydration: ");
					sprintf(lcd2_buff,"%d",dehydration);
					move_cursor(1,0);
					lcd_string(lcd1_buff);
					move_cursor(1,13);
					lcd_string(lcd2_buff);

					menu_flag=3;//탈수		  //0은 물높이 1은 세탁 2는 헹굼 3은 탈수
				}
				if(get_button(BUTTON_4_GPIO_Port,BUTTON_4_Pin,4)==BUTTON_PRESS)//수동자동 버튼 PF0 BTN4
				{

					menu_flag=0;//탈수		  //0은 물높이 1은 세탁 2는 헹굼 3은 탈수
					run_flag=0; //0은 일시정지, 1은 run
					auto_flag=1;//0은 manual 1은 auto
					lcd_command(CLEAR_DISPLAY);
				}
				if(get_button(BUTTON_5_GPIO_Port,BUTTON_5_Pin,5)==BUTTON_PRESS)//동작/일시정지 버튼 PF1 BTN5
				{
					HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);//4번타이머의 PWM CH1번에 연결된 모터를 돌리는 거임.


					HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, 0);
					HAL_GPIO_WritePin(LED_YELLOW_GPIO_Port, LED_YELLOW_Pin, 1);
					last_time=water_level+washing*washing_time+dehydration;
					machine_state_flag=1;//메뉴선택	   // 0은 메뉴선택, 1은 동작 2는 종료
					run_flag=1; //0은 일시정지, 1은 run
					lcd_command(CLEAR_DISPLAY);
				}
				if(get_button(BUTTON_6_GPIO_Port,BUTTON_6_Pin,6)==BUTTON_PRESS)//전원 버튼 PF2 BTN6
				{
					power_on_flag=0;//power off
					menu_flag=0;//물높이		  //0은 물높이 1은 세탁 2는 헹굼 3은 탈수
					run_flag=0; //0은 일시정지, 1은 run
					auto_flag=0;//0은 manual 1은 auto
					lcd_command(CLEAR_DISPLAY);

				}
				/*
				if(get_button(GPIOF,DOOR_BUTTON_Pin,7)==BUTTON_PRESS)//수위 버튼 PE15 BTN0
				{
					door_close_flag=(door_close_flag+1)%2;// 0: 열린상태, 1:닫힌상태
					if(!door_close_flag)//문이 열려있음
					{
							TIM10_20ms_servomotor=0;
							__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,100);//닫아주고

						HAL_GPIO_WritePin(DOOR_LED_GPIO_Port, DOOR_LED_Pin, 0);
					}
					else
					{

							TIM10_20ms_servomotor=0;
							__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,20);//열어주고


					}
				}
				 	*/
			}
			if(machine_state_flag==1)//세탁기 동작 상태
			{
				if(get_button(BUTTON_5_GPIO_Port,BUTTON_5_Pin,5)==BUTTON_PRESS)//동작/일시정지 버튼 PF1 BTN5
				{
					run_flag=(run_flag+1)%2; //누를때마다 Toggle  0은 일시정지, 1은 run
					if(run_flag)//일시정지 상태에서->작동 상태로 바뀌었다면 다시 yellow led on
					{
						HAL_GPIO_WritePin(LED_YELLOW_GPIO_Port, LED_YELLOW_Pin, 1);
						HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);//모터를 멈추는 거임.

						move_cursor(1,0);
						lcd_string("    Running!!");
					}
					if(!run_flag)
					{
						HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_1);//모터를 멈추는 거임.
						pause_time=last_time;//버튼 눌린 순간의 남은 시간을 pause time에 넣어줌
						last_time=pause_time;
						//HAL_GPIO_WritePin(DOOR_LED_GPIO_Port, DOOR_LED_Pin, 0);
						move_cursor(1,0);
						lcd_string("    Pause!!");
					}
					lcd_command(CLEAR_DISPLAY);
					HAL_GPIO_WritePin(GPIOD, 0xff, 0);
				}
				if(get_button(BUTTON_6_GPIO_Port,BUTTON_6_Pin,6)==BUTTON_PRESS)//전원 버튼 PF2 BTN6
				{
					power_on_flag=0;//power off
					machine_state_flag=0;//메뉴선택	   // 0은 메뉴선택, 1은 동작 2는 종료
					menu_flag=0;//물높이		  //0은 물높이 1은 세탁 2는 헹굼 3은 탈수
					run_flag=0; //0은 일시정지, 1은 run
					auto_flag=0;//0은 manual 1은 auto
					lcd_command(CLEAR_DISPLAY);
				}
				if(last_time==0)//동작 중 시간이 끝나면
				{
					HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_1);//모터를 멈추는 거임.
					HAL_GPIO_WritePin(LED_YELLOW_GPIO_Port, LED_YELLOW_Pin, 0);//yellow led off
					HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, 1);// red led on
FND4digit_off();
					machine_state_flag=2;//세탁기 종료 상태
				}
			}
			if(machine_state_flag==2)//세탁기 종료 상태
			{
				machine_state_flag=0;
				HAL_TIM_PWM_Start(&htim5,TIM_CHANNEL_4);//buzzer pwm start
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, 1);//buzzer on
				move_cursor(1,0);
				lcd_string("   finish!!!!");
				HAL_Delay(5000);//5초 딜레이 종료되었으니 HAL_Delay 사용
				HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, 0);//빨간 led 불끄고
				power_on_flag=0;
				HAL_TIM_PWM_Stop(&htim5,TIM_CHANNEL_4);	//buzzer off

			}

		}//수동모드 센싱 끝

		else
		{//자동 모드 센싱 시작
			if(machine_state_flag==0)//세탁기 메뉴선택 상태
			{
				//lcd 물높이 선택창 출력
				//led green on
				if(get_button(BUTTON_0_GPIO_Port,BUTTON_0_Pin,0)==BUTTON_PRESS)//표준모드 버튼 PE15 BTN0
				{

					water_level=6;
					washing=15;
					washing_time=2;
					dehydration=5;
					auto_menu_select=0;		//auto mode menu선택 0은 표준모드 1은 소량 2는 강력 3은 이불
					lcd_command(CLEAR_DISPLAY);
					move_cursor(0,0);
					lcd_string("  Auto Mode");
					move_cursor(1,0);
					lcd_string("   Standard");
					HAL_GPIO_WritePin(GPIOD, 0xff, 0);
				}
				if(get_button(BUTTON_1_GPIO_Port,BUTTON_1_Pin,1)==BUTTON_PRESS)//소량모드 버튼 PE14 BTN1
				{
					water_level=3;
					washing=10;
					washing_time=1;
					dehydration=3;
					auto_menu_select=1;		//auto mode menu선택 0은 표준모드 1은 소량 2는 강력 3은 이불
					lcd_command(CLEAR_DISPLAY);
					move_cursor(0,0);
					lcd_string("  Auto Mode");
					move_cursor(1,0);
					lcd_string("Small Quantity");
					HAL_GPIO_WritePin(GPIOD, 0xff, 0);
				}
				if(get_button(BUTTON_2_GPIO_Port,BUTTON_2_Pin,2)==BUTTON_PRESS)//강력모드 버튼 PE12 BTN2
				{
					water_level=8;
					washing=30;
					washing_time=3;
					dehydration=9;
					auto_menu_select=2;		//auto mode menu선택 0은 표준모드 1은 소량 2는 강력 3은 이불
					lcd_command(CLEAR_DISPLAY);
					move_cursor(0,0);
					lcd_string("  Auto Mode");
					move_cursor(1,0);
					lcd_string(" Power Wash");
					HAL_GPIO_WritePin(GPIOD, 0xff, 0);
				}
				if(get_button(BUTTON_3_GPIO_Port,BUTTON_3_Pin,3)==BUTTON_PRESS)//이불모드 버튼 PE10 BTN3
				{
					last_time=77;//level:8  wash:20 time:3 dehyd:9
					washing=20;
					washing_time=3;
					dehydration=9;
					auto_menu_select=3;		//auto mode menu선택 0은 표준모드 1은 소량 2는 강력 3은 이불
					lcd_command(CLEAR_DISPLAY);
					move_cursor(0,0);
					lcd_string("  Auto Mode");
					move_cursor(1,0);
					lcd_string("   Blankets");
					HAL_GPIO_WritePin(GPIOD, 0xff, 0);
				}
				if(get_button(BUTTON_4_GPIO_Port,BUTTON_4_Pin,4)==BUTTON_PRESS)//수동자동 버튼 PF0 BTN4
				{

					auto_menu_select=0;		//auto mode menu선택 0은 표준모드 1은 소량 2는 강력 3은 이불
					run_flag=0; //0은 일시정지, 1은 run
					auto_flag=0;//0은 manual 1은 auto
					water_level=4;
					washing=15;
					washing_time=2;
					dehydration=5;
					lcd_command(CLEAR_DISPLAY);

					HAL_GPIO_WritePin(GPIOD, 0xff, 0);
				}
				if(get_button(BUTTON_5_GPIO_Port,BUTTON_5_Pin,5)==BUTTON_PRESS)//동작/일시정지 버튼 PF1 BTN5
				{//동작
					HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, 0);
					HAL_GPIO_WritePin(LED_YELLOW_GPIO_Port, LED_YELLOW_Pin, 1);
					last_time=water_level+washing*washing_time+dehydration;
					HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);//4번타이머의 PWM CH1번에 연결된 모터를 돌리는 거임.
					machine_state_flag=1;//메뉴선택	   // 0은 메뉴선택, 1은 동작 2는 종료
					run_flag=1; //0은 일시정지, 1은 run
					lcd_command(CLEAR_DISPLAY);
					HAL_GPIO_WritePin(GPIOD, 0xff, 0);
				}
				if(get_button(BUTTON_6_GPIO_Port,BUTTON_6_Pin,6)==BUTTON_PRESS)//전원 버튼 PF2 BTN6
				{
					power_on_flag=0;//power off
					auto_menu_select=0;	//auto mode menu선택 0은 표준모드 1은 소량 2는 강력 3은 이불
					run_flag=0; //0은 일시정지, 1은 run
					auto_flag=0;//0은 manual 1은 auto
				}
				/*버튼으로 door 열고
				if(get_button(DOOR_BUTTON_GPIO_Port,DOOR_BUTTON_Pin,7)==BUTTON_PRESS)//수위 버튼 PE15 BTN0
				{
					door_close_flag=(door_close_flag+1)%2;// 0: 열린상태, 1:닫힌상태
					if(!door_close_flag)//문이 열려있음
					{
							TIM10_20ms_servomotor=0;
							__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,100);//닫아주고


						HAL_GPIO_WritePin(DOOR_LED_GPIO_Port, DOOR_LED_Pin, 0);
					}
					else
					{
							TIM10_20ms_servomotor=0;
							__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,20);//열어주고


					}
				}
				*/
			}
			if(machine_state_flag==1)//세탁기 동작 상태
			{

				if(auto_menu_select==0)
				{
					move_cursor(0,0);
					lcd_string("   Standard");
				}
				else if(auto_menu_select==1)
				{
					move_cursor(0,0);
					lcd_string("Small Quantity");
				}
				else if(auto_menu_select==2)
				{
					move_cursor(0,0);
					lcd_string(" Power Wash");				}
				else if(auto_menu_select==3)
				{
					move_cursor(0,0);
					sprintf(lcd0_buff,"   Blankets");
				}

				if(get_button(BUTTON_5_GPIO_Port,BUTTON_5_Pin,5)==BUTTON_PRESS)//동작/일시정지 버튼 PF1 BTN5
				{//일시정지
					run_flag=(run_flag+1)%2; //0은 일시정지, 1은 run

					if(run_flag)//일시정지 상태에서->작동 상태로 바뀌었다면 다시 yellow led on
					{
						HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);//모터를 멈추는 거임.
						HAL_GPIO_WritePin(LED_YELLOW_GPIO_Port, LED_YELLOW_Pin, 1);
						move_cursor(1,0);
						lcd_string("    Running!!");
					}
					if(!run_flag)
					{
						HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_1);//모터를 멈추는 거임.
						pause_time=last_time;//버튼 눌린 순간의 남은 시간을 pause time에 넣어줌
						last_time=pause_time;
						move_cursor(1,0);
						lcd_string("    Pause!!");
					}

				}
				if(get_button(BUTTON_6_GPIO_Port,BUTTON_6_Pin,6)==BUTTON_PRESS)//전원 버튼 PF2 BTN6
				{
					power_on_flag=0;//power off
					machine_state_flag=0;//메뉴선택	   // 0은 메뉴선택, 1은 동작 2는 종료
					menu_flag=0;//물높이		  //0은 물높이 1은 세탁 2는 헹굼 3은 탈수
					run_flag=0; //0은 일시정지, 1은 run
					auto_flag=0;//0은 manual 1은 auto
				}
				if(last_time==0)//동작 중 시간이 끝나면
				{
					machine_state_flag=2;//세탁기 종료 상태

					HAL_GPIO_WritePin(LED_YELLOW_GPIO_Port, LED_YELLOW_Pin, 0);
					HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, 1);
FND4digit_off();

					HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_1);//모터를 멈추는 거임.
	//				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 1);//모터드라이버 IN1 =0
	//				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 0);//모터드라이버 IN =1
				}

				/*
				 if(last_time==0)//동작 중 시간이 끝나면
				{
					HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_1);//모터를 멈추는 거임.
					HAL_GPIO_WritePin(LED_YELLOW_GPIO_Port, LED_YELLOW_Pin, 0);//yellow led off
					HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, 1);// red led on
FND4digit_off();
					machine_state_flag=2;//세탁기 종료 상태
				}
				 */
			}
			if(machine_state_flag==2)//세탁기 종료 상태
			{
				machine_state_flag=0;
				HAL_TIM_PWM_Start(&htim5,TIM_CHANNEL_4);//buzzer pwm start
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, 1);//buzzer on
				move_cursor(1,0);
				lcd_string("   finish!!!!");
				HAL_Delay(5000);//2초 딜레이 종료되었으니 HAL_Delay 사용
				HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, 0);//빨간 led 불끄고
				power_on_flag=0;
				HAL_TIM_PWM_Stop(&htim5,TIM_CHANNEL_4);	//buzzer off
				FND4digit_off();
				/*
			if(machine_state_flag==2)//세탁기 종료 상태
			{
				machine_state_flag=0;
				HAL_TIM_PWM_Start(&htim5,TIM_CHANNEL_4);//buzzer pwm start
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, 1);//buzzer on
				move_cursor(1,0);
				lcd_string("   finish!!!!");
				HAL_Delay(5000);//5초 딜레이 종료되었으니 HAL_Delay 사용
				HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, 0);//빨간 led 불끄고
				power_on_flag=0;
				HAL_TIM_PWM_Stop(&htim5,TIM_CHANNEL_4);	//buzzer off



				 * */
			}
		}//자동모드 센싱끝

	}//전원 on 상태 끝

}



void washing_machine_processing(void)
{
	washing_machine_sensing();
	washing_machine_running();
}


void wm_ledbar_display(void)
{
//	static uint8_t temp=0x0f;// 이전 water level값 기억용 초기값은 초기 water_level값과 같음
	if(power_on_flag==1)
	{
		//systick Handler 1ms Timer
		if(t1ms_counter %20 == 0)//20번째 인터럽트라면 아래 로직을 동작시켜라. 인터럽트 핸들러는 1ms마다 동작하기 때문에 결국 200ms HAL_Delay와 같은 동작이 되나 HW는 계속 동작하게끔 하는거임.
		{
			HAL_GPIO_WritePin(GPIOD, led_bar[water_level-1], 1);// 이전 값이 현재 값보다 높았다면 그 부분만큼 led bar를 꺼주는 작업 temp&led_bar[water_level-1]
//			temp=led_bar[water_level-1];//이전 값을 8bit형태로 temp에 임시저장
		}

	}
	else if(power_on_flag==0)
	{
		HAL_GPIO_WritePin(GPIOD, 0xff, 0);
	}
}


#if 0
void wm_lcd_display(void)
{
	//char lcd0_buff[20]={0},lcd1_buff[20]={0},lcd2_buff[20]={0},clear_buff[20]={0};

	if(power_on_flag)//전원 off 상태라면
	{
		//==lcd 출력부
		if(!auto_flag)//수동 매뉴얼 모드
		{
			move_cursor(0,0);
			lcd_string(" Manual Mode");
			if(machine_state_flag==0)//세탁기 메뉴선택 상태
			{

				if(menu_flag==0)//물높이
				{
					strcpy(lcd1_buff,clear_buff);
					strcpy(lcd1_buff,"Water Level:");
					sprintf(lcd2_buff,"%d0%",water_level);
					move_cursor(1,0);
					lcd_string(lcd1_buff);
					move_cursor(1,12);
					lcd_string(lcd2_buff);
				}
				if(menu_flag==1)//세탁
				{

					strcpy(lcd1_buff,clear_buff);
					strcpy(lcd1_buff,"Washing: ");
					sprintf(lcd2_buff,"%d",washing);
					move_cursor(1,0);
					lcd_string(lcd1_buff);
					move_cursor(1,9);
					lcd_string(lcd2_buff);
					/*
					strcpy(lcd1_buff,clear_buff);
					sprintf(lcd1_buff,"Washing: %dsec",washing);
					move_cursor(1,0);
					lcd_string(lcd1_buff);
					*/
				}
				if(menu_flag==2)//헹굼
				{
					/*
					strcpy(lcd1_buff,clear_buff);
					sprintf(lcd1_buff,"Washing Time: %d",washing_time);
					move_cursor(1,0);
					lcd_string(lcd1_buff);
					*/
					strcpy(lcd1_buff,clear_buff);
					strcpy(lcd1_buff,"Washing Time: ");
					sprintf(lcd2_buff,"%d",washing_time);
					move_cursor(1,0);
					lcd_string(lcd1_buff);
					move_cursor(1,14);
					lcd_string(lcd2_buff);
				}
				if(menu_flag==3)//탈수
				{
					/*
					strcpy(lcd1_buff,clear_buff);
					sprintf(lcd1_buff,"dehydration:%dsec",dehydration);
					move_cursor(1,0);
					lcd_string(lcd1_buff);
					*/
					strcpy(lcd1_buff,clear_buff);
					strcpy(lcd1_buff,"dehydration: ");
					sprintf(lcd2_buff,"%d",dehydration);
					move_cursor(1,0);
					lcd_string(lcd1_buff);
					move_cursor(1,13);
					lcd_string(lcd2_buff);
				}
			}
			if(machine_state_flag==1)//세탁기 동작 상태
			{
				if(run_flag)//작동 상태라면
				{
					/*
					strcpy(lcd1_buff,clear_buff);
					sprintf(lcd1_buff,"  Running!!");
					move_cursor(1,0);
					lcd_string(lcd1_buff);
					*/
					move_cursor(1,0);
					lcd_string("    Running!!");
				}
				else
				{
					/*
					strcpy(lcd1_buff,clear_buff);
					sprintf(lcd1_buff,"    Pause!!");
					move_cursor(1,0);
					lcd_string(lcd1_buff);
					*/
					move_cursor(1,0);
					lcd_string("    Pause!!");
				}
			}


		}//수동모드 끝
		else//자동 모드
		{

			if(machine_state_flag==0)//세탁기 메뉴선택 상태
			{
				move_cursor(0,0);
				lcd_string("  Auto Mode");
				if(auto_menu_select==0)//표준모드
				{
					/*
					strcpy(lcd1_buff,clear_buff);
					sprintf(lcd1_buff,"   Standard");
					move_cursor(1,0);
					lcd_string(lcd1_buff);
					 */
					move_cursor(1,0);
					lcd_string("   Standard");
				}
				if(auto_menu_select==1)//소량
				{
					/*
					strcpy(lcd1_buff,clear_buff);
					sprintf(lcd1_buff,"Small Quantity");
					move_cursor(1,0);
					lcd_string(lcd1_buff);
					*/
					move_cursor(1,0);
					lcd_string("Small Quantity");
				}
				if(auto_menu_select==2)//강력
				{
					/*
					strcpy(lcd1_buff,clear_buff);
					sprintf(lcd1_buff," Power Wash");
					move_cursor(1,0);
					lcd_string(lcd1_buff);
					*/
					move_cursor(1,0);
					lcd_string(" Power Wash");
				}
				if(auto_menu_select==3)//이불
				{
					/*
					strcpy(lcd1_buff,clear_buff);
					sprintf(lcd1_buff,"   Blankets");
					move_cursor(1,0);
					lcd_string(lcd1_buff);
					*/
					move_cursor(1,0);
					lcd_string("   Blankets");
				}
			}
			if(machine_state_flag==1)//세탁기 동작 상태
			{
				//0,0 출력
				if(auto_menu_select==0)//표준모드
				{
					/*
					strcpy(lcd0_buff,clear_buff);
					sprintf(lcd0_buff,"   Standard");
					move_cursor(0,0);
					lcd_string(lcd0_buff);
					*/
					move_cursor(0,0);
					lcd_string("   Standard");
				}
				if(auto_menu_select==1)//소량
				{
					/*
					strcpy(lcd0_buff,clear_buff);
					sprintf(lcd0_buff,"Small Quantity");
					move_cursor(0,0);
					lcd_string(lcd0_buff);
					*/
					move_cursor(0,0);
					lcd_string("Small Quantity");

				}
				if(auto_menu_select==2)//강력
				{
					/*
					strcpy(lcd0_buff,clear_buff);
					sprintf(lcd0_buff," Power Wash");
					move_cursor(0,0);
					lcd_string(lcd0_buff);
					*/
					move_cursor(0,0);
					lcd_string(" Power Wash");
				}
				if(auto_menu_select==3)//이불
				{
					move_cursor(0,0);
					lcd_string("   Blankets");
				}
				//lcd 1,0 출력
				if(run_flag)//작동 상태라면
				{	/*
					strcpy(lcd1_buff,clear_buff);
					sprintf(lcd1_buff,"  Running!!");
					move_cursor(1,0);
					lcd_string(lcd1_buff);
					*/
					move_cursor(1,0);
					lcd_string("    Running!!");
				}
				else if(!run_flag)
				{
					/*
					strcpy(lcd1_buff,clear_buff);
					sprintf(lcd1_buff,"    Pause!!");
					move_cursor(1,0);
					lcd_string(lcd1_buff);
					*/
					move_cursor(1,0);
					lcd_string("    Pause!!");
				}
			}

		}//자동모드 끝
	}//전원 off 끝
}


#endif
