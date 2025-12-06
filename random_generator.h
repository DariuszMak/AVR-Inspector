#ifndef _RANDOM_GENERATOR_H_
#define _RANDOM_GENERATOR_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "PCF8583.h"

void random_generator_init ( void );

	volatile uint8_t cnt;
	volatile uint8_t interr;
#endif // _RANDOM_GENERATOR_H_
