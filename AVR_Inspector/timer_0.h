#ifndef _TIMER_0_H_
#define _TIMER_0_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "main.h"


void timer_0_init( void );

void checking_pins_interrupt_on();

void checking_pins_interrupt_off();


#endif // _RANDOM_GENERATOR_H_
