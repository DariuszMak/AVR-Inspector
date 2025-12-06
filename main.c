//Pomiar napiêcia przetwornikiem A/C i prezentacja wyniku na LCD 2x16 HD44780

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "HD44780.h"
#include "ir_decode.h"
#include "d_led.h"
#include "pwm_led.h"
#include <stdlib.h>
#define _delay_ms delay_ms_var_double
#define _delay_us delay_ms_var_double


//Program glowny:



int main( void )
{

	const int liczbaPodprogramow = 3;

	int rozmiar; // zmienna odpowiedzialna za rozmiar tablicy dynamicznej
	int t; // zmienna pomocnicza wykorzystana w pętlach for do iteracji, może być używana do przeróżnych innych operacji w programie
	int	cyfry = 0; // zmienna przechowująca wartość wyświetlaną póżniej na wyświetlaczu alfanumerycznym
	int zwiekszanie = 0; // zmienna potrzebna do zmiany wartości liczby na wyświetlaczu alfanumerycznym (przyjmuje wartości 1,10,100,1000)
	int menu = 0;// zmienna odpowiedzialna za przebywanie w danym podprogramie
	int start = 1; // zmienna pomocna do stwierdzenia, czy jest się już w glownym menu = 0, czy właśnie wyszło się z podprogramu i trzeba np. zatrzymać jakiś timer = 1

//definicje funkcji

	void buzzer( void )//funkcja odpowiedzialna za sygnał dźwiękowy (trwa jedną milisekundę
	{
		PORTD |= ( 1 << PD7 );
		_delay_ms( 1 );
		PORTD &= ~( 1 << PD7 );
	}

	void wybor( int number ) // funkcja wyświetlająca podczas wchodenia w dany podprogram numeru podprogramu
	{
		LCD_Clear();
		LCD_WriteText( "Program: " );
		LCD_Int( number );
		for ( t = 0; t < 5; ++t )
		{
			_delay_ms( 10 );
			buzzer();
		}
		_delay_ms( 500 );
		LCD_Clear();
	}

	void wysw( int men, int add ) // funkcja wyświetlająca - interfejs dla każdego z podprogramów
	{
		switch ( men )
		{
		case 0:
			LCD_EraseAll();
			LCD_GoTo( 0, 0 );
			LCD_WriteText( "Wybierz:" );
			LCD_GoTo( 0, 1 );
			LCD_WriteText( "1 - 3" );

			//LCDWriteToBuffer( 0, 0, "napis" );
			//LCD_WriteText ( "Pierwszy Napis abcdefghijklmnopqrstuvwxyz" );
			//LCD_WriteText ( "Drugi" );

			break;
		case 1:
			LCD_EraseAll();
			LCD_GoTo( 0, 0 );
			LCD_Int( add );
			LCD_GoTo( 6, 0 );
			LCD_Int( address );
			LCD_GoTo( 0, 1 );
			LCD_Int( toggle_bit );
			break;
		case 2:
			if ( cyfry >= 10000 ) cyfry = 10000;
			else if( cyfry <= -10000 ) cyfry = -10000;
			//OCR0 = cyfry;//zmienna przepełnienia Timera 0
			LCD_EraseAll();
			d_led_Int( cyfry );
			LCD_GoTo( 0, 0 );
			LCD_Int( cyfry );
			break;
		case 3:
			LCD_EraseAll();
			LCD_GoTo( 0, 0 );
			LCD_Int( pwm1 );
			LCD_GoTo( 0, 1 );
			LCD_Int( pwm2 );
			//OCR0 = pwm1;//zmienna przepełnienia Timera 0
			break;
		}
	}

	void wysw_skok( int number ) // funkcja wyświetlająca numer kroku o danej wartości
	{
		zwiekszanie = number;
		LCD_EraseAll();
		for ( t = 0; t < 40; t += 8 )
		{
			LCD_GoTo( t, 0 );
			LCD_Int( number );
			LCD_GoTo( t, 1 );
			LCD_Int( number );
		}
		_delay_ms( 500 );
	}

	void czynnosc( const int * const men, int com ) //funkcja odpowiedzialna za wywołanie odpowiedniej czynności (pierwszy argument musi być przez wskaźnik, ponieważ, może być dokonana zmiana zmiennej "menu")
	{
		buzzer();

		switch( *men )//warianty w zależności od zmiennej menu, na końcu każdego wywoływana jest funkcja wyświetlająca
		{

		case 0:
			switch ( com )
			{
			case 41:
				LCD_Clear();
				LCD_WriteText( "ATmega32 program" );
				LCD_GoTo( 0, 1 );
				LCD_WriteText( "Dariusz Makarew." );

				rozmiar = 16;

				while( 1 )
				{
					char * j =  ( char* ) malloc( rozmiar * sizeof * j * 2 );

					LCD_Home();

					for( t = 0; t < rozmiar; ++t )
					{
						j[t] = LCD_ReadData();
					}

					LCD_GoTo( 0, 1 );

					for( t = rozmiar; t < 2 * rozmiar; ++t )
					{
						j[t] = LCD_ReadData();
					}

					LCD_Clear();

					for( t = 0; t < rozmiar; ++t )
					{
						LCD_WriteData( j[t] );

					}

					LCD_GoTo( 0, 1 );

					for( t = rozmiar; t < 2 * rozmiar; ++t )
					{
						LCD_WriteData( j[t] );
					}
					free( j );
				}
				break;
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

				rozmiar = 6;

				char * i =  ( char* ) malloc( rozmiar * sizeof * i );

				void pisz( void )
				{
					for( t = 0; t < rozmiar; ++t )
					{
						LCD_WriteData( i[t] );
						_delay_ms( 50 );
					}
				}

				for( t = 0; t < rozmiar; ++t )
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

		case 1:
			LCD_Displaying( com );
			wysw( *men, com );
			break;

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
				wysw_skok( 1000 );
				break;
			case 54:
				wysw_skok( 100 );
				break;
			case 50:
				wysw_skok( 10 );
				break;
			case 52:
				wysw_skok( 1 );
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

		case 3:
			switch ( com )
			{
			case 55:
				wysw_skok( 1000 );
				break;
			case 54:
				wysw_skok( 100 );
				break;
			case 50:
				wysw_skok( 10 );
				break;
			case 52:
				wysw_skok( 1 );
				break;
			case 17:
				pwm1 -= zwiekszanie;
				break;
			case 16:
				pwm1 += zwiekszanie;
				break;
			case 32:
				pwm2 += zwiekszanie;
				break;
			case 33:
				pwm2 -= zwiekszanie;
				break;
			}
			wysw( *men, com );
			break;
		}
//komendy wspólne dla wszystkich podprogramów

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
			start = 1;//oznaka wyjścia z podprogramów
			break;
		}

	}

// funkcja obsługująca menu dwupoziomowe

	void pilot( int * const men , int com )//
	{

		if( *men == 0 )//jeśli wyszliśmy z podprogramu lub weszliśmy do podprogramu
		{
//ważne opcje przy wchodzeniu/wychodzeniu z podprogramów

			if( com > 0 && com <= liczbaPodprogramow )//jeśli komenda była z zakresu numerów podprogramów
			{
				*men = com;//przypisanie zmiennej menu nowej wartości
				wybor( *men );

				switch( *men )//można podać tu komendy które mają wykonać się podczas wchodzenia do podprogramu
				{
				case 1:
					wysw( *men, com );//niepotrzebne, gdy mają być wywoływane jakieś przyciski
					break;

				case 2:
					TCCR0 |= ( 1 << CS02 ) | ( 1 << CS00 ); // timer włączony
					czynnosc( men, 52 );
					//wysw( *men, com );//niepotrzebne, gdy mają być wywoływane jakieś przyciski
					break;

				case 3:
					czynnosc( men, 1 );
					czynnosc( men, 52 );
					//wysw( *men, com );//niepotrzebne, gdy mają być wywoływane jakieś przyciski
					break;
				}
			}
		}

		if( !start ) czynnosc( men, com );//jeśli jest się już w menu głównym, a nie idzie się właśnie do jakiegoś podprogramu



		if ( start == 1 )//jeśli było się w jakimś podprogramie i właśnie przechodzimy do podprogramu głównego
		{
			start = 0;//informacja, że zaraz będziemy "chwilę" w menu głównym
			*men = 0;//
			TCCR0 &= ~( ( 1 << CS02 ) | ( 1 << CS00 ) ); // timer 0 od wyświetlacza alfanumerycznego wyłączony
			wybor( *men );
			wysw ( *men, com );// wyświetlenie ekranu
		}



	}

// funkcja odpowiedzialna za odczytanie komend z pilota i przekazaniu ich do fukcji pilot, dopóki nie zostaną wykonane wszystkie rozkazy, nie będzie można odzczytać innego przysisku

	void zczytaj_komende( void )
	{
		if( Ir_key_press_flag )
		{
			if( !address )
			{
				TCCR1B &= ~( ( 1 << CS12 ) | ( 1 << CS11 ) | ( 1 << CS10 ) ); //wyłączenie Timera1 (prescaler na zero)
				pilot( &menu, command );//wywołanie funkcji pilot
				Ir_key_press_flag = 0;
				command = 0xff;
				address = 0xff;
#if TIMER1_PRESCALER == 1
				TCCR1B |= ( 1 << CS10 );
#endif // TIMER1_PRESCALER

#if TIMER1_PRESCALER == 8
				TCCR1B |= ( 1 << CS11 );
#endif // TIMER1_PRESCALER

#if TIMER1_PRESCALER == 64
				TCCR1B |= ( 1 << CS11 ) | ( 1 << CS10 );
#endif // TIMER1_PRESCALER

#if TIMER1_PRESCALER == 256
				TCCR1B |= ( 1 << CS12 );
#endif // TIMER1_PRESCALER

#if TIMER1_PRESCALER == 1024
				TCCR1B |= ( 1 << CS12 ) | ( 1 << CS10 );
#endif // TIMER1_PRESCALER
			}
		}
	}


//Koniec definicji metod

//Inicjalizacja


	DDRD |= ( 1 << PD7 );// PORTD7 jako wyjście do buzzera

	pwm_led_init();//inicjaliacja diod pwm
	LCD_Initalize();//inicjalizacja wyświetlacza
	ir_init();//inicjalizacja odbioru sygnału z pilota
	d_led_init();//inicjalizacja wyświetlacza alfanumerycznego
	sei();//włącza przerwania

	pilot( &menu, 0 );//rozpoczęcie programu od głównego menu

	//główna pętla programu

	while( 1 )
	{

		zczytaj_komende();
	}

	return 0;
}
