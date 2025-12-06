#include "HD44780.h"
//-------------------------------------------------------------------------------------------------
// Wyświetlacz alfanumeryczny ze sterownikiem HD44780
// Sterowanie w trybie 4-bitowym z odczytem flagi zajêtoœci
// z dowolnym przypisaniem sygna³ów steruj¹cych
// Plik : HD44780.c
// Mikrokontroler : Atmel AVR
// Kompilator : avr-gcc
//-------------------------------------------------------------------------------------------------

void LCD_position_decrease(void)
{
    if(LCD_position == 0) LCD_position = LCD_CHARSPERLINE - 1;
    else --LCD_position;
}

void LCD_position_increase(void)
{
    if(LCD_position == LCD_CHARSPERLINE - 1) LCD_position = 0;
    else ++LCD_position;
}

void LCD_set_appropiate_position(uint8_t position)
{
    uint8_t rozmiar = LCD_CHARSPERLINE;
    LCD_Home();
    if(position < rozmiar / 2) LCD_MoveLeft ( 0, position, 1 );
    else LCD_MoveRight(0,  rozmiar - position, 1);
}

//-------------------------------------------------------------------------------------------------
//
// Funkcja wystawiaj¹ca półbajt na magistralę danych
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
// Funkcja wystawiaj¹ca półbajt na magistralę danych
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
#endif // USE_RW
//-------------------------------------------------------------------------------------------------
//
// Funkcja zapisu bajtu do wyświetacza (bez rozróżnienia instrukcja/dane).
//
//-------------------------------------------------------------------------------------------------
void _LCD_Write( unsigned char dataToWrite )
{
#if USE_RW == 1
    LCD_DB4_DIR |= LCD_DB4;
    LCD_DB5_DIR |= LCD_DB5;
    LCD_DB6_DIR |= LCD_DB6;
    LCD_DB7_DIR |= LCD_DB7;
    LCD_RW_PORT &= ~LCD_RW;
#endif // USE_RW
    LCD_E_PORT |= LCD_E;
    _LCD_OutNibble( dataToWrite >> 4 );
    LCD_E_PORT &= ~LCD_E;
    LCD_E_PORT |= LCD_E;
    _LCD_OutNibble( dataToWrite );
    LCD_E_PORT &= ~LCD_E;
#if USE_RW == 1
    while( LCD_ReadStatus() & HD44780_DDRAM_SET );
#else
    delay_us_var( 50 );
#endif // USE_RW
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
#endif // USE_RW
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
#endif // USE_RW
//-------------------------------------------------------------------------------------------------
//
// Funkcja zapisu danych do pamięci wyœwietlacza
//
//-------------------------------------------------------------------------------------------------
void LCD_WriteData( unsigned char dataToWrite )
{
    LCD_RS_PORT |= LCD_RS;
    _LCD_Write( dataToWrite );
}
//-------------------------------------------------------------------------------------------------
//
// Funkcja odczytu danych z pamięci wyświetlacza
//
//-------------------------------------------------------------------------------------------------
#if USE_RW == 1
unsigned char LCD_ReadData( void )
{
    LCD_RS_PORT |= LCD_RS;
    return _LCD_Read();
}
#endif // USE_RW
//-------------------------------------------------------------------------------------------------
//
// Funkcja wyświetlenia napisu na wyswietlaczu
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
// Funkcja czyszczenia ekranu wyświetlacza.
//
//-------------------------------------------------------------------------------------------------
void LCD_Clear( void )
{
    LCD_WriteCommand( HD44780_CLEAR );
#if USE_RW == 0
    _delay_ms( 2 );
#endif // USE_RW
    LCD_set_appropiate_position(LCD_position);
}
//-------------------------------------------------------------------------------------------------
//
// Funkcja przywrócenia pocz¹tkowych wspó³rzêdnych wyœwietlacza.
//
//-------------------------------------------------------------------------------------------------
void LCD_Home( void )
{
    LCD_WriteCommand( HD44780_HOME );
#if USE_RW == 0
    _delay_ms( 2 );
#endif // USE_RW
    LCD_position = 0;
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
#endif // USE_RW
    _delay_ms( 15 ); // oczekiwanie na ustalibizowanie się napiecia zasilajacego
    LCD_RS_PORT &= ~LCD_RS; // wyzerowanie linii RS
    LCD_E_PORT &= ~LCD_E;  // wyzerowanie linii E
#if USE_RW == 1
    LCD_RW_PORT &= ~LCD_RW;
#endif // USE_RW
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
#endif // USE_RW
    LCD_WriteCommand( HD44780_ENTRY_MODE | HD44780_EM_SHIFT_CURSOR | HD44780_EM_INCREMENT ); // inkrementaja adresu i przesuwanie kursora
    LCD_WriteCommand( HD44780_DISPLAY_ONOFF | HD44780_DISPLAY_ON | HD44780_CURSOR_OFF | HD44780_CURSOR_NOBLINK ); // w³¹cz LCD, bez kursora i mrugania

#if USE_LCD_Backlight == 1
    LCD_BACKLIGHT_DIR	|= LCD_BACKLIGHT;
    LCD_BacklightOn();
#endif
    LCD_position = 0;
}

#if USE_LCD_Backlight == 1
//-------------------------------------------------------------------------------------------------
//
// Funkcja włączenia podświetlenia wyświetlacza
//
//-------------------------------------------------------------------------------------------------
void LCD_BacklightOn(void)
{
    LCD_BACKLIGHT_PORT |= LCD_BACKLIGHT;
}
//-------------------------------------------------------------------------------------------------
//
// Funkcja wyłączenia podświetlenia wyświetlacza
//
//-------------------------------------------------------------------------------------------------
void LCD_BacklightOff(void)
{
    LCD_BACKLIGHT_PORT &= ~LCD_BACKLIGHT;
}
#endif // USE_LCD_Backlight
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
#endif // USE_LCD_Int

//-------------------------------------------------------------------------------------------------
//
// Funkcja wyświetlenia liczby (pobiera liczbę zmiennoprzecinkową i wyświetla w systemie dziesiętnym z dokładnością do podajego miejsca po pawej stronie przecinka - maksymalna ilość miejsc to cztery)
//
//-------------------------------------------------------------------------------------------------


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
#endif // USE_LCD_Hex
//-------------------------------------------------------------------------------------------------
//
// Efekt przesunięcia zawartości o okreœlonej częstotliwoœci kroku oraz liczbie kroków
//
//-------------------------------------------------------------------------------------------------
#if USE_LCD_MoveRight == 1
void LCD_MoveRight ( unsigned int freq, unsigned int step, unsigned int way )
{
    int temp;
    for ( temp = 0; temp < step; ++temp )
    {
        if ( way )
        {
            LCD_WriteCommand( HD44780_DISPLAY_CURSOR_SHIFT | HD44780_SHIFT_DISPLAY | HD44780_SHIFT_RIGHT );
            LCD_position_decrease();
        }
        else LCD_WriteCommand( HD44780_DISPLAY_CURSOR_SHIFT | HD44780_SHIFT_CURSOR | HD44780_SHIFT_RIGHT );
        if ( freq ) delay_ms_var( freq );
    }
}
#endif // USE_LCD_MoveRight
//-------------------------------------------------------------------------------------------------
//
// Efekt przesuniêcia zawartości o cały ekran w prawo
//
//-------------------------------------------------------------------------------------------------
#if USE_LCD_MoveLeft == 1
void LCD_MoveLeft ( unsigned int freq, unsigned int step, unsigned int way )
{
    int temp;
    for ( temp = 0; temp < step; ++temp )
    {
        if ( way )
        {
            LCD_WriteCommand( HD44780_DISPLAY_CURSOR_SHIFT | HD44780_SHIFT_DISPLAY | HD44780_SHIFT_LEFT );
            LCD_position_increase();
        }
        else LCD_WriteCommand( HD44780_DISPLAY_CURSOR_SHIFT | HD44780_SHIFT_CURSOR | HD44780_SHIFT_LEFT );
        if ( freq ) delay_ms_var( freq );
    }
}
#endif // USE_LCD_MoveLeft
//-------------------------------------------------------------------------------------------------
//
// Różne opcje wyświelania
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
#endif // USE_LCD_Displaying
//-------------------------------------------------------------------------------------------------
//
// Koniec pliku HD44780.c
//
//-------------------------------------------------------------------------------------------------
