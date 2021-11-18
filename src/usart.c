#include <avr/io.h>
#include <stdint.h>
#include "usart.h"
#include <string.h>


void USARTInit(int buad_rate) {
    // Set baud rate
    UBRR0 = (((F_CPU / (buad_rate * 16UL))) - 1);
    //UBRR0 = UBRR_VALUE;
    // Set frame format to 8 data bits, no parity, 1 stop bit
    UCSR0C |= (1<<UCSZ01)|(1<<UCSZ00);
    //enable transmission and reception
    UCSR0B |= (1<<RXEN0)|(1<<TXEN0);
}

void USARTSendByte(uint8_t u8Data) {
    //wait while previous byte is completed
    while(!(UCSR0A&(1<<UDRE0))){};
    // Transmit data
    UDR0 = u8Data;
}

void USARTSendString(char string[]){
    uint8_t size = strlen(string);


    // loop through char array 
    for(uint8_t i = 0; i < size; i++){
        USARTSendByte( (uint8_t)string[i] );
    }
    USARTSendByte('\n');
    return;
}

uint8_t USARTReceiveByte() {
    // Wait for byte to be received
    while(!(UCSR0A&(1<<RXC0))){};
    // Return received data
    return UDR0;
}
