#include "HD44780.h"
#include <stdlib.h>
//-------------------------------------------------------------------------------------------------
// Wyœwietlacz alfanumeryczny ze sterownikiem HD44780
// Sterowanie w trybie 4-bitowym z odczytem flagi zajêtoœci
// z dowolnym przypisaniem sygna³ów steruj¹cych
// Plik : HD44780.c
// Mikrokontroler : Atmel AVR
// Kompilator : avr-gcc
// Autorzy : Rados³aw Kwiecieñ & Dariusz Makarewicz
//-------------------------------------------------------------------------------------------------
void delay_ms_var( uint16_t count )
{
	while( count-- )
	{
		_delay_ms( 1 );

	}
}

void delay_us_var( uint16_t count )
{
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
			// wait 1/10 ms
			_delay_loop_2( ( ( F_CPU ) / 4e3 ) / 10 );
			__ticks --;
		}
		return;
	}
	else
		__ticks = ( uint16_t )__tmp;
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
//-------------------------------------------------------------------------------------------------
//
// Funkcja wystawiaj¹ca pó³bajt na magistralê danych
//
//-------------------------------------------------------------------------------------------------
void _LCD_OutNibble( unsigned char nibbleToWrite )
{

	if( nibbleToWrite & 0x01 )
		LCD_DB4_PORT |= LCD_DB4;
	else
		LCD_DB4_PORT  &= ~LCD_DB4;

	if( nibbleToWrite & 0x02 )
		LCD_DB5_PORT |= LCD_DB5;
	else
		LCD_DB5_PORT  &= ~LCD_DB5;

	if( nibbleToWrite & 0x04 )
		LCD_DB6_PORT |= LCD_DB6;
	else
		LCD_DB6_PORT  &= ~LCD_DB6;

	if( nibbleToWrite & 0x08 )
		LCD_DB7_PORT |= LCD_DB7;
	else
		LCD_DB7_PORT  &= ~LCD_DB7;
}
//-------------------------------------------------------------------------------------------------
//
// Funkcja wystawiaj¹ca pó³bajt na magistralê danych
//
//-------------------------------------------------------------------------------------------------
#if USE_RW == 1
unsigned char _LCD_InNibble( void )
{
	unsigned char tmp = 0;

	if( LCD_DB4_PIN & LCD_DB4 )
		tmp |= ( 1 << 0 );
	if( LCD_DB5_PIN & LCD_DB5 )
		tmp |= ( 1 << 1 );
	if( LCD_DB6_PIN & LCD_DB6 )
		tmp |= ( 1 << 2 );
	if( LCD_DB7_PIN & LCD_DB7 )
		tmp |= ( 1 << 3 );
	return tmp;
}
#endif
//-------------------------------------------------------------------------------------------------
//
// Funkcja zapisu bajtu do wyœwietacza (bez rozró¿nienia instrukcja/dane).
//
//-------------------------------------------------------------------------------------------------
void _LCD_Write( unsigned char dataToWrite )
{
#if ( USE_RW == 1 ) || ( BUFFERING == 1 )
	LCD_DB4_DIR |= LCD_DB4;
	LCD_DB5_DIR |= LCD_DB5;
	LCD_DB6_DIR |= LCD_DB6;
	LCD_DB7_DIR |= LCD_DB7;
	LCD_RW_PORT &= ~LCD_RW;
#endif
	LCD_E_PORT |= LCD_E;
	_LCD_OutNibble( dataToWrite >> 4 );
	LCD_E_PORT &= ~LCD_E;
	LCD_E_PORT |= LCD_E;
	_LCD_OutNibble( dataToWrite );
	LCD_E_PORT &= ~LCD_E;
#if ( USE_RW == 1 ) || ( BUFFERING == 1 )
	while( LCD_ReadStatus() & HD44780_DDRAM_SET );
#else
	_delay_us( 50 );
#endif
}

//-------------------------------------------------------------------------------------------------
//
// Funkcja odczytu bajtu z wyœwietacza (bez rozró¿nienia instrukcja/dane).
//
//-------------------------------------------------------------------------------------------------
#if USE_RW == 1
unsigned char _LCD_Read( void )
{
	unsigned char tmp = 0;
	LCD_DB4_DIR &= ~LCD_DB4;
	LCD_DB5_DIR &= ~LCD_DB5;
	LCD_DB6_DIR &= ~LCD_DB6;
	LCD_DB7_DIR &= ~LCD_DB7;

	LCD_RW_PORT |= LCD_RW;
	LCD_E_PORT |= LCD_E;
	tmp |= ( _LCD_InNibble() << 4 );
	LCD_E_PORT &= ~LCD_E;
	LCD_E_PORT |= LCD_E;
	tmp |= _LCD_InNibble();
	LCD_E_PORT &= ~LCD_E;
	_delay_us( 50 );
	return tmp;
}
#endif

//-------------------------------------------------------------------------------------------------
//
// Funkcja zapisu rozkazu do wyœwietlacza
//
//-------------------------------------------------------------------------------------------------
void LCD_WriteCommand( unsigned char commandToWrite )
{
	LCD_RS_PORT &= ~LCD_RS;
	_LCD_Write( commandToWrite );
}

//-------------------------------------------------------------------------------------------------
//
// Funkcja odczytu bajtu statusowego
//
//-------------------------------------------------------------------------------------------------
#if USE_RW == 1
unsigned char LCD_ReadStatus( void )
{
	LCD_RS_PORT &= ~LCD_RS;
	return _LCD_Read();
}
#endif
//-------------------------------------------------------------------------------------------------
//
// Funkcja zapisu danych do pamiêci wyœwietlacza
//
//-------------------------------------------------------------------------------------------------
void LCD_WriteData( unsigned char dataToWrite )
{
	LCD_RS_PORT |= LCD_RS;
	_LCD_Write( dataToWrite );
}
//-------------------------------------------------------------------------------------------------
//
// Funkcja odczytu danych z pamiêci wyœwietlacza
//
//-------------------------------------------------------------------------------------------------
#if USE_RW == 1
unsigned char LCD_ReadData( void )
{
	LCD_RS_PORT |= LCD_RS;
	return _LCD_Read();
}
#endif
//-------------------------------------------------------------------------------------------------
//
// Funkcja wyœwietlenia napisu na wyswietlaczu
//
//-------------------------------------------------------------------------------------------------
void LCD_WriteText( char * text )
{
	while( *text )
		LCD_WriteData( *text++ );
}
//-------------------------------------------------------------------------------------------------
//
// Funkcja ustawienia wspó³rzêdnych ekranowych
//
//-------------------------------------------------------------------------------------------------
void LCD_GoTo( unsigned char x, unsigned char y )
{
	LCD_WriteCommand( HD44780_DDRAM_SET | ( x + ( HD44780_CGRAM_SET * y ) ) );
}
//-------------------------------------------------------------------------------------------------
//
// Funkcja czyszczenia ekranu wyœwietlacza.
//
//-------------------------------------------------------------------------------------------------
void LCD_Clear( void )
{
	LCD_WriteCommand( HD44780_CLEAR );
	_delay_ms( 2 );
}
//-------------------------------------------------------------------------------------------------
//
// Funkcja przywrócenia pocz¹tkowych wspó³rzêdnych wyœwietlacza.
//
//-------------------------------------------------------------------------------------------------
void LCD_Home( void )
{
	LCD_WriteCommand( HD44780_HOME );
	_delay_ms( 2 );
}
//-------------------------------------------------------------------------------------------------
//
// Procedura inicjalizacji kontrolera HD44780.
//
//-------------------------------------------------------------------------------------------------
void LCD_Initalize( void )
{
	unsigned char i;
	LCD_DB4_DIR |= LCD_DB4; // Konfiguracja kierunku pracy wyprowadzeñ
	LCD_DB5_DIR |= LCD_DB5; //
	LCD_DB6_DIR |= LCD_DB6; //
	LCD_DB7_DIR |= LCD_DB7; //
	LCD_E_DIR 	|= LCD_E;   //
	LCD_RS_DIR 	|= LCD_RS;  //
#if USE_RW == 1
	LCD_RW_DIR 	|= LCD_RW;  //
#endif
	_delay_ms( 15 ); // oczekiwanie na ustalibizowanie się napiecia zasilajacego
	LCD_RS_PORT &= ~LCD_RS; // wyzerowanie linii RS
	LCD_E_PORT &= ~LCD_E;  // wyzerowanie linii E
#if USE_RW == 1
	LCD_RW_PORT &= ~LCD_RW;
#endif
	for( i = 0; i < 3; ++i ) // trzykrotne powtórzenie bloku instrukcji
	{
		LCD_E_PORT |= LCD_E; //  E = 1
		_LCD_OutNibble( 0x03 ); // tryb 8-bitowy
		LCD_E_PORT &= ~LCD_E; // E = 0
		_delay_ms( 5 ); // czekaj 5ms
	}

	LCD_E_PORT |= LCD_E; // E = 1
	_LCD_OutNibble( 0x02 ); // tryb 4-bitowy
	LCD_E_PORT &= ~LCD_E; // E = 0

	_delay_ms( 1 ); // czekaj 1ms
	LCD_WriteCommand( HD44780_FUNCTION_SET | HD44780_FONT5x7 | HD44780_TWO_LINE | HD44780_4_BIT ); // interfejs 4-bity, 2-linie, znak 5x7
	LCD_WriteCommand( HD44780_DISPLAY_ONOFF | HD44780_DISPLAY_OFF ); // wy³¹czenie wyswietlacza
	LCD_WriteCommand( HD44780_CLEAR ); // czyszczenie zawartosæi pamieci DDRAM
#if USE_RW == 0
	_delay_ms( 2 );
#endif
	LCD_WriteCommand( HD44780_ENTRY_MODE | HD44780_EM_SHIFT_CURSOR | HD44780_EM_INCREMENT ); // inkrementaja adresu i przesuwanie kursora
	LCD_WriteCommand( HD44780_DISPLAY_ONOFF | HD44780_DISPLAY_ON | HD44780_CURSOR_OFF | HD44780_CURSOR_NOBLINK ); // w³¹cz LCD, bez kursora i mrugania
}
//-------------------------------------------------------------------------------------------------
//
// Funkcja wyświetlenia liczby (pobiera liczbę całkowitą i wyświetla w systemie dziesiętnym)
//
//-------------------------------------------------------------------------------------------------
#if USE_LCD_Int == 1
void LCD_Int( int value )
{
	char bufor[17];
	LCD_WriteText( itoa( value, bufor, 10 ) );
}
#endif
//-------------------------------------------------------------------------------------------------
//
// Funkcja wyświetlenia liczby (pobiera liczbę całkowitą i wyświetla w systemie szesnastkowym)
//
//-------------------------------------------------------------------------------------------------
#if USE_LCD_Hex == 1
void LCD_Hex( int value )
{
	char bufor[17];
	LCD_WriteText( itoa( value, bufor, 16 ) );
}
#endif
//-------------------------------------------------------------------------------------------------
//
// Pomocnicze zmienne
//
//-------------------------------------------------------------------------------------------------

const int czterdziesci = 40;

//-------------------------------------------------------------------------------------------------
//
// Efekt przesuniêcia zawartoœci o okreœlonej czêstotliwoœci kroku oraz liczbie kroków
//
//-------------------------------------------------------------------------------------------------
#if USE_LCD_MoveRight == 1
void LCD_MoveRight ( unsigned int freq, unsigned int step, unsigned int way )
{
	int temp;
	for ( temp = 0; temp < step; ++temp )
	{
		if ( way )LCD_WriteCommand( HD44780_DISPLAY_CURSOR_SHIFT | HD44780_SHIFT_DISPLAY | HD44780_SHIFT_RIGHT );
		else LCD_WriteCommand( HD44780_DISPLAY_CURSOR_SHIFT | HD44780_SHIFT_CURSOR | HD44780_SHIFT_RIGHT );
		if ( freq ) delay_ms_var( freq );
	}
}
#endif
//-------------------------------------------------------------------------------------------------
//
// Efekt przesuniêcia zawartoœci o ca³y ekran w prawo
//
//-------------------------------------------------------------------------------------------------
#if USE_LCD_MoveLeft == 1
void LCD_MoveLeft ( unsigned int freq, unsigned int step, unsigned int way )
{
	int temp;
	for ( temp = 0; temp < step; ++temp )
	{
		if ( way )LCD_WriteCommand( HD44780_DISPLAY_CURSOR_SHIFT | HD44780_SHIFT_DISPLAY | HD44780_SHIFT_LEFT );
		else LCD_WriteCommand( HD44780_DISPLAY_CURSOR_SHIFT | HD44780_SHIFT_CURSOR | HD44780_SHIFT_LEFT );
		if ( freq ) delay_ms_var( freq );
	}
}
#endif
//-------------------------------------------------------------------------------------------------
//
// Czyszczenie zawartoœci okna
//
//-------------------------------------------------------------------------------------------------
#if USE_LCD_Erase == 1
void LCD_Erase ( unsigned int row )
{
	int temp;
	if ( row == 0 || row == 1 )
	{
		LCD_GoTo( 0, 0 );
		for ( temp = 0; temp < czterdziesci; ++temp )
		{
			LCD_WriteData( 32 );//znak "spacji"
		}
	}

	if ( row == 0 || row == 2 )
	{
		LCD_GoTo( 0, 1 );
		for ( temp = 0; temp < czterdziesci; ++temp )
		{
			LCD_WriteData( 32 );//znak "spacji"
		}
	}
}
#endif
//-------------------------------------------------------------------------------------------------
//
// Ró¿ne opcje wyœwielania
//
//-------------------------------------------------------------------------------------------------
#if USE_LCD_Displaying == 1
void LCD_Displaying ( unsigned int option )
{
	switch ( option )
	{
	case 1:
		LCD_WriteCommand( HD44780_DISPLAY_ONOFF | HD44780_DISPLAY_ON );
		break;
	case 2:
		LCD_WriteCommand( HD44780_DISPLAY_ONOFF | HD44780_DISPLAY_OFF );
		break;
	case 3:
		LCD_WriteCommand( HD44780_DISPLAY_ONOFF | HD44780_DISPLAY_ON | HD44780_CURSOR_ON );
		break;
	case 4:
		LCD_WriteCommand( HD44780_DISPLAY_ONOFF | HD44780_DISPLAY_ON | HD44780_CURSOR_BLINK );
		break;
	case 5:
		LCD_WriteCommand( HD44780_DISPLAY_ONOFF | HD44780_DISPLAY_ON | HD44780_CURSOR_ON | HD44780_CURSOR_BLINK );
		break;
	}
}
#endif

#if BUFFERING == 1

unsigned char LCDBuffer[LCD_LINES][LCD_CHARSPERLINE];//tablica dwuwymiarowa stanowiąca bufor (znaki do wyświetlania)
unsigned char LCDNeedUpdate[LCD_LINES];//tablica wielkości ilości linii wyświetlacza (stwierdzenie, czy dana linia wymaga odświeżenia)
signed 	 char LCDCharIndex[LCD_LINES];//w tej tablicy przechowywana jest pozycja w danej linii wyświetlacza
unsigned char LCDLineIndex = 0;//zmienna pomocnicza w iteracjach
unsigned char LCDLineAddress[2] = {0x00, 0x40/*, 0x14, 0x54*/};//adresy (w pamięci DDRAM wyświetlacza) poszczególnych linii

//-------------------------------------------------------------------------------------------------
// Wywoływane funkcje zewnętrzne :
//		LCD_NotBusy - zwraca 0 jeśli wyświetlacz jest zajęty, w przeciwnym razie zwraca 1
//		LCD_JustWriteCommand - zapisuje rozkaz do sterownika wyświetlacza (bezzwłocznie)
//		LCD_JustWriteData	 - zapisuje dane do sterownika wyświetlacza (bezzwłocznie)
//=================================================================================================

void LCDClearBuffer( void )
{
	int i, j;
	for( j = 0; j < LCD_LINES; ++j )
	{
		LCDCharIndex[j] = -1;
		for( i = 0; i < LCD_CHARSPERLINE; ++i )
		{
			LCDBuffer[j][i] = 32;//wypełnienie bufora znakami "spacji"
		}
	}
}

//=================================================================================================

void LCDWriteToBuffer( unsigned char x, unsigned char y, char * str )
{
	int cnt = 0;//zmienna pomocnicza
	while( *str != 0 && cnt + x < LCD_CHARSPERLINE)//w pętli o ilości iteracji równej długości łąńcucha, jeśli łańcuch jest zbyt długi, to się nie prześle
	{
		LCDBuffer[y][x + cnt] = *str;//do tablicy bufora o określonej linii i od określonego miejsca zapisywane zostają dane
		++str;
		++cnt;
	}
	LCDNeedUpdate[y] = 1;
	//return cnt;
}
//=================================================================================================
// Należy wywoływać cykliczne w pętli głównej
//=================================================================================================

void LCDUpdateTask( void )
{
	if( LCDNeedUpdate[LCDLineIndex] )//jeśli któraś linia potrzebuje "aktualizacji"
	{
		if( LCD_ReadStatus() != HD44780_DDRAM_SET )// jeśli wyświetlacz nie jest zajęty
		{
			if( LCDCharIndex[LCDLineIndex] == -1 )//jeśli jest to pozycja pierwsza w danej linii
			{
				LCD_WriteCommand( HD44780_DDRAM_SET | LCDLineAddress[LCDLineIndex] );//ustawienie kursora na początku
				LCDCharIndex[LCDLineIndex]++;//zwiększenie kursora w tablicy z położeniem o jeden
				return;//wyjście z funkcji
			}
			LCD_WriteData( LCDBuffer[LCDLineIndex][LCDCharIndex[LCDLineIndex]++] );//zapis na wyświetlaczu pojedynczego znaku
			if( LCDCharIndex[LCDLineIndex] == ( LCD_CHARSPERLINE ) )//jeśli w tablicy położenia jest już ostatni indeks
			{
				LCDCharIndex[LCDLineIndex] 		= -1;//położenie w danej linii zostanie przywrócone na początek
				LCDNeedUpdate[LCDLineIndex] 	= 0;//dana linia nie potrzebuje już aktualizacji
			}
		}
		return;
	}
	++LCDLineIndex;//inkrementacja linii
	if( LCDLineIndex == LCD_LINES )//jeśli indeks przyjmuje wartoś spoza zakresu, należy przypisać mu wartość zero
		LCDLineIndex = 0;
}

#endif
//-------------------------------------------------------------------------------------------------
//
// Koniec pliku HD44780.c
//
//-------------------------------------------------------------------------------------------------
