#ifndef _pwm_led_h
#define _pwm_led_h

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

extern volatile uint8_t pwm1, pwm2;

void pwm_led_init ( void );

#endif // _pwm_led_h
