#include <avr/io.h>
#include <avr/interrupt.h>
#include "d_led.h"

volatile uint8_t cy1;
volatile uint8_t cy2;
volatile uint8_t cy3;
volatile uint8_t cy4;

uint8_t cyfry[10] =
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
};

void d_led_init( void )
{
	LED_DATA_DIR = 0xFF;
	LED_DATA = 0xFF;

	ANODY_DIR |= CA1 | CA2 | CA3 | CA4;
	ANODY_PORT |= CA1 | CA2 | CA3 | CA4;

	TCCR0 |= ( 1 << WGM01 ); // tryb CTC timera 0
	TCCR0 |= ( 1 << CS02 ) | ( 1 << CS00 ); // preskaler 1024
	OCR0 = 78; // specjalna zmienna - rejestr przepe³nienia (maksymalna wartoœæ = 255)
	TIMSK |= ( 1 << OCIE0 ); // zezwolenie na przerwania COMPARE MATCH


}

ISR( TIMER0_COMP_vect )
{
	static uint8_t licznik = 1;
	ANODY_PORT = ~licznik;
	if( licznik == 1 ) LED_DATA = cyfry[cy1];
	else if( licznik == 2 ) LED_DATA = cyfry[cy2];
	else if( licznik == 4 ) LED_DATA = cyfry[cy3];
	else if( licznik == 8 ) LED_DATA = cyfry[cy4];

	licznik <<= 1;
	if( licznik > 8 ) licznik = 1;
}
