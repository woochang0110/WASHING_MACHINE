#include "main.h" //for GPIO handling

#define BUTTON_RELEASE 1 //pull-up(IDLE)
#define BUTTON_PRESS 0	//ACTIVE LOW(ACTIVE)
#define BUTTON_NUMBER 8 //NUM Of BTN
#define FUNC_NUM 8
int get_button(GPIO_TypeDef*,uint16_t,uint8_t);
