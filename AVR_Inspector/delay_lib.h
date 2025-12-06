#ifndef DELAY_LIB_H_
#define DELAY_LIB_H_
#include <util/delay.h>
#include <avr/wdt.h>

void delay_ms_var( uint16_t ); // czekaj określoną ilość milisekund
void delay_us_var( uint16_t ); // czekaj określoną ilość mikrosekund
void delay_ms_var_double( double ); // czekaj określoną ilość milisekund
void delay_us_var_double( double ); // czekaj określoną ilość mikrosekund

#endif // DELAY_LIB_H_
