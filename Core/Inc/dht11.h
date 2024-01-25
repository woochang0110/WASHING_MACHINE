/*
 * DHT11.h
 *
 *  Created on: 2019. 9. 2.
 *      Author: kccistc
 */

#ifndef SRC_DHT11_H_
#define SRC_DHT11_H_

#define DHT11_PORT			GPIOA
#define DHT11_DATA_RIN		GPIO_PIN_0
void DHT11_Init(void);
void DHT11_trriger(void);
void DHT11_DataLine_Input(void);
void DHT11_DataLine_Output(void);
uint8_t DHT11_rx_Data(void);
void DHT11_dumi_read(void);



#endif /* SRC_DHT11_H_ */
