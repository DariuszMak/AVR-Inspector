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
    ~( (1<<0) | (1<<1) | (1<<2) | (1<<3) | (1<<4) | (1<<5) ),//0
    ~( (1<<1) | (1<<2) ),//1
    ~( (1<<0) | (1<<1) | (1<<3) | (1<<4) | (1<<6) ),//2
    ~( (1<<0) | (1<<1) | (1<<2) | (1<<3) | (1<<6) ),//3
    ~( (1<<1) | (1<<2) | (1<<5) | (1<<6) ),//4
    ~( (1<<0) | (1<<2) | (1<<3) | (1<<5) | (1<<6) ),//5
    ~( (1<<0) | (1<<2) | (1<<3) | (1<<4) | (1<<5) | (1<<6) ),//6
    ~( (1<<0) | (1<<1) | (1<<2) | (1<<5) ),//7
    ~( (1<<0) | (1<<1) | (1<<2) | (1<<3) | (1<<4) | (1<<5) | (1<<6) ),//8
    ~( (1<<0) | (1<<1) | (1<<2) | (1<<3) | (1<<5) | (1<<6) ),//9
    ( (1<<0) | (1<<1) | (1<<2) | (1<<3) | (1<<4) | (1<<5) | (1<<6) | (1<<7) ),//puste pole
    ~( (1<<6) )
};

void d_led_init( void )
{
    LED_DATA_SEG_A_DIR |= SEG_A;
    LED_DATA_SEG_A_PORT |= SEG_A;

    LED_DATA_SEG_B_DIR |= SEG_B;
    LED_DATA_SEG_B_PORT |= SEG_B;

    LED_DATA_SEG_C_DIR |= SEG_C;
    LED_DATA_SEG_C_PORT |= SEG_C;

    LED_DATA_SEG_D_DIR |= SEG_D;
    LED_DATA_SEG_D_PORT |= SEG_D;

    LED_DATA_SEG_E_DIR |= SEG_E;
    LED_DATA_SEG_E_PORT |= SEG_E;

    LED_DATA_SEG_F_DIR |= SEG_F;
    LED_DATA_SEG_F_PORT |= SEG_F;

    LED_DATA_SEG_G_DIR |= SEG_G;
    LED_DATA_SEG_G_PORT |= SEG_G;

    LED_DATA_SEG_DP_DIR |= SEG_DP;
    LED_DATA_SEG_DP_PORT |= SEG_DP;

    LED_ANODY_CA1_DIR |= CA1;
    LED_ANODY_CA1_PORT |= CA1;

    LED_ANODY_CA2_DIR |= CA2;
    LED_ANODY_CA2_PORT |= CA2;

    LED_ANODY_CA3_DIR |= CA3;
    LED_ANODY_CA3_PORT |= CA3;

    LED_ANODY_CA4_DIR |= CA4;
    LED_ANODY_CA4_PORT |= CA4;



    TCCR0 |= ( 1 << WGM01 ); // tryb CTC timera 0
    TCCR0 |= ( 1 << CS02 ) | ( 1 << CS00 ); // preskaler 1024
    OCR0 = 78; //78 specjalna zmienna - rejestr przepe³nienia (maksymalna wartoœæ = 255)
    TIMSK |= ( 1 << OCIE0 ); // zezwolenie na przerwania COMPARE MATCH
}

ISR( TIMER0_COMP_vect )
{
    static uint8_t licznik = 1;

    LED_ANODY_CA1_PORT |= CA1;//wygaszenie wszystkich elementów
    LED_ANODY_CA2_PORT |= CA2;
    LED_ANODY_CA3_PORT |= CA3;
    LED_ANODY_CA4_PORT |= CA4;

    uint8_t temp = 0;

    if( licznik == 1 ) temp = cy1;
    else if( licznik == 2 ) temp = cy2;
    else if( licznik == 4 ) temp = cy3;
    else if( licznik == 8 ) temp = cy4;

    uint8_t led_sign = pgm_read_byte ( &cyfry[temp] );

    if ( led_sign & ( 1 << 0 ) )
    {
        LED_DATA_SEG_A_PORT |= SEG_A;
    }
    else
    {
        LED_DATA_SEG_A_PORT &= ~SEG_A;
    }

    if ( led_sign & ( 1 << 1 ) )
    {
        LED_DATA_SEG_B_PORT |= SEG_B;
    }
    else
    {
        LED_DATA_SEG_B_PORT &= ~SEG_B;
    }

    if ( led_sign & ( 1 << 2 ) )
    {
        LED_DATA_SEG_C_PORT |= SEG_C;
    }
    else
    {
        LED_DATA_SEG_C_PORT &= ~SEG_C;
    }

    if ( led_sign & ( 1 << 3 ) )
    {
        LED_DATA_SEG_D_PORT |= SEG_D;
    }
    else
    {
        LED_DATA_SEG_D_PORT &= ~SEG_D;
    }

    if ( led_sign & ( 1 << 4 ) )
    {
        LED_DATA_SEG_E_PORT |= SEG_E;
    }
    else
    {
        LED_DATA_SEG_E_PORT &= ~SEG_E;
    }

    if ( led_sign & ( 1 << 5 ) )
    {
        LED_DATA_SEG_F_PORT |= SEG_F;
    }
    else
    {
        LED_DATA_SEG_F_PORT &= ~SEG_F;
    }

    if ( led_sign & ( 1 << 6 ) )
    {
        LED_DATA_SEG_G_PORT |= SEG_G;
    }
    else
    {
        LED_DATA_SEG_G_PORT &= ~SEG_G;
    }

    if ( led_sign & ( 1 << 7 ) )
    {
        LED_DATA_SEG_DP_PORT |= SEG_DP;
    }
    else
    {
        LED_DATA_SEG_DP_PORT &= ~SEG_DP;
    }

    if(licznik == 1) LED_ANODY_CA1_PORT &= ~CA1;
    else if(licznik == 2) LED_ANODY_CA2_PORT &= ~CA2;
    else if(licznik == 4) LED_ANODY_CA3_PORT &= ~CA3;
    else if(licznik == 8) LED_ANODY_CA3_PORT &= ~CA4;

    licznik <<= 1;
    if( licznik > 8 ) licznik = 1;
}

void d_led_Int ( int dana )
{
    if ( dana >= 10000 || dana <= -1000 )//zakres zMiennych do wyświetlania
    {
        cy1 = 11;
        cy2 = 11;
        cy3 = 11;
        cy4 = 11;
        return;
    }

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
            //if ( dana_temp == 0 ) cy4 = 10;
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
