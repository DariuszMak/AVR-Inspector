#include "random_generator.h"

volatile uint8_t pwm1, pwm2;

void random_generator_init ( void )
{
    TCCR2 |= ( 1 << WGM21 );// tryb CTC
    //TCCR2 |= ( 1 << CS20 ) | ( 1 << CS21 ) | ( 1 << CS22 ); // preskaler 1024
    OCR2 = 255;
    TIMSK |= ( 1 << OCIE2 );
}

void refreshing_interrupt_on()
{
TCCR2 |= ( 1 << CS20 ) | ( 1 << CS21 ) | ( 1 << CS22 ); // preskaler 1024, timer do odświeżania
}

void refreshing_interrupt_off()
{
TCCR2 &= ~( 1 << CS20 ) | ( 1 << CS21 ) | ( 1 << CS22 ); // wyłączenie timera preskaler 1024, timer do odświeżania

}

ISR( _VECTOR( 4 ) )
{
    if( cnt == 20) interr = 1;
    ++cnt;
}
