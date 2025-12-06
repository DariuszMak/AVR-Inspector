//Pomiar napiêcia przetwornikiem A/C i prezentacja wyniku na LCD 2x16 HD44780

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "HD44780.h"
#include "ir_decode.h"
#include "d_led.h"
#define _delay_ms delay_ms_var_double
#define _delay_us delay_ms_var_double

//Program glowny:
//##############################################################################

int main( void )
{
// PORTD7 jako wyjście do buzzera
//##############################################################################

	DDRD |= ( 1 << PD7 );

//##############################################################################

	int t; // zmienna pomocnicza wykorzystana w pętlach for do iteracji, może być używana do przeróżnych innych operacji w programie
	int rozmiar = 6; // zmienna odpowiedzialna za rozmiar tablicy dynamicznej
	int	cyfry = 0; // zmienna przechowująca wartość wyświetlaną póżniej na wyświetlaczu alfanumerycznym
	int zwiekszanie = 0; // zmienna potrzebna do zmiany wartości liczby na wyświetlaczu alfanumerycznym (przyjmuje wartości 1,10,100,1000)
	int menu = 0;// zmienna odpowiedzialna za przebywanie w danym podprogramie
	int start = 1; // zmienna pomocna do stwierdzenia, czy jest się już w glownym menu, czy nie (dosyć zagmatwany mechanizm)


//funkcje
//##############################################################################

	void buzzer( void )
	{
		PORTD |= ( 1 << PD7 );
		_delay_ms( 1 );
		PORTD &= ~( 1 << PD7 );
	}

	void wybor( int number ) // funkcja wyświetlająca na początku wchodenia w dany podprogram numeru podprogramu
	{
		LCD_Clear();
		LCD_WriteText( "Program: " );
		LCD_Int( number );
		_delay_ms( 500 );
		LCD_Clear();
	}

	void wysw( int men, int com ) // funkcja wyświetlająca dla każdego z podprogramów,
	{
		switch ( men )
		{
		case 0:
			LCD_EraseAll();
			LCD_GoTo( 0, 0 );
			LCD_WriteText( "Wybierz:" );
			LCD_GoTo( 0, 1 );
			LCD_WriteText( "1 - 3" );
			break;
		case 1:
			LCD_EraseAll();
			LCD_GoTo( 0, 0 );
			LCD_Int( com );
			LCD_GoTo( 6, 0 );
			LCD_Int( address );
			LCD_GoTo( 0, 1 );
			LCD_Int( toggle_bit );
			break;
		case 2:
			if ( cyfry >= 10000 ) cyfry = 10000;
			else if( cyfry <= -10000 ) cyfry = -10000;
			OCR0 = cyfry;
			LCD_EraseAll();
			d_led_Int( cyfry );
			LCD_GoTo( 0, 0 );
			LCD_Int( cyfry );
			break;
		case 3:
			LCD_EraseAll();
			LCD_GoTo( 0, 0 );
			LCD_WriteText( "PWM dla diod" );
			break;
		}
	}

// najważniejsza i najbardziej skomplikowana funkcja
//##############################################################################
	void pilot( int *men , int com )
	{
		if( !start ) start = 2;//start przyjmuje wartość inną od 0 albo 1
		buzzer();
		switch( *men )//warianty w zależności od zmiennej menu, na końcu każdego wywoływana jest funkcja wyświetlająca
		{
//##############################################################################
		case 0:
			switch ( com )
			{
			case 12:
				LCD_Clear();
				LCD_GoTo( 9, 1 );
				LCD_WriteText( "Witaj!" );
				_delay_ms( 500 );
				LCD_Home();
				LCD_WriteText( "(D)Arek" );
				_delay_ms( 500 );

				LCD_ShiftRightScreen();
				_delay_ms( 700 );

				LCD_ScreenOff();
				_delay_ms( 700 );
				LCD_ScreenOn();
				_delay_ms( 700 );

				LCD_ScreenOff();
				_delay_ms( 700 );
				LCD_CursorBlink();
				_delay_ms( 1000 );
				LCD_Blink() ;
				LCD_ShiftRightCursor();
				_delay_ms( 1000 );
				LCD_ShiftLeftCursor();
				LCD_Cursor();
				_delay_ms( 1000 );
				LCD_CursorBlink() ;
				LCD_GoTo( 0, 1 );
				LCD_WriteText( "Czytam:" );
				_delay_ms( 1500 );

				LCD_GoTo( 9, 1 );


				char * i =  ( char* ) malloc( rozmiar * sizeof * i );

				void pisz( void )
				{
					for( t = 0; t < rozmiar; t++ )
					{
						LCD_WriteData( i[t] );
						_delay_ms( 50 );
					}
				}

				for( t = 0; t < rozmiar; t++ )
				{
					i[t] = LCD_ReadData();
					_delay_ms( 100 );
				}
				LCD_ShiftLeftScreen();
				_delay_ms( 1500 );

				LCD_Cursor();
				LCD_EraseAll();
				_delay_ms( 1500 );
				LCD_GoTo( 0, 1 );
				pisz();
				LCD_Blink();
				LCD_GoTo( 9, 0 );
				pisz();
				LCD_GoTo( 8, 1 );
				LCD_CursorBlink();
				pisz();
				LCD_GoTo( 1, 0 );
				pisz();

				free( i );

				LCD_Cursor();
				_delay_ms( 1500 );
				LCD_PageUpScreen ();
				LCD_PageDownScreen();
				LCD_EraseUp();
				LCD_Blink();
				_delay_ms( 1000 );
				LCD_PageDownScreen();
				LCD_PageUpScreen();
				LCD_Cursor();
				LCD_EraseDown();
				_delay_ms( 500 );

				LCD_Clear();
				LCD_ScreenOn();
				break;
			}
			wysw( *men, com );
			break;
//##############################################################################
		case 1:

			wysw( *men, com );

			break;
//##############################################################################
		case 2:

			switch( com )
			{
			case 59:
				cy1 = 9;
				cy2 = 0;
				cy3 = 0;
				cy4 = 0;
				_delay_ms( 1000 );
				cy1 = 5;
				cy2 = 6;
				cy3 = 7;
				cy4 = 8;
				_delay_ms( 1000 );
				cy1 = 1;
				cy2 = 2;
				cy3 = 3;
				cy4 = 4;
				_delay_ms( 1000 );
				break;
			case 55:
				zwiekszanie = 1000;
				break;
			case 54:
				zwiekszanie = 100;
				break;
			case 50:
				zwiekszanie = 10;
				break;
			case 52:
				zwiekszanie = 1;
				break;
			case 32:
				TCCR0 |= ( 1 << CS02 ) | ( 1 << CS00 ); // timer włączony
				break;
			case 33:
				TCCR0 &= ~( ( 1 << CS02 ) | ( 1 << CS00 ) ); // timer wyłączony
				break;
			case 17:
				cyfry -= zwiekszanie;
				break;
			case 16:
				cyfry += zwiekszanie;
				break;
			}
			wysw( *men, com );
			break;
//##############################################################################
		case 3:
			wysw( *men, com );
			break;
		}
//komendy dla wszystkich podprogramów
//##############################################################################
		switch ( com )
		{
		case 38:
			LCD_Clear();
			LCD_WriteText( "Kalibracja" );
			LCD_GoTo( 0, 1 );
			LCD_WriteText( "ekranu..." );
			_delay_ms( 250 );
			LCD_PageUpScreen();
			LCD_PageDownScreen();
			LCD_Clear();
			wysw( *men, com );
			break;
		case 34:
			LCD_ShiftRightScreen();
			break;
		case 46:
			LCD_ShiftLeftScreen();
			break;
		case 35:
			LCD_PageUpScreen();
			break;
		case 36:
			LCD_PageDownScreen();
			break;

		case 14:
			*men = 0;
			start = 1;
			break;
		}

		if( menu == 0 )
		{
//ważne opcje przy wchodzeniu/wychodzeniu z podprogramów
//##############################################################################
			if ( start == 1 )
			{
				TCCR0 &= ~( ( 1 << CS02 ) | ( 1 << CS00 ) ); // timer wyłączony
				wybor( *men );
				start = 0;
				pilot( &menu, 59 );

			}

			if( com > 0 && com <= 3 )
			{
				*men = com;
				start = 0;
				wybor( *men );

				switch( *men )//można podać tu komendy które mają wykonać się podczas wchodzenia do podprogramu
				{
				case 1:
					wysw( *men, com );
					break;

				case 2:
					TCCR0 |= ( 1 << CS02 ) | ( 1 << CS00 ); // timer włączony
					pilot( &menu, 55 ); // ta komenda działa tak, jaby się wcisnęło przycisk na pilocie RC5
					pilot( &menu, 16 );
					pilot( &menu, 54 );
					pilot( &menu, 16 );
					pilot( &menu, 50 );
					pilot( &menu, 16 );
					pilot( &menu, 52 );
					pilot( &menu, 16 );
					break;

				case 3:
					wysw( *men, com );
					break;
				}

			}
		}
	}

	int zczytaj_komende( void ) // funkcja odpowiedzialna za odczytanie komend z pilota i przekazaniu ich do fukcji pilot, dopóki nie zostaną wykonane wszystkie rozkacy, nie będzie można odzczytać innego przysisku
	{
		if( Ir_key_press_flag )
		{
			if( !address )
			{
				pilot( &menu, command );
				Ir_key_press_flag = 0;
				command = 0xff;
				address = 0xff;
				return 1;

			}
		}
	}



//##############################################################################

	LCD_Initalize();
	ir_init();
	d_led_init();
	sei();
	pilot( &menu, 0 );
	while( 1 )
	{
		zczytaj_komende();
	}



	return 0;
}


