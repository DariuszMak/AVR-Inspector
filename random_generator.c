#include "random_generator.h"

volatile uint8_t pwm1, pwm2;

void random_generator_init ( void )
{
	TCCR2 |= ( 1 << WGM21 );// tryb CTC
	TCCR2 |= ( 1 << CS20 ) | ( 1 << CS21 ) | ( 1 << CS22 ); // preskaler 1024
	OCR2 = 10;
	TIMSK |= ( 1 << OCIE2 );
}

ISR( _VECTOR( 4 ) )
{
	static uint8_t cnt;

	if(!cnt) PCF8583_get_wall_time();

	++cnt;
}
