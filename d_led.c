#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "d_led.h"
#include <stdlib.h>

volatile uint8_t cy1;
volatile uint8_t cy2;
volatile uint8_t cy3;
volatile uint8_t cy4;

const uint8_t cyfry[12] PROGMEM =
{
    ~( SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F ),//0
    ~( SEG_B | SEG_C ),//1
    ~( SEG_A | SEG_B | SEG_D | SEG_E | SEG_G ),//2
    ~( SEG_A | SEG_B | SEG_C | SEG_D | SEG_G ),//3
    ~( SEG_B | SEG_C | SEG_F | SEG_G ),//4
    ~( SEG_A | SEG_C | SEG_D | SEG_F | SEG_G ),//5
    ~( SEG_A | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G ),//6
    ~( SEG_A | SEG_B | SEG_C | SEG_F ),//7
    ~( SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G ),//8
    ~( SEG_A | SEG_B | SEG_C | SEG_D | SEG_F | SEG_G ),//9
    ( SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G | SEG_DP ),//puste pole
    ~( SEG_G )
};

void d_led_init( void )
{
    LED_DATA_DIR = 0xFF;
    LED_DATA = 0xFF;

    ANODY_DIR |= CA1 | CA2 | CA3 | CA4;
    ANODY_PORT |= CA1 | CA2 | CA3 | CA4;

    TCCR0 |= ( 1 << WGM01 ); // tryb CTC timera 0
    TCCR0 |= ( 1 << CS02 ) | ( 1 << CS00 ); // preskaler 1024
    OCR0 = 78; //78 specjalna zmienna - rejestr przepe³nienia (maksymalna wartoœæ = 255)
    TIMSK |= ( 1 << OCIE0 ); // zezwolenie na przerwania COMPARE MATCH
}

ISR( TIMER0_COMP_vect )
{
    static uint8_t licznik = 1;
    ANODY_PORT = ( ANODY_PORT & 0xF0 ) | ( 0x0F );
    if( licznik == 1 ) LED_DATA = pgm_read_byte( &cyfry[cy1] );
    else if( licznik == 2 ) LED_DATA = pgm_read_byte( &cyfry[cy2] );
    else if( licznik == 4 ) LED_DATA = pgm_read_byte( &cyfry[cy3] );
    else if( licznik == 8 ) LED_DATA = pgm_read_byte( &cyfry[cy4] );
    ANODY_PORT = ( ANODY_PORT & 0xF0 ) | ( ~licznik & 0x0F );
    licznik <<= 1;
    if( licznik > 8 ) licznik = 1;
}

void d_led_Int ( int dana )
{
    if ( dana >= 10000 || dana <= -1000 ) dana = 0;//zakres ziennych do wyświetlania
    int dana_temp = abs( dana );

    int d = 1;
    int g = 10;
    while ( dana_temp >= g )
    {
        d += 1;
        g *= 10;
    }

    //w zmiennej g jest wielkość danej liczby

    int f;
    int h;
    g = 10;

    for( f = 0; f < d; ++f)//wyłuskiwanie poszczególnych cyfr od najmniej znaczącej
    {
        h = dana_temp % g;
        if( f == 1 ) h -= cy4;
        if( f == 2 ) h -= cy3 + cy4;
        if( f == 3 ) h -= cy2 + cy3 + cy4;
        h /= g / 10;

        if ( f == 0 ) cy4 = h;
        if ( f == 1 ) cy3 = h;
        if ( f == 2 ) cy2 = h;
        if ( f == 3 ) cy1 = h;

        g *= 10;
    }

    for ( f = d; f < 5; ++f)//uzupełnianie o puste pola, gdzie już nie ma cyfr liczby
    {
        if(f == 1 )
        {
            cy3 = 10;
            if ( dana_temp == 0 ) cy4 = 10;
        }

        if(f == 2 ) cy2 = 10;
        if(f == 3 ) cy1 = 10;
    }

    if(dana < 0)//dopisnie znaku "minus"
    {
        if ( d == 1 ) cy3 = 11;
        if ( d == 2 ) cy2 = 11;
        if ( d == 3 ) cy1 = 11;
    }
}
