//-------------------------------------------------------------------------------------------------
// Wyúwietlacz alfanumeryczny ze sterownikiem HD44780
// Sterowanie w trybie 4-bitowym z odczytem flagi zajÍtoúci
// z dowolnym przypisaniem sygna≥Ûw sterujπcych
// Plik : HD44780.h
// Mikrokontroler : Atmel AVR
// Kompilator : avr-gcc
// Autor : Rados≥aw KwiecieÒ
// èrÛd≥o : http://radzio.dxp.pl/hd44780/
// Data : 24.03.2007
//-------------------------------------------------------------------------------------------------

#include <avr/io.h>
#include <util/delay.h>

//-------------------------------------------------------------------------------------------------
//
// Konfiguracja sygna≥Ûw sterujπcych wyúwietlaczem.
// Moøna zmieniÊ stosownie do potrzeb.
//
//-------------------------------------------------------------------------------------------------
#define LCD_RS_DIR		DDRD
#define LCD_RS_PORT 	PORTD
#define LCD_RS_PIN		PIND
#define LCD_RS			(1 << PD2)

#define LCD_RW_DIR		DDRD
#define LCD_RW_PORT		PORTD
#define LCD_RW_PIN		PIND
#define LCD_RW			(1 << PD1)

#define LCD_E_DIR		DDRD
#define LCD_E_PORT		PORTD
#define LCD_E_PIN		PIND
#define LCD_E			(1 << PD3)

#define LCD_DB4_DIR		DDRD
#define LCD_DB4_PORT	PORTD
#define LCD_DB4_PIN		PIND
#define LCD_DB4			(1 << PD4)

#define LCD_DB5_DIR		DDRD
#define LCD_DB5_PORT	PORTD
#define LCD_DB5_PIN		PIND
#define LCD_DB5			(1 << PD5)

#define LCD_DB6_DIR		DDRD
#define LCD_DB6_PORT	PORTD
#define LCD_DB6_PIN		PIND
#define LCD_DB6			(1 << PD6)

#define LCD_DB7_DIR		DDRD
#define LCD_DB7_PORT	PORTD
#define LCD_DB7_PIN		PIND
#define LCD_DB7			(1 << PD7)

//-------------------------------------------------------------------------------------------------
//
// Instrukcje kontrolera Hitachi HD44780
//
//-------------------------------------------------------------------------------------------------

#define HD44780_CLEAR					0x01

#define HD44780_HOME					0x02

#define HD44780_ENTRY_MODE				0x04
#define HD44780_EM_SHIFT_CURSOR		0
#define HD44780_EM_SHIFT_DISPLAY	1
#define HD44780_EM_DECREMENT		0
#define HD44780_EM_INCREMENT		2

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

#define HD44780_FUNCTION_SET			0x20
#define HD44780_FONT5x7				0
#define HD44780_FONT5x10			4
#define HD44780_ONE_LINE			0
#define HD44780_TWO_LINE			8
#define HD44780_4_BIT				0
#define HD44780_8_BIT				16

#define HD44780_CGRAM_SET				0x40

#define HD44780_DDRAM_SET				0x80

//-------------------------------------------------------------------------------------------------
//
// Deklaracje funkcji
//
//-------------------------------------------------------------------------------------------------

void LCD_WriteCommand(unsigned char);
unsigned char LCD_ReadStatus(void);

void LCD_WriteData(unsigned char); // odczytywanie danych po kolei w zaleønoúci od pozycji kursora
unsigned char LCD_ReadData(void); // zapisywanie danych po kolei w zaleønoúci od pozycji kursora

void LCD_WriteText(char *);
void LCD_GoTo(unsigned char, unsigned char);
void LCD_Clear(void);
void LCD_Home(void);
void LCD_Initalize(void);

void LCD_MoveRight(unsigned int, unsigned int, unsigned int); // czÍstotliwoúÊ kroku, iloúÊ krokÛw, 0 - kursor, 1 - ekran
void LCD_MoveLeft(unsigned int, unsigned int, unsigned int); // czÍstotliwoúÊ kroku, iloúÊ krokÛw, 0 - kursor, 1 - ekran
#define LCD_ShiftRightCursor() LCD_MoveRight(0,1,0) // jeden krok kursora w prawo
#define LCD_ShiftLeftCursor() LCD_MoveLeft(0,1,0) // jeden krok kursora w lewo
#define LCD_ShiftRightScreen() LCD_MoveRight(0,1,1) // jeden krok ekranu w prawo
#define LCD_ShiftLeftScreen() LCD_MoveLeft(0,1,1) // jeden krok ekranu w lewo
#define LCD_PageUpScreen() LCD_MoveRight(20,15,1) // przesuniÍcie o ca≥y ekran w prawo
#define LCD_PageDownScreen() LCD_MoveLeft(20,15,1) // przesuniÍcie o ca≥y ekran w lewo

void LCD_Erase(unsigned int); // uzuoe≥nianie spacjami wyúwietlacza 0 - dwa wiersze, 1 - gÛrny wiersz, 2 - dolny wiersz
#define LCD_EraseAll() LCD_Erase(0) // uzuoe≥nianie spacjami wyúwietlacza - dwa wiersze
#define LCD_EraseUp() LCD_Erase(1) // uzuoe≥nianie spacjami wyúwietlacza 1 - gÛrny wiersz,
#define LCD_EraseDown() LCD_Erase(2) // uzuoe≥nianie spacjami wyúwietlacza 2 - dolny wiersz

void LCD_Displaying(unsigned int);
#define LCD_ScreenOn() LCD_Displaying(1) // zwyk≥y tryb pracy wyúwietlacza bez kursora
#define LCD_ScreenOff() LCD_Displaying(2) // wy≥πczenie wyúwietlacza (nic nie wyúwietla, ale wciπø pracuje)
#define LCD_Cursor() LCD_Displaying(3) // tryb pracy z kursorem
#define LCD_Blink() LCD_Displaying(4) // tryb pracy z migajπcym prostokπtem
#define LCD_CursorBlink() LCD_Displaying(5) // tryb pracy z kursorem i z migajπcym prostokπtem
//-------------------------------------------------------------------------------------------------
//
// Koniec pliku HD44780.h
//
//-------------------------------------------------------------------------------------------------
