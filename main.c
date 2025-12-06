/*
 * main.c
 *
 *  Created on: 23-05-2013
 *      Author: Piotr Rzeszut
 *
 * Description: Przyk³ad obs³ugi zegara PCF8563
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include "uart.h"
//#include "twi.h"
//#include "uart.h"

uint8_t dec2bcd(uint8_t dane)
{
    return (((dane/10)<<4)|(dane%10));
}

uint8_t bcd2dec(uint8_t dane)
{
    return (((dane>>4)*10)+(dane&0x0F));
}

const char wd_names[7][4]=
{
    " Nd",
    "Pon",
    " Wt",
    " Sr",
    "Czw",
    " Pt",
    " Sb"
};

volatile uint8_t h,i,s=255,d,wd,m,y,s_old=255;

int main(void)
{

    uart_init(57000);//inicjalizacja uart'u

    DDRD &= ~(1<<PD2);
    PORTD |= (1<<PD2);//PD2 - wejœcie + pull-up

    sei();

    char table_of_test[10];
    uart_putint(sizeof(table_of_test),10);

    uart_gets(table_of_test,sizeof(table_of_test));

    uart_puts("co wyszlo:\r\n");

    uart_puts(table_of_test);
            uart_putc('\n');



    char lol = uart_getc();
    uart_putc(lol);
//	int16_t times;

    /*while(1)
    {
        lol = uart_getc();
        if(lol == 'A') for(times = 0; times < 1000; ++times) uart_putc(lol);
        uart_putc('T');

    }*/

    uart_puts("Ustawic inny czas? (0-NIE   1-TAK): ");
    h=uart_getint();
    uart_puts("\r\n");
    if(h!=0)
    {

        uart_puts("Rok (00-99): ");
        y=dec2bcd(uart_getint());
        uart_puts("\r\n");

        uart_puts("Miesiac: ");
        m=dec2bcd(uart_getint());
        uart_puts("\r\n");

        uart_puts("Dzien: ");
        d=dec2bcd(uart_getint());
        uart_puts("\r\n");

        uart_puts("Dzien tygodnia\r\n");

        uart_puts("Podaj liczbe z zakresu (0-6): ");
        wd=uart_getint();
        uart_puts("\r\n");

        uart_puts("Godzina: ");
        h=dec2bcd(uart_getint());
        uart_puts("\r\n");

        uart_puts("Minuta: ");
        i=dec2bcd(uart_getint());
        uart_puts("\r\n");

        uart_puts("Sekunda: ");
        s=dec2bcd(uart_getint());
        uart_puts("\r\n");

        _delay_ms(10);


        uart_puts("Czas zapisany!!!\r\n");

    }


    //do wyœwietlania u¿ywamy tricku - wyœwietlamy liczbê kodowan¹ jako BCD
    //tak jakby by³a to liczba w zapisie heksadecymalnym
    //dziêki temu odpowiednie liczby l¹duj¹ na swoich miejscach
    //bez koniecznoœci u¿ywania instrukcji bcd2dec
    if(h<0x10)uart_putc('0');
    uart_putint(h,16);

    uart_putc(':');

    if(i<0x10)uart_putc('0');
    uart_putint(i,16);

    uart_putc(':');

    if(s<0x10)uart_putc('0');
    uart_putint(s,16);

    uart_putc(' ');

    uart_puts((char*)wd_names[wd]);

    uart_putc(' ');

    if(d<0x10)uart_putc('0');
    uart_putint(d,16);

    uart_putc('-');

    if(m<0x10)uart_putc('0');
    uart_putint(m,16);

    uart_putc('-');

    if(y<0x10)uart_putc('0');
    uart_putint(y,16);

    uart_puts("\r\n");

    while(1);



    return 0;
}

