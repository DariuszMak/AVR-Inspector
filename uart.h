#ifndef UART_H
#define UART_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdio.h>

//rozmiar bufora musi byæ naturaln¹ potêg¹ liczby 2
#define UART_WRITE_BUFFER_SIZE 32

#define UART_READ_BUFFER_SIZE 16

#define UART_WRITE_BUFFER_MASK (UART_WRITE_BUFFER_SIZE-1)

#define UART_READ_BUFFER_MASK (UART_READ_BUFFER_SIZE-1)

//flaga sygnalizuj¹ca przepe³nienie bufora odbiorczego
//volatile uint8_t rx_overrun;

void uart_init(uint16_t baud);

void USART_Transmit(uint8_t c, FILE *stream);
void uart_putc(char data);
void uart_puts(char *string);
//void uart_putint(int number, uint8_t base);

char uart_getc(void);
//void uart_gets(char * temporary_table, uint16_t size_of_table);
//uint16_t uart_getint(void);

#endif //UART_H
