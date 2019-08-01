#include "delay_lib.h"

void delay_ms_var( uint16_t count )
{
    while( count-- )
    {
        wdt_reset();
        _delay_ms( 1 );
    }
}

void delay_us_var( uint16_t count )
{
    wdt_reset();
    while( count-- )
    {
        _delay_us( 1 );
    }
}

void delay_ms_var_double( double __ms )
{
    uint16_t __ticks;
    double __tmp = ( ( F_CPU ) / 4e3 ) * __ms;
    if ( __tmp < 1.0 )
        __ticks = 1;
    else if ( __tmp > 65535 )
    {
        //	__ticks = requested delay in 1/10 ms
        __ticks = ( uint16_t ) ( __ms * 10.0 );
        while( __ticks )
        {
            wdt_reset();
            // wait 1/10 ms
            _delay_loop_2( ( ( F_CPU ) / 4e3 ) / 10 );
            __ticks --;
        }
        return;
    }
    else
        __ticks = ( uint16_t )__tmp;
    wdt_reset();
    _delay_loop_2( __ticks );
}

void delay_us_var_double( double __us )
{
    uint8_t __ticks;
    double __tmp = ( ( F_CPU ) / 3e6 ) * __us;
    if ( __tmp < 1.0 )
        __ticks = 1;
    else if ( __tmp > 255 )
    {
        delay_ms_var_double( __us / 1000.0 );
        return;
    }
    else
        __ticks = ( uint8_t )__tmp;
    _delay_loop_1( __ticks );
}
