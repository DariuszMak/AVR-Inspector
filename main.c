//Pomiar napiêcia przetwornikiem A/C i prezentacja wyniku na LCD 2x16 HD44780

#include <avr/io.h>
#include <util/delay.h>
#include "HD44780.h"

//##############################################################################
int main(void)
{
    while(1)
    {
        int t, rozmiar=7;
        char i[rozmiar];
        LCD_Initalize();   //inicjalizacja LCD
        LCD_GoTo(3, 1);      //Ustawienie kursora w pozycji (0,0)
        LCD_WriteText("Witaj!");
        _delay_ms(500);

        LCD_Home();
        LCD_WriteText("(D)Arek");
        _delay_ms(500);

        LCD_WriteCommand(HD44780_DISPLAY_ONOFF | HD44780_DISPLAY_ON | HD44780_CURSOR_ON);
        _delay_ms(1500);

        LCD_WriteCommand(HD44780_DISPLAY_ONOFF | HD44780_DISPLAY_ON | HD44780_CURSOR_ON | HD44780_CURSOR_BLINK);
        _delay_ms(1500);

        LCD_Home();
        for(t=0; t<rozmiar; t++){
            i[t] = LCD_ReadData();
            _delay_ms(150);
        }
        _delay_ms(1500);

        LCD_Clear();
        _delay_ms(1500);

        LCD_GoTo(0,1);
        for(t=0; t<rozmiar; t++){
            LCD_WriteData(i[t]);
            _delay_ms(50);
        }

        LCD_GoTo(8,0);
        for(t=0; t<rozmiar; t++){
            LCD_WriteData(i[t]);
            _delay_ms(50);
        }

        _delay_ms(1500);
        for (t=0; t<15; t++){
            LCD_WriteCommand(HD44780_DISPLAY_CURSOR_SHIFT | HD44780_SHIFT_DISPLAY | HD44780_SHIFT_RIGHT);
            _delay_ms(25);
        }

        for (t=0; t<15; t++){
            LCD_WriteCommand(HD44780_DISPLAY_CURSOR_SHIFT | HD44780_SHIFT_DISPLAY | HD44780_SHIFT_LEFT);
            _delay_ms(25);
        }


        for (t=0; t<15; t++){
            LCD_WriteCommand(HD44780_DISPLAY_CURSOR_SHIFT | HD44780_SHIFT_DISPLAY | HD44780_SHIFT_LEFT);
            _delay_ms(25);
        }

        for (t=0; t<15; t++){
            LCD_WriteCommand(HD44780_DISPLAY_CURSOR_SHIFT | HD44780_SHIFT_DISPLAY | HD44780_SHIFT_RIGHT);
            _delay_ms(25);
        }


        _delay_ms(1500);
        LCD_Clear();

//        i = LCD_ReadData();
//        _delay_ms(500);
//        LCD_Clear();
//        _delay_ms(500);
//        LCD_WriteText('i');
//        _delay_ms(500);
        ;
    }

}
