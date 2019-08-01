#include "timer_2.h"

void timer_2_init( void )
{
    TCCR2 |= ( 1 << WGM21 );// tryb CTC
    //TCCR2 |= ( 1 << CS20 ) | ( 1 << CS21 ) | ( 1 << CS22 ); // preskaler 1024
    OCR2 = 5;
    TIMSK |= ( 1 << OCIE2 );
    cnt = 0;

    timer_cycle_overflow = 0;

    overflow_timer_2 = 0;
    interr = 0;
}

void RGB_init()
{
    RGB_R_DIR |= RGB_R;
    RGB_G_DIR |= RGB_G;
    RGB_B_DIR |= RGB_B;
    RGB_Red = 0;
    RGB_Green = 0;
    RGB_Blue = 0;
}

void refreshing_interrupt_on()
{
    TCCR2 |= ( 1 << CS20 ) | ( 1 << CS21 ) | ( 1 << CS22 ); // preskaler 1024, timer do odświeżania
}

void refreshing_interrupt_off()
{
    TCCR2 &= ~(( 1 << CS20 ) | ( 1 << CS21 ) | ( 1 << CS22 )); // wyłączenie timera preskaler 1024, timer do odświeżania
}

ISR( TIMER2_COMP_vect  )
{
    if(timer_cycle_overflow == 0) timer_cycle_overflow = 1;
    if( interr == 0 && overflow_timer_2 == 1200 )
    {
        interr = 1;
    }
    else ++overflow_timer_2;

    if( cnt >= RGB_Red ) RGB_R_PORT |= RGB_R;
    else RGB_R_PORT &= ~RGB_R;
    if( cnt >= RGB_Green ) RGB_G_PORT |= RGB_G;
    else RGB_G_PORT &= ~RGB_G;
    if( cnt >= RGB_Blue ) RGB_B_PORT |= RGB_B;
    else RGB_B_PORT &= ~RGB_B;
    ++cnt;
}
