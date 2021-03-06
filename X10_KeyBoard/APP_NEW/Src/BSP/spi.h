#ifndef __SPI_H__
#define __SPI_H__

#include "includes.h"
#include "FIFO.h"
#include "system.h"


#define USER_SIMULATE_SPI    		1


//NSS
#define  SPI_NSS_LOW() 				gpio_bit_reset(GPIOA, GPIO_PIN_15);
#define  SPI_NSS_HIGH() 			gpio_bit_set(GPIOA, GPIO_PIN_15)
//CPND
#define NPD_LOW()       			gpio_bit_reset(GPIOF, GPIO_PIN_6)		//CPDN	//RST
#define NPD_HIGH()      			gpio_bit_set(GPIOF, GPIO_PIN_6)			//CPDN	//RST	

//clock
#define SPI_CLK_LOW()   			gpio_bit_reset(GPIOB, GPIO_PIN_3)		//CSCK
#define SPI_CLK_HIGH()  			gpio_bit_set(GPIOB, GPIO_PIN_3)			//CSCK
//master out
#define SPI_MOSI_LOW()   			gpio_bit_reset(GPIOB, GPIO_PIN_5)
#define SPI_MOSI_HIGH()  			gpio_bit_set(GPIOB, GPIO_PIN_5)
//MISO
#define SPI_MISO_READ() 			gpio_input_bit_get(GPIOB, GPIO_PIN_4)

//#define FM1752_CIRQ_LOW()    		gpio_bit_reset(GPIOA, GPIO_PIN_1)
//#define FM1752_CIRQ_HIGH()     		gpio_bit_set(GPIOA, GPIO_PIN_1)




extern void SpiInit(void);
extern uint8_t SpiReadWriteByte(uint8_t byte);
extern uint16_t SpiSendHalfWord(uint16_t half_word);
extern uint8_t SpiReadByte(void);
extern uint8_t SPITest(uint8_t send_data);



#endif




