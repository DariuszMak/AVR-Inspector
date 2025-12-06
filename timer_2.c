#include "timer_2.h"

volatile uint8_t pwm1, pwm2;

void timer_2_init( void )
{
    TCCR2 |= ( 1 << WGM21 );// tryb CTC
    //TCCR2 |= ( 1 << CS20 ) | ( 1 << CS21 ) | ( 1 << CS22 ); // preskaler 1024
    OCR2 = 10;
    TIMSK |= ( 1 << OCIE2 );
}

void RGB_init()
{
    RGB_R_DIR |= RGB_R;
    RGB_G_DIR |= RGB_G;
    RGB_B_DIR |= RGB_B;
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
    if( overflow_timer_2 == 20 ) interr = 1;
    if( cnt >= RGB_Red ) RGB_R_PORT |= ( 1 << RGB_R );
    else RGB_R_PORT &= ~RGB_R;
    if( cnt >= RGB_Green ) RGB_G_PORT |= ( 1 << RGB_G );
    else RGB_G_PORT &= ~RGB_G;
    if( cnt >= RGB_Blue ) RGB_B_PORT |= ( 1 << RGB_B );
    else RGB_B_PORT &= ~RGB_B;
    ++overflow_timer_2;
    ++cnt;
}
