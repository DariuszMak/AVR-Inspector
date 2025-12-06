//Pomiar napiêcia przetwornikiem A/C i prezentacja wyniku na LCD 2x16 HD44780

#include <avr/io.h>
#include <util/delay.h>
#include "HD44780.h"

//##############################################################################
void main(void)
{
    int t, rozmiar=7;
    char i[rozmiar];
    LCD_Initalize();   //inicjalizacja LCD

    while(1)
    {

        LCD_GoTo(9, 1);      //Ustawienie kursora w pozycji (0,0)
        LCD_WriteText("Witaj!");
        _delay_ms(500);
        LCD_ShiftLeftScreen();

        LCD_Home();
        LCD_WriteText("(D)Arek");
        _delay_ms(500);
        LCD_ShiftRightScreen();

        _delay_ms(700);
        LCD_ScreenOff();
        _delay_ms(700);
        LCD_ScreenOn();
        _delay_ms(700);
        LCD_ScreenOff();
        _delay_ms(700);
        LCD_CursorBlink();
        _delay_ms(1000);
        LCD_Blink() ;
        _delay_ms(1000);
        LCD_Cursor();
        _delay_ms(1000);
        LCD_CursorBlink() ;
        LCD_GoTo(0,1);
        LCD_WriteText("Czytam..");
        _delay_ms(1500);

        LCD_Home();
        for(t=0; t<rozmiar; t++)
        {
            i[t] = LCD_ReadData();
            _delay_ms(150);
        }
        _delay_ms(1500);
        LCD_Cursor();
        //LCD_Clear();
        LCD_EraseAll();
        _delay_ms(1500);

        LCD_GoTo(0,1);
        for(t=0; t<rozmiar; t++)
        {
            LCD_WriteData(i[t]);
            _delay_ms(50);
        }
        LCD_Blink();
        LCD_GoTo(8,0);
        for(t=0; t<rozmiar; t++)
        {
            LCD_WriteData(i[t]);
            i[t]=0;
            _delay_ms(50);
        }
        LCD_Cursor();
        _delay_ms(1500);
        LCD_PageUpScreen ();
        LCD_PageDownScreen();
        LCD_EraseUp();
        LCD_Blink();
        _delay_ms(1000);
        LCD_PageDownScreen();
        LCD_PageUpScreen();
        LCD_Cursor();
        _delay_ms(500);
        LCD_EraseDown();
        _delay_ms(500);

        LCD_Clear();
    }
}
