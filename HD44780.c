//-------------------------------------------------------------------------------------------------
// Wyœwietlacz alfanumeryczny ze sterownikiem HD44780
// Sterowanie w trybie 4-bitowym z odczytem flagi zajêtoœci
// z dowolnym przypisaniem sygna³ów steruj¹cych
// Plik : HD44780.c
// Mikrokontroler : Atmel AVR
// Kompilator : avr-gcc
// Autorzy : Rados³aw Kwiecieñ & Dariusz Makarewicz
//-------------------------------------------------------------------------------------------------

#include "HD44780.h"
//-------------------------------------------------------------------------------------------------
//
// Funkcja wystawiaj¹ca pó³bajt na magistralê danych
//
//-------------------------------------------------------------------------------------------------
void _LCD_OutNibble(unsigned char nibbleToWrite)
{

    if(nibbleToWrite & 0x01)
        LCD_DB4_PORT |= LCD_DB4;
    else
        LCD_DB4_PORT  &= ~LCD_DB4;

    if(nibbleToWrite & 0x02)
        LCD_DB5_PORT |= LCD_DB5;
    else
        LCD_DB5_PORT  &= ~LCD_DB5;

    if(nibbleToWrite & 0x04)
        LCD_DB6_PORT |= LCD_DB6;
    else
        LCD_DB6_PORT  &= ~LCD_DB6;

    if(nibbleToWrite & 0x08)
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
unsigned char _LCD_InNibble(void)
{
    unsigned char tmp = 0;

    if(LCD_DB4_PIN & LCD_DB4)
        tmp |= (1 << 0);
    if(LCD_DB5_PIN & LCD_DB5)
        tmp |= (1 << 1);
    if(LCD_DB6_PIN & LCD_DB6)
        tmp |= (1 << 2);
    if(LCD_DB7_PIN & LCD_DB7)
        tmp |= (1 << 3);
    return tmp;
}
#endif
//-------------------------------------------------------------------------------------------------
//
// Funkcja zapisu bajtu do wyœwietacza (bez rozró¿nienia instrukcja/dane).
//
//-------------------------------------------------------------------------------------------------
void _LCD_Write(unsigned char dataToWrite)
{
#if USE_RW == 1
    LCD_DB4_DIR |= LCD_DB4;
    LCD_DB5_DIR |= LCD_DB5;
    LCD_DB6_DIR |= LCD_DB6;
    LCD_DB7_DIR |= LCD_DB7;

    LCD_RW_PORT &= ~LCD_RW;
#endif
    LCD_E_PORT |= LCD_E;
    _LCD_OutNibble(dataToWrite >> 4);
    LCD_E_PORT &= ~LCD_E;
    LCD_E_PORT |= LCD_E;
    _LCD_OutNibble(dataToWrite);
    LCD_E_PORT &= ~LCD_E;
#if USE_RW == 1
    while(LCD_ReadStatus()&0x80);
#else
    _delay_us(50);
#endif
}
//-------------------------------------------------------------------------------------------------
//
// Funkcja odczytu bajtu z wyœwietacza (bez rozró¿nienia instrukcja/dane).
//
//-------------------------------------------------------------------------------------------------
#if USE_RW == 1
unsigned char _LCD_Read(void)
{
    unsigned char tmp = 0;
    LCD_DB4_DIR &= ~LCD_DB4;
    LCD_DB5_DIR &= ~LCD_DB5;
    LCD_DB6_DIR &= ~LCD_DB6;
    LCD_DB7_DIR &= ~LCD_DB7;

    LCD_RW_PORT |= LCD_RW;
    LCD_E_PORT |= LCD_E;
    tmp |= (_LCD_InNibble() << 4);
    LCD_E_PORT &= ~LCD_E;
    LCD_E_PORT |= LCD_E;
    tmp |= _LCD_InNibble();
    LCD_E_PORT &= ~LCD_E;
    return tmp;
}
#endif

//-------------------------------------------------------------------------------------------------
//
// Funkcja zapisu rozkazu do wyœwietlacza
//
//-------------------------------------------------------------------------------------------------
void LCD_WriteCommand(unsigned char commandToWrite)
{
    LCD_RS_PORT &= ~LCD_RS;
    _LCD_Write(commandToWrite);
}

//-------------------------------------------------------------------------------------------------
//
// Funkcja odczytu bajtu statusowego
//
//-------------------------------------------------------------------------------------------------
#if USE_RW == 1
unsigned char LCD_ReadStatus(void)
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
void LCD_WriteData(unsigned char dataToWrite)
{
    LCD_RS_PORT |= LCD_RS;
    _LCD_Write(dataToWrite);
}
//-------------------------------------------------------------------------------------------------
//
// Funkcja odczytu danych z pamiêci wyœwietlacza
//
//-------------------------------------------------------------------------------------------------
#if USE_RW == 1
unsigned char LCD_ReadData(void)
{
    LCD_RS_PORT |= LCD_RS;
    return _LCD_Read();
}
#endif
//-------------------------------------------------------------------------------------------------
//
// Funkcja wyœwietlenia napisu na wyswietlaczu.
//
//-------------------------------------------------------------------------------------------------
void LCD_WriteText(char * text)
{
    while(*text)
        LCD_WriteData(*text++);
}
//-------------------------------------------------------------------------------------------------
//
// Funkcja ustawienia wspó³rzêdnych ekranowych
//
//-------------------------------------------------------------------------------------------------
void LCD_GoTo(unsigned char x, unsigned char y)
{
    LCD_WriteCommand(HD44780_DDRAM_SET | (x + (0x40 * y)));
}
//-------------------------------------------------------------------------------------------------
//
// Funkcja czyszczenia ekranu wyœwietlacza.
//
//-------------------------------------------------------------------------------------------------
void LCD_Clear(void)
{
    LCD_WriteCommand(HD44780_CLEAR);
    _delay_ms(2);
}
//-------------------------------------------------------------------------------------------------
//
// Funkcja przywrócenia pocz¹tkowych wspó³rzêdnych wyœwietlacza.
//
//-------------------------------------------------------------------------------------------------
void LCD_Home(void)
{
    LCD_WriteCommand(HD44780_HOME);
    _delay_ms(2);
}
//-------------------------------------------------------------------------------------------------
//
// Procedura inicjalizacji kontrolera HD44780.
//
//-------------------------------------------------------------------------------------------------
void LCD_Initalize(void)
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
    _delay_ms(15); // oczekiwanie na ustalibizowanie się napiecia zasilajacego
    LCD_RS_PORT &= ~LCD_RS; // wyzerowanie linii RS
    LCD_E_PORT &= ~LCD_E;  // wyzerowanie linii E
#if USE_RW == 1
    LCD_RW_PORT &= ~LCD_RW;
#endif
    for(i = 0; i < 3; i++) // trzykrotne powtórzenie bloku instrukcji
    {
        LCD_E_PORT |= LCD_E; //  E = 1
        _LCD_OutNibble(0x03); // tryb 8-bitowy
        LCD_E_PORT &= ~LCD_E; // E = 0
        _delay_ms(5); // czekaj 5ms
    }

    LCD_E_PORT |= LCD_E; // E = 1
    _LCD_OutNibble(0x02); // tryb 4-bitowy
    LCD_E_PORT &= ~LCD_E; // E = 0

    _delay_ms(1); // czekaj 1ms
    LCD_WriteCommand(HD44780_FUNCTION_SET | HD44780_FONT5x7 | HD44780_TWO_LINE | HD44780_4_BIT); // interfejs 4-bity, 2-linie, znak 5x7
    LCD_WriteCommand(HD44780_DISPLAY_ONOFF | HD44780_DISPLAY_OFF); // wy³¹czenie wyswietlacza
    LCD_WriteCommand(HD44780_CLEAR); // czyszczenie zawartosæi pamieci DDRAM
#if USE_RW == 0
    _delay_ms(2);
#endif
    LCD_WriteCommand(HD44780_ENTRY_MODE | HD44780_EM_SHIFT_CURSOR | HD44780_EM_INCREMENT);// inkrementaja adresu i przesuwanie kursora
    LCD_WriteCommand(HD44780_DISPLAY_ONOFF | HD44780_DISPLAY_ON | HD44780_CURSOR_OFF | HD44780_CURSOR_NOBLINK); // w³¹cz LCD, bez kursora i mrugania
}
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
void LCD_MoveRight (unsigned int freq, unsigned int step, unsigned int way)
{
    int temp;
    for (temp=0; temp < step; temp++)
    {
        if (way)LCD_WriteCommand(HD44780_DISPLAY_CURSOR_SHIFT | HD44780_SHIFT_DISPLAY | HD44780_SHIFT_RIGHT);
        else LCD_WriteCommand(HD44780_DISPLAY_CURSOR_SHIFT | HD44780_SHIFT_CURSOR | HD44780_SHIFT_RIGHT);
        if (freq) _delay_ms(freq);
    }
}
//-------------------------------------------------------------------------------------------------
//
// Efekt przesuniêcia zawartoœci o ca³y ekran w prawo
//
//-------------------------------------------------------------------------------------------------
void LCD_MoveLeft (unsigned int freq, unsigned int step, unsigned int way)
{
    int temp;
    for (temp=0; temp < step; temp++)
    {
        if (way)LCD_WriteCommand(HD44780_DISPLAY_CURSOR_SHIFT | HD44780_SHIFT_DISPLAY | HD44780_SHIFT_LEFT);
        else LCD_WriteCommand(HD44780_DISPLAY_CURSOR_SHIFT | HD44780_SHIFT_CURSOR | HD44780_SHIFT_LEFT);
        if (freq) _delay_ms(freq);
    }
}
//-------------------------------------------------------------------------------------------------
//
// Czyszczenie zawartoœci okna
//
//-------------------------------------------------------------------------------------------------
void LCD_Erase (unsigned int row)
{
    int temp;
    if (row == 0 || row == 1)
    {
        LCD_GoTo(0,0);
        for (temp=0; temp < czterdziesci; temp++)
        {
            LCD_WriteText(" ");
        }
    }

    if (row == 0 || row == 2)
    {
        LCD_GoTo(0,1);
        for (temp=0; temp < czterdziesci; temp++)
        {
            LCD_WriteText(" ");
        }
    }
}
//-------------------------------------------------------------------------------------------------
//
// Ró¿ne opcje wyœwielania
//
//-------------------------------------------------------------------------------------------------
void LCD_Displaying (unsigned int option)
{
    switch (option)
    {
    case 1:
        LCD_WriteCommand(HD44780_DISPLAY_ONOFF | HD44780_DISPLAY_ON);
        break;
    case 2:
        LCD_WriteCommand(HD44780_DISPLAY_ONOFF | HD44780_DISPLAY_OFF);
        break;
    case 3:
        LCD_WriteCommand(HD44780_DISPLAY_ONOFF | HD44780_DISPLAY_ON | HD44780_CURSOR_ON);
        break;
    case 4:
        LCD_WriteCommand(HD44780_DISPLAY_ONOFF | HD44780_DISPLAY_ON | HD44780_CURSOR_BLINK);
        break;
    case 5:
        LCD_WriteCommand(HD44780_DISPLAY_ONOFF | HD44780_DISPLAY_ON | HD44780_CURSOR_ON | HD44780_CURSOR_BLINK);
        break;
    }
}
//-------------------------------------------------------------------------------------------------
//
// Koniec pliku HD44780.c
//
//-------------------------------------------------------------------------------------------------
