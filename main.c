//Pomiar napiêcia przetwornikiem A/C i prezentacja wyniku na LCD 2x16 HD44780

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "HD44780.h"
#include "ir_decode.h"
#define _delay_ms delay_ms_var_double
#define _delay_us delay_ms_var_double


//##############################################################################
int main( void )
{
	int t, rozmiar = 6;
	char i[rozmiar];

	void pisz( void )
	{
		for( t = 0; t < rozmiar; t++ )
		{
			LCD_WriteData( i[t] );
			_delay_ms( 50 );
		}
	}

	LCD_Initalize();
	ir_init();
	sei();

	while( 1 )
	{
		if( Ir_key_press_flag )
		{
			if( !address )
			{

				LCD_EraseAll();
				LCD_GoTo( 0, 0 );
				LCD_Int( command );
				LCD_GoTo( 6, 0 );
				LCD_Int( address );
				LCD_GoTo( 0, 1 );
				LCD_Int( toggle_bit );

				switch ( command )
				{
				case 59:
					LCD_PageUpScreen();
					LCD_PageDownScreen();
					LCD_Clear();
					break;
				case 16:
					LCD_ShiftRightScreen();
					break;
				case 17:
					LCD_ShiftLeftScreen();
					break;
				case 32:
					LCD_PageUpScreen();
					break;
				case 33:
					LCD_PageDownScreen();
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
			}

			Ir_key_press_flag = 0;
			command = 0xff;
			address = 0xff;
		}
	}
	return 0;
}


