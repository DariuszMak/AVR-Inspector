#include <avr/io.h>
#include <avr/interrupt.h>
#include "pwm_led.h"

volatile uint8_t pwm1, pwm2;

void pwm_led_init ( void )
{
	DDRD |= ( 1 << PD2 ) | ( 1 << PD3 ); // dwie diody dterowane PWM
	PORTD |= ( 1 << PD2 ) | ( 1 << PD3 ); // dwie diody dterowane PWM
	TCCR2 |= ( 1 << WGM21 );// tryb CTC
	TCCR2 |= ( 1 << CS20 ) | ( 1 << CS22 ); // preskaler 1
	OCR2 = 10;
	TIMSK |= ( 1 << OCIE2 );
}

ISR( _VECTOR( 4 ) )
{
	static uint8_t cnt;
	if( cnt >= pwm1 ) PORTD |= ( 1 << PD2 );
	else PORTD &= ~( 1 << PD2 );
	if( cnt >= pwm2 ) PORTD |= ( 1 << PD3 );
	else PORTD &= ~( 1 << PD3 );
	cnt++;
}
