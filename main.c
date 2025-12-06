//Pomiar napiêcia przetwornikiem A/C i prezentacja wyniku na LCD 2x16 HD44780

#include <avr/io.h>
#include <util/delay.h>
#include "HD44780.h"
#define _delay_ms delay_ms_var_double
#define _delay_us delay_ms_var_double

void pisz(void);

//##############################################################################
int main(void)
{
    int t, rozmiar=6;
    char i[rozmiar];
    void pisz(void)
    {
        for(t=0; t<rozmiar; t++)
        {
            LCD_WriteData(i[t]);
            _delay_ms(50);
        }
    }

    LCD_Initalize();

    while(1)
    {
        LCD_GoTo(9, 1);
        LCD_WriteText("Witaj!");
        _delay_ms(500);
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
        LCD_ShiftRightCursor();
        _delay_ms(1000);
        LCD_ShiftLeftCursor();
        LCD_Cursor();
        _delay_ms(1000);
        LCD_CursorBlink() ;
        LCD_GoTo(0,1);
        LCD_WriteText("Czytam:");
        _delay_ms(1500);

        LCD_GoTo(9,1);

        for(t=0; t<rozmiar; t++)
        {
            i[t] = LCD_ReadData();
            _delay_ms(100);
        }
        LCD_ShiftLeftScreen();
        _delay_ms(1500);

        LCD_Cursor();
        LCD_EraseAll();
        _delay_ms(1500);
        LCD_GoTo(0,1);
        pisz();
        LCD_Blink();
        LCD_GoTo(9,0);
        pisz();
        LCD_GoTo(8,1);
        LCD_CursorBlink();
        pisz();
        LCD_GoTo(1,0);
        pisz();

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
        LCD_EraseDown();
        _delay_ms(500);

        LCD_Clear();
    }
    return 0;
}


