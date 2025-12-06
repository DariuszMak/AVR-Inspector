#ifndef _RANDOM_GENERATOR_H_
#define _RANDOM_GENERATOR_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "main.h"

void random_generator_init ( void );

void refreshing_interrupt_on();

void refreshing_interrupt_off();

volatile uint8_t cnt;
volatile uint8_t interr;
#endif // _RANDOM_GENERATOR_H_
