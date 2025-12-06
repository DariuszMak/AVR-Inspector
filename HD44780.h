//-------------------------------------------------------------------------------------------------
// Wyświetlacz alfanumeryczny ze sterownikiem HD44780
// Sterowanie w trybie 4-bitowym z odczytem flagi zajętości
// z dowolnym przypisaniem sygnałów sterujących
// Plik : HD44780.h
// Mikrokontroler : Atmel AVR
// Kompilator : avr-gcc
// Autorzy : Rados³aw Kwiecieñ & Dariusz Makarewicz
//-------------------------------------------------------------------------------------------------
#ifndef HD44780_H_
#define HD44789_H_

#include <avr/io.h>
#include <util/delay.h>

//-------------------------------------------------------------------------------------------------
//
// Konfiguracja sygnałów sterujących wyświetlaczem.
// Można zmienić stosownie do potrzeb.
//
//-------------------------------------------------------------------------------------------------
#define USE_RW 1 // Tryb używania pinu RW (odczyt flagi zajętości i w ogóle odczyt) 0 - bez odczytu (wtedy MUSI ten pin być podpięty na stałe do VCC, albo: LCD_RW_DIR |= LCD_RW; ) / 1 - z odczytem

#define BUFFERING 1 // włączanie trybu buforowania danych na wyświetlaczu 0 - normalny tryb (istnieje konieczność zakomentowania niektórych funkji w programie), 1 - tryb buforowania włączony

#define 	  LCD_LINES				2// liczba wierszy bufora wyświetlacza
#define 	  LCD_CHARSPERLINE		40// liczba znaków w pojedynczej linii bufora wyświetlacza

#define USE_LCD_Int 1
#define USE_LCD_Hex 1
#define USE_LCD_MoveRight 1
#define USE_LCD_MoveLeft 1
#define USE_LCD_Erase 1
#define USE_LCD_Displaying 1


#if USE_RW == 1
#define LCD_RW_DIR		DDRA
#define LCD_RW_PORT		PORTA
#define LCD_RW_PIN		PINA
#define LCD_RW			(1 << PA1)
#endif


#define LCD_RS_DIR		DDRA
#define LCD_RS_PORT 	PORTA
#define LCD_RS_PIN		PINA
#define LCD_RS			(1 << PA2)

#define LCD_E_DIR		DDRA
#define LCD_E_PORT		PORTA
#define LCD_E_PIN		PINA
#define LCD_E			(1 << PA3)

#define LCD_DB4_DIR		DDRA
#define LCD_DB4_PORT	PORTA
#define LCD_DB4_PIN		PINA
#define LCD_DB4			(1 << PA4)

#define LCD_DB5_DIR		DDRA
#define LCD_DB5_PORT	PORTA
#define LCD_DB5_PIN		PINA
#define LCD_DB5			(1 << PA5)

#define LCD_DB6_DIR		DDRA
#define LCD_DB6_PORT	PORTA
#define LCD_DB6_PIN		PINA
#define LCD_DB6			(1 << PA6)

#define LCD_DB7_DIR		DDRA
#define LCD_DB7_PORT	PORTA
#define LCD_DB7_PIN		PINA
#define LCD_DB7			(1 << PA7)

//-------------------------------------------------------------------------------------------------
//
// Instrukcje kontrolera Hitachi HD44780
//
//-------------------------------------------------------------------------------------------------

#define HD44780_CLEAR					0x01

#define HD44780_HOME					0x02

#define HD44780_ENTRY_MODE				0x04
#define HD44780_EM_SHIFT_CURSOR		0 // tryb z automatycznym przesuwaniem kursora
#define HD44780_EM_SHIFT_DISPLAY	1 // tryb z automatycznym przesuwaniem ekranu (tak jakby shiftować za każda wyświetlnaną literą, bardzo dezorientujące umieszczanie kursora)
#define HD44780_EM_DECREMENT		0 // tryb ze zmiejszaniem adresu
#define HD44780_EM_INCREMENT		2 // tryb za zwiększaniem adresu

#define HD44780_DISPLAY_ONOFF			0x08
#define HD44780_DISPLAY_OFF			0
#define HD44780_DISPLAY_ON			4
#define HD44780_CURSOR_OFF			0
#define HD44780_CURSOR_ON			2
#define HD44780_CURSOR_NOBLINK		0
#define HD44780_CURSOR_BLINK		1

#define HD44780_DISPLAY_CURSOR_SHIFT	0x10
#define HD44780_SHIFT_CURSOR		0
#define HD44780_SHIFT_DISPLAY		8
#define HD44780_SHIFT_LEFT			0
#define HD44780_SHIFT_RIGHT			4

#define HD44780_FUNCTION_SET    		0x20
#define HD44780_FONT5x7				0
#define HD44780_FONT5x10			4
#define HD44780_ONE_LINE			0
#define HD44780_TWO_LINE			8
#define HD44780_4_BIT				0
#define HD44780_8_BIT				16

#define HD44780_CGRAM_SET	        	0x40

#define HD44780_DDRAM_SET		    	0x80

//-------------------------------------------------------------------------------------------------
//
// Deklaracje funkcji
//
//-------------------------------------------------------------------------------------------------


void delay_ms_var( uint16_t ); // czekaj określoną ilość milisekund
void delay_us_var( uint16_t ); // czekaj określoną ilość mikrosekund
void delay_ms_var_double( double ); // czekaj określoną ilość milisekund
void delay_us_var_double( double ); // czekaj określoną ilość mikrosekund

void _LCD_OutNibble( unsigned char );
#if USE_RW == 1
unsigned char _LCD_InNibble( void );
#endif
void _LCD_Write( unsigned char );
#if BUFFERING == 1
int LCDWriteToBuffer( unsigned char, unsigned char, char * );
void LCDClearBuffer( void );
void LCDUpdateTask( void );
#endif
#if USE_RW == 1
unsigned char _LCD_Read( void );
#endif
void LCD_WriteCommand( unsigned char );
#if USE_RW == 1
unsigned char LCD_ReadStatus( void );
#endif
void LCD_WriteData( unsigned char ); // odczytywanie danych po kolei w zależności od pozycji kursora
#if USE_RW == 1
unsigned char LCD_ReadData( void ); // zapisywanie danych po kolei w zależności od pozycji kursora
#endif

void LCD_Initalize( void ); // Inicjalizacja wyświetlacza

void LCD_WriteText( char * );//Wypisanie tekstu (łańcuch znaków)
void LCD_GoTo( unsigned char, unsigned char ); // pozycja X, pozycja Y
void LCD_Clear( void ); // czyści wszystko sprzętowo
void LCD_Home( void ); // sprętowa funkcje powrotu ns początek (ekran i kursor)

#if USE_LCD_Int == 1
void LCD_Int( int ); // wyświetla liczby (pobiera liczbę całkowitą i wyświetla w systemie dziesiętnym)
#endif

#if USE_LCD_Hex == 1
void LCD_Hex( int ); // wyświetla liczby (pobiera liczbę całkowitą i wyświetla w systemie szesnastkowym)
#endif

#if USE_LCD_MoveRight == 1
void LCD_MoveRight( unsigned int, unsigned int, unsigned int ); // częstotliwość kroku, ilość kroków, 0 - kursor / 1 - ekran
#define LCD_PageUpScreen() LCD_MoveRight(20,15,1) // przesunięcie o cały ekran w prawo
#define LCD_ShiftRightScreen() LCD_MoveRight(0,1,1) // jeden krok ekranu w prawo
#define LCD_ShiftRightCursor() LCD_MoveRight(0,1,0) // jeden krok kursora w prawo
#endif

#if USE_LCD_MoveLeft == 1
void LCD_MoveLeft( unsigned int, unsigned int, unsigned int ); // częstotliwość kroku, ilość kroków, 0 - kursor / 1 - ekran
#define LCD_ShiftLeftCursor() LCD_MoveLeft(0,1,0) // jeden krok kursora w lewo
#define LCD_ShiftLeftScreen() LCD_MoveLeft(0,1,1) // jeden krok ekranu w lewo
#define LCD_PageDownScreen() LCD_MoveLeft(20,15,1) // przesunięcie o cały ekran w lewo
#endif

#if USE_LCD_Erase == 1
void LCD_Erase( unsigned int ); // uzuoełnianie spacjami wyświetlacza 0 - dwa wiersze, 1 - górny wiersz, 2 - dolny wiersz
#define LCD_EraseAll() LCD_Erase(0) // uzuoełnianie spacjami wyświetlacza - dwa wiersze
#define LCD_EraseUp() LCD_Erase(1) // uzuoełnianie spacjami wyświetlacza 1 - górny wiersz,
#define LCD_EraseDown() LCD_Erase(2) // uzuoełnianie spacjami wyświetlacza 2 - dolny wiersz
#endif

#if USE_LCD_Displaying == 1
void LCD_Displaying( unsigned int );
#define LCD_ScreenOn() LCD_Displaying(1) // zwykły tryb pracy wyświetlacza bez kursora
#define LCD_ScreenOff() LCD_Displaying(2) // wyłączenie wyświetlacza (nic nie wyświetla, ale wciąż pracuje)
#define LCD_Cursor() LCD_Displaying(3) // tryb pracy z kursorem
#define LCD_Blink() LCD_Displaying(4) // tryb pracy z migającym prostokątem
#define LCD_CursorBlink() LCD_Displaying(5) // tryb pracy z kursorem i z migającym prostokątem
#endif
//-------------------------------------------------------------------------------------------------
//
// Koniec pliku HD44780.h
//
//-------------------------------------------------------------------------------------------------
#endif // HD44780_h_
