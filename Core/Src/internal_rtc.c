#if 0
#include "internal_rtc.h"
#include "button.h"
#include "i2c_lcd.h"



//===function
extern int get_button(GPIO_TypeDef *GPIO,uint16_t GPIO_PIN,uint8_t button_number);
void get_rtc_time(void);
void set_rtc_time(char *date_time);
void lcd_display_time(void);
void set_time_button_ui(void);

unsigned char bin2dec(unsigned char byte);
unsigned char dec2bin(unsigned char byte);

//===variable

//from main.c
extern RTC_HandleTypeDef hrtc;
extern uint8_t lcd_display_mode_flag;//0: RTC display   1: Humid&Temp  2: UltraSonic Distance
extern RTC_TimeTypeDef mTime; // button.c time information

RTC_TimeTypeDef sTime = {0}; // time information
RTC_DateTypeDef sDate ={0};	//date information
extern RTC_TimeTypeDef mTime; // button.c time information

//char lcd0_buff[20],lcd1_buff[20],lcd5_buff[20];//0,1= Time


//get real time
void get_rtc_time(void)
{
	static RTC_TimeTypeDef oldTime;//previous time info
	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BCD);//binary를 decimal로 convert 해줘야됨.
	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BCD);

	if(oldTime.Seconds != sTime.Seconds)//if time change
	{

		oldTime.Seconds = sTime.Seconds;
		//comport master에 YYYY-MM-DD HH:mm:ss 로 출력
		printf("%04d-%02d-%02d %02d:%02d:%02d\n",bin2dec(sDate.Year)+2000
			,bin2dec(sDate.Month),bin2dec(sDate.Date),bin2dec(sTime.Hours)
			,bin2dec(sTime.Minutes),bin2dec(sTime.Seconds));

		lcd_display_time();

	}
}



//set real time
//uart pc_command_processing에서
//setrtc231016103810이 넘어옴
//      6789순으로 index 올라감.
void set_rtc_time(char *date_time)
{
	char yy[4]={0},mm[4]={0},dd[4]={0}; //date
	char hh[4]={0},min[4]={0},ss[4]={0};//time
//parsing
	strncpy(yy,date_time+6,2);// yy=23  yy[0]='2' yy[1]='3' yy[2]=0
	strncpy(mm,date_time+8,2); // mm=10
	strncpy(dd,date_time+10,2);//dd=16
	strncpy(hh,date_time+12,2);//hh=10
	strncpy(min,date_time+14,2);//min=38
	strncpy(ss,date_time+16,2); ////ss=10
//ascii->int->bcd
	sDate.Year=dec2bin(atoi(yy));
	sDate.Month=dec2bin(atoi(mm));
	sDate.Date=dec2bin(atoi(dd));
	sTime.Hours=dec2bin(atoi(hh));
	sTime.Minutes=dec2bin(atoi(min));
	sTime.Seconds=dec2bin(atoi(ss));
	HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD);
	HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD);

}


unsigned char bin2dec(unsigned char byte)//왜 uint8_t 안쓰는지? 구조체 내부에 값들은 uint8_t인데?
{
	unsigned char high_nibble, low_nibble;

	high_nibble = (byte>>4) *10;
	low_nibble = byte & 0x0f;
	return high_nibble+low_nibble;
}


unsigned char dec2bin(unsigned char byte)
{
	unsigned char high_nibble, low_nibble;


	high_nibble=(byte / 10)<<4;
	low_nibble=byte%10;
	return high_nibble + low_nibble;
}


void lcd_display_time(void){

	if(lcd_display_mode_flag==0)
	{
		lcd_command(CLEAR_DISPLAY);
		char lcd0_buff[20],lcd1_buff[20];

		sprintf(lcd0_buff,"%5s%04d-%02d-%02d"," ",bin2dec(sDate.Year)+2000
					,bin2dec(sDate.Month),bin2dec(sDate.Date));
		sprintf(lcd1_buff,"%4s%02d:%02d:%02d"," ",bin2dec(sTime.Hours)
					,bin2dec(sTime.Minutes),bin2dec(sTime.Seconds));
		move_cursor(0,0);
		lcd_string(lcd0_buff);
		move_cursor(1,0);
		lcd_string(lcd1_buff);
	}

	if(lcd_display_mode_flag==3)
	{

		lcd_command(CLEAR_DISPLAY);

		//sprintf(lcd5_buff,"  %02d:%02d:%02d",bin2dec(mTime.Hours)
							,bin2dec(mTime.Minutes),bin2dec(mTime.Seconds));
		move_cursor(0,0);
		//lcd_string(lcd1_buff);
		//(1,0);
		lcd_string(lcd5_buff);

	}
}

//btn0 시간 Hour 정보 변경 00~23(up count)
//btn1 분 min 정보 변경 00~60(up count)
//btn2 초 sec 정보 변경 00~60(up count)
//btn3 변경완료 현재까지 값 입력
void set_time_button_ui(void)
{
	if(lcd_display_mode_flag==3)
	{
		if(get_button(BUTTON_0_GPIO_Port,BUTTON_0_Pin,0)==0)
		{
			mTime.Hours++;
			if(mTime.Hours==24) mTime.Hours=0;
		}
		if(get_button(BUTTON_1_GPIO_Port,BUTTON_1_Pin,1)==0)
		{
			mTime.Minutes++;
			if(mTime.Minutes==60) mTime.Minutes=0;
			//mTime.Minutes=(mTime.Minutes+1)%60;  이렇게 치면 값이 이상하게 증가하는데 이유를 모르겠음.
		}
		if(get_button(BUTTON_2_GPIO_Port,BUTTON_2_Pin,2)==0)
		{
			mTime.Seconds++;
			if(mTime.Seconds==60) mTime.Seconds=0;
		}
		if(get_button(BUTTON_3_GPIO_Port,BUTTON_3_Pin,3)==0)
		{
			HAL_RTC_SetTime(&hrtc, &mTime, RTC_FORMAT_BCD);
			lcd_display_mode_flag=0;
		}

	}

}
#endif
