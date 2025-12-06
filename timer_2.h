#ifndef _TIMER_2_H_
#define _TIMER_2_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "main.h"

#define RGB_R_DIR	    DDRB
#define RGB_R_PORT	    PORTB
#define RGB_R_PIN		PINB
#define RGB_R			(1 << PB1)

#define RGB_G_DIR	    DDRB
#define RGB_G_PORT	    PORTB
#define RGB_G_PIN		PINB
#define RGB_G			(1 << PB2)

#define RGB_B_DIR	    DDRB
#define RGB_B_PORT	    PORTB
#define RGB_B_PIN		PINB
#define RGB_B			(1 << PB3)


volatile uint8_t RGB_Red, RGB_Green, RGB_Blue;

void RGB_init();

void timer_2_init( void );

void refreshing_interrupt_on();

void refreshing_interrupt_off();

volatile uint8_t cnt;
volatile uint16_t overflow_timer_2;
volatile uint8_t timer_cycle_overflow;
volatile uint8_t interr;
#endif // _RANDOM_GENERATOR_H_
