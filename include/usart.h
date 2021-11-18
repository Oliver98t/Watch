#ifndef USART_H
#define USART_H

#include <stdio.h>

void USARTInit(int baud_rate);
void USARTSendByte(uint8_t u8Data);
void USARTSendString(char string[]);
uint8_t USARTReceiveByte();




#endif