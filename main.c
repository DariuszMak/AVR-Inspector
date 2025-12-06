//Pomiar napiêcia przetwornikiem A/C i prezentacja wyniku na LCD 2x16 HD44780

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#include "delay_lib.h"
#include "HD44780.h"
#include "ir_decode.h"
#include "d_led.h"
#include "random_generator.h"
#include "lockers.h"
#include "PCF8583.h"
#include "EEPROM.h"
#include "i2c.h"
#include "termometer.h"

//Program glowny:


int main( void )
{
//UWAGA!!! PONIŻSZE CHARAKTERYZACJE ZMIENNYCH SĄ BARDZO ISTOTNE W CELU POPRAWNEGO ICH UŻYWANIA W PROGRAMIE

//zmienne zarezerwowane - nie można ich używać do innych celów niż wskazane
//zmienne zarezerwowane globalnie dla całego programu
    const int liczbaPodprogramow = 3;
    int menu = 0;// zmienna odpowiedzialna za przebywanie w danym podprogramie
    int start = 1; // zmienna pomocna do stwierdzenia, czy jest się już w glownym menu = 0, czy właśnie wyszło się z podprogramu i trzeba np. zatrzymać jakiś timer = 1
    int toggle = 2;//zmienna odpowiedzialna za świadomość dłuższego przytrzymania przycisku pilota (wartość 2 jest wartością początkową w celu późniejszego skalibrowania ze stanem pilota)
    unsigned int zwiekszanie = 0; // zmienna potrzebna do zmiany wartości liczby na wyświetlaczu alfanumerycznym (przyjmuje wartości 1,10,100,1000)
//zmienne zarezerwowane dla podprogramu nr 2:
    int pozycja = 0;//zminna dodatkowa (pomocnicza) pamiętająca wylosowaną pozycję cyfry na wyświetlaczu alfanumerycznym
    int	cyfry = 0; // zmienna przechowująca wartość wyświetlaną póżniej na wyświetlaczu alfanumerycznym
//zmienne spełniające określone funkcje
    int rozmiar; // zmienna odpowiedzialna za rozmiar tablicy dynamicznej
    int t; // zmienna pomocnicza wykorzystana w pętlach for do iteracji, może być używana do przeróżnych innych operacji w programie, nie można polegać na globalnej wartości tej zmiennej, ponieważ bardzo często ulega zmianie
    //inne zmienne pomocnicze do wykorzystywania w innch podprogramach (wymaga to wcześniejszego zapoznania się z kodem)

    int u; //inna (dodatkowa) zmienna pomocnicza
    int w; //inna (dodatkowa) zmienna pomocnicza
    int s;//inna (dodatowa zmienna)


//definicje funkcji

    void buzzer()//funkcja odpowiedzialna za sygnał dźwiękowy (trwa jedną milisekundę)
    {
        PORTD |= ( 1 << PD7 );
        delay_ms_var_double( 1 );
        PORTD &= ~( 1 << PD7 );
    }

    void buzzer_time( double time )//funkcja odpowiedzialna za sygnał dźwiękowy (trwa podaną liczbę milisekund)
    {
        PORTD |= ( 1 << PD7 );
        delay_ms_var_double( time );
        PORTD &= ~( 1 << PD7 );
    }

    void wysw_skok( unsigned int number ) // funkcja wyświetlająca numer kroku o danej wartości
    {
        int d = 1;
        t = 10;
        while ( number >= t )
        {
            d += 1;
            t *= 10;
        }

        zwiekszanie = number;
        LCD_EraseAll();
        for ( t = 0; t < 40 - (d + 1); t += d + 3 )
        {
            LCD_GoTo( t, 0 );
            LCD_Int( number );
            LCD_GoTo( t, 1 );
            LCD_Int( number );
        }
        delay_ms_var_double( 500 );
    }

    void wybor( int number ) // funkcja wyświetlająca podczas wchodenia w dany podprogram numeru podprogramu
    {
        LCD_Clear();
        LCD_WriteText( "Program: " );
        LCD_Int( number );
        for ( t = 0; t < 5; ++t )
        {
            delay_ms_var_double( 10 );
            buzzer();
        }
        delay_ms_var_double( 500 );
        LCD_Clear();
    }

    void wysw( int men ) // funkcja wyświetlająca - interfejs dla każdego z podprogramów
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
            LCD_Int( command );
            LCD_GoTo( 6, 0 );
            LCD_Int( address );
            LCD_GoTo( 0, 1 );
            LCD_Int( toggle_bit );
            break;
        case 2:
            /*if ( cyfry >= 10000 ) cyfry = 10000;
            else if( cyfry <= -10000 ) cyfry = -10000;*/
            //OCR0 = cyfry;//zmienna przepełnienia Timera 0
            LCD_EraseAll();
            //d_led_Int( cyfry );
            LCD_GoTo( 0, 0 );
            LCD_Int( cyfry );

            char* tablicaTemp = (char*) malloc(rozmiar * sizeof (char*));//tablica pomoznicza do umiejscowienia cyfry

            if( u > 0 || u == -1 )//jeśli losowanie trwa, lub właśnie się kończy
            {
                for(s = 0; s < rozmiar; ++s)//wypełnienie odpowiednio tablicy dynamicznej, np. "0010" - cyfra stoi na 3 miemscu
                {
                    if(s == pozycja) tablicaTemp[s] = '1';
                    else tablicaTemp[s] = '0';
                }
                if(u > 0)//gdzy losowanie trwa, wyświetlają się procenty
                {
                    LCD_GoTo(4,0);
                    LCD_Int((int) (t * 100 / 250));
                    LCD_WriteText("%");
                }

                for(s = 0; s < rozmiar; ++s)//odpowiednie wyświetlanie na ekranie
                {
                    if(tablicaTemp[s] == '1')
                    {
                        if (s == 0) cy1 = cyfry;
                        if (s == 1) cy2 = cyfry;
                        if (s == 2) cy3 = cyfry;
                        if (s == 3) cy4 = cyfry;
                    }
                    else
                    {
                        if (s == 0 || cyfry == 0) cy1 = 10;
                        if (s == 1 || cyfry == 0) cy2 = 10;
                        if (s == 2 || cyfry == 0) cy3 = 10;
                        if (s == 3 || cyfry == 0) cy4 = 10;
                    }
                }
                if ( u == -1 )//koniec animacji - wyświetlenie kresek poziomych
                {
                    for(s = 0; s < rozmiar; ++s)
                    {
                        if(tablicaTemp[s] == '0' || cyfry == 0 )
                        {
                            if ( s == 0 ) cy1 = 11;
                            if ( s == 1 ) cy2 = 11;
                            if ( s == 2 ) cy3 = 11;
                            if ( s == 3 ) cy4 = 11;
                            buzzer();
                            delay_ms_var_double(100);
                        }
                    }
                    u = 0;
                }
            }

            free(tablicaTemp);

            break;
        case 3:
            LCD_EraseAll();
            int moveStep=0;
            PCF8583_get_wall_time();

            LCD_GoTo( 0 + moveStep, 0 );
            if(godz < 10) LCD_Int(0);
            LCD_Int(godz);
            LCD_WriteText(":");
            if(min < 10) LCD_Int(0);
            LCD_Int(min);
            LCD_WriteText(":");
            if(sek < 10) LCD_Int(0);
            LCD_Int(sek);
            LCD_WriteText(":");
            if(hsek < 10) LCD_Int(0);
            LCD_Int(hsek);
            LCD_WriteText(":");
            LCD_Int(dzien_tygodnia);
            LCD_GoTo( 0 + moveStep, 1 );
            if(dzien < 10) LCD_Int(0);
            LCD_Int(dzien);
            LCD_WriteText(":");
            if(miesiac < 10) LCD_Int(0);
            LCD_Int(miesiac);
            LCD_WriteText(":");
            LCD_Int(rok);

            LCD_WriteText(":");
            LCD_Double(ds18b20_temperature(),2);

            PCF8583_get_wall_alarm();
            moveStep=24;

            LCD_GoTo( 0 + moveStep, 0 );
            if(godz < 10) LCD_Int(0);
            LCD_Int(godz);
            LCD_WriteText(":");
            if(min < 10) LCD_Int(0);
            LCD_Int(min);
            LCD_WriteText(":");
            if(sek < 10) LCD_Int(0);
            LCD_Int(sek);
            LCD_WriteText(":");
            if(hsek < 10) LCD_Int(0);
            LCD_Int(hsek);
            LCD_WriteText(":");
            LCD_Int(dzien_tygodnia);
            LCD_GoTo( 0 + moveStep, 1 );
            if(dzien < 10) LCD_Int(0);
            LCD_Int(dzien);
            LCD_WriteText(":");
            if(miesiac < 10) LCD_Int(0);
            LCD_Int(miesiac);
            LCD_WriteText(" ");
            LCD_Int(PCF8583_recognise_type_of_alarm());




            //LCD_Int( pwm1 );
            //LCD_Int( pwm2 );
            //OCR0 = pwm1;//zmienna przepełnienia Timera 0
            break;
        }
    }

    void czynnosc( const int * const men, int com, int tog ) //funkcja odpowiedzialna za wywołanie odpowiedniej czynności (pierwszy argument musi być przez wskaźnik, ponieważ, może być dokonana zmiana zmiennej "menu")
    {
        buzzer();

        switch( *men )//warianty w zależności od zmiennej menu, na końcu każdego wywoływana jest funkcja wyświetlająca
        {

        case 0:
            switch ( com )
            {
            case 41:
                LCD_Clear();
                rozmiar = LCD_CHARSPERLINE;
                char original_text_static[2][40] = { {"ATmega32 programabcdefghijklmnopqrstuvwx"}, {"Dariusz M. proj.yz1234567890987654321!@$"}};

                char** original_text = ( char** ) malloc( 2 * sizeof (*original_text) );//tablica dwuwymiarowa z testowym napisem

                for ( t = 0; t < 2; ++t )
                {
                    original_text[t] = ( char* ) malloc ( LCD_CHARSPERLINE * sizeof ( *original_text ) );
                    for (u = 0; u < rozmiar; ++u)
                    {
                        original_text[t][u] = original_text_static[t][u];
                    }
                }

                /*original_text[0] = "ATmega32 programabcdefghijklmnopqrstuvwx";//błąd, bo nie działa przy powtórnym użyciu
                original_text[1] = "Dariusz M. proj.yz1234567890987654321!@$";*/

                for( t = 0; t < LCD_CHARSPERLINE; ++t )
                {
                    LCD_WriteData( original_text[0][t] );
                }

                LCD_GoTo( 0, 1 );

                for( t = 0; t < LCD_CHARSPERLINE; ++t )
                {
                    LCD_WriteData( original_text[1][t] );
                }

                rozmiar = 40;
                u = 1;

                while( u )
                {
                    char** buffer_table = ( char** ) malloc( 2 * sizeof (*buffer_table) );//tablia dwuwymiarowa jako bufor do odczytu z wyświetlacza

                    for ( t = 0; t < 2; ++t )
                    {
                        buffer_table[t] = ( char* ) malloc ( rozmiar * sizeof ( *buffer_table ) );
                    }

                    LCD_Home();

                    for( t = 0; t < rozmiar; ++t )
                    {
                        buffer_table[0][t] = LCD_ReadData();
                    }

                    LCD_GoTo( 0, 1 );

                    for( t = 0; t < rozmiar; ++t )
                    {
                        buffer_table[1][t] = LCD_ReadData();
                    }

                    LCD_Clear();

                    for( t = 0; t < rozmiar; ++t )
                    {
                        if( buffer_table[0][t] != original_text[0][t] || buffer_table[1][t] != original_text[1][t]) u = 0;
                    }

                    if ( stop_button() )//jeśli przycisk zatrzymania został wciśnięty
                    {
                        u = 0;
                    }

                    if( u )
                    {
                        for( t = 0; t < rozmiar; ++t )
                        {
                            LCD_WriteData( buffer_table[0][t] );
                        }

                        LCD_GoTo( 0, 1 );

                        for( t = 0; t < rozmiar; ++t )
                        {
                            LCD_WriteData( buffer_table[1][t] );
                        }
                    }

                    for ( t = 0; t < 2; ++t )
                    {
                        free( buffer_table[t] );
                    }
                    free( buffer_table );
                }

                for ( t = 0; t < 2; ++t )
                {
                    free( original_text[t] );
                }
                free( original_text );

                buzzer();
                delay_ms_var_double(10);
                buzzer();
                delay_ms_var_double(10);
                buzzer();

                break;
            case 12:
                LCD_Clear();
                LCD_Blink();
                LCD_GoTo( 9, 1 );
                LCD_WriteText( "Witaj!" );
                delay_ms_var_double( 500 );
                LCD_Home();
                LCD_WriteText( "LCD HD44780" );
                delay_ms_var_double( 500 );

                LCD_ShiftRightScreen();
                delay_ms_var_double( 700 );

                LCD_ScreenOff();
                delay_ms_var_double( 700 );
                LCD_ScreenOn();
                delay_ms_var_double( 700 );

                LCD_ScreenOff();
                delay_ms_var_double( 700 );
                LCD_CursorBlink();
                delay_ms_var_double( 1000 );
                LCD_Blink() ;
                LCD_ShiftRightCursor();
                delay_ms_var_double( 1000 );
                LCD_ShiftLeftCursor();
                LCD_Cursor();
                delay_ms_var_double( 1000 );
                LCD_CursorBlink() ;
                LCD_GoTo( 0, 1 );
                LCD_WriteText( "Czytam:" );
                delay_ms_var_double( 1500 );

                LCD_GoTo( 9, 1 );

                rozmiar = 6;

                char * i =  ( char* ) malloc( rozmiar * sizeof (*i) );

                void pisz( void )
                {
                    for( t = 0; t < rozmiar; ++t )
                    {
                        LCD_WriteData( i[t] );
                        delay_ms_var_double( 50 );
                    }
                }

                for( t = 0; t < rozmiar; ++t )
                {
                    i[t] = LCD_ReadData();
                    delay_ms_var_double( 100 );
                }

                LCD_ShiftLeftScreen();
                delay_ms_var_double( 1500 );

                LCD_Cursor();
                LCD_EraseAll();
                delay_ms_var_double( 1500 );
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
                delay_ms_var_double( 1500 );
                LCD_PageUpScreen ();
                LCD_PageDownScreen();
                LCD_EraseUp();
                LCD_Blink();
                delay_ms_var_double( 1000 );
                LCD_PageDownScreen();
                LCD_PageUpScreen();
                LCD_Cursor();
                LCD_EraseDown();
                delay_ms_var_double( 500 );

                LCD_Clear();
                LCD_ScreenOn();
                break;
            }
            wysw( *men );
            break;
        case 1:
            LCD_Displaying( com );
            if ( com != 100 ) wysw( *men );
            break;
        case 2:
            switch( com )
            {
            case 100://reakcja na naciśnięcie przycisku "stop"
            case 41://reakcja na naciśnięcie przyciku z pilota RC5

                t = 1;//zmienna odpowiedzialna za ilość podjętych prób losowań
                u = 1;//zmienna pomocnicza, pamięta wylosowaną liczbę kropek na kostce, przydaje się w różnych trybach wyświetlania

                do
                {
                    buzzer_time(0.4);
                    ++t;
                    wysw ( *men );
                    delay_ms_var_double(750/t+10);//rozpędzanie kostki im dalej, tym szybciej
                }
                while (stop_button() && t != 250);//przerwanie rozpędzania po puszczeniu przyciksu lub po przekroczniu zakresu

                delay_ms_var_double(200);
                PCF8583_get_wall_time();
                for(w = 0; w < hsek; ++w)
                {
                    rand();
                }

                while ( t != 1 )
                {
                    u=rand()%6 + 1;//wylosowanie liczby oczek na kostce

                    for(w = 1; w <= u; ++w)//przekulnięcia kostki w danej próbie
                    {
                        delay_ms_var_double((1+2500/t)/(7-w));//specjalny interwał zwalniający
                        pozycja = rand() % rozmiar;//wylosowanie pozycji na wyświetlaczu;
                        cyfry = w;
                        wysw ( *men );
                        buzzer_time(0.8);
                    }
                    --t;
                }
                t++;//przywrócenie efektu z ostatniej tury
                if( w < 7 )delay_ms_var_double((1+2500/t)/(7-w));//jeszcze jedno opóźnienie
                u = -1;//tryb wyświetlania
                wysw( *men );
                //cy1 = 8;

                break;
            case 59:
                /*d_led_Int ( 9000 );
                delay_ms_var_double( 1000 );
                d_led_Int ( 8765 );
                delay_ms_var_double( 1000 );
                d_led_Int ( 4321 );
                delay_ms_var_double( 1000 );
                d_led_Int ( 0 );
                delay_ms_var_double( 1000 );
                d_led_Int ( -123 );
                delay_ms_var_double( 1000 );
                d_led_Int ( -3 );
                delay_ms_var_double( 1000 );
                d_led_Int ( -1000 );
                delay_ms_var_double( 1000 );*/
                cyfry = 0;
                pozycja = 0;
                u = -1;
                wysw( *men );
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
                //cyfry -= zwiekszanie;
                break;
            case 16:
                //cyfry += zwiekszanie;
                break;
            case 14:
                switch(tog)
                {
                case 0:
                    break;
                case 1://wygaszenie elementów wyświetlacza podczas opuszczania podprogramu
                    TCCR0 |= ( 1 << CS02 ) | ( 1 << CS00 ); // timer włączony
                    cy1 = 10;
                    cy2 = 10;
                    cy3 = 10;
                    cy4 = 10;
                    break;
                }
                break;
            }
            wysw( *men );
            break;
        case 3:
            switch ( com )
            {
            case 1:
                EEPROM_write(10, 0);
                EEPROM_write(11, 0);
                EEPROM_write(12, 0);
                EEPROM_write(13, 0);
                EEPROM_write(14, 0);
                EEPROM_write(15, 0);
                EEPROM_write_word(16, 0);
                EEPROM_write(18,0);
                break;
            case 2:
                EEPROM_write(10, 1);
                EEPROM_write(11, 2);
                EEPROM_write(12, 0);
                EEPROM_write(13, 0);
                EEPROM_write(14, 5);
                EEPROM_write(15, 6);
                EEPROM_write_word(16, 1234);
                EEPROM_write(18,0b00010000);
                break;
            case 55:
                wysw_skok( 1000 );
                PCF8583_alarm_off();
                break;
            case 54:
                wysw_skok( 100 );
                PCF8583_alarm_every_day();
                break;
            case 50:
                wysw_skok( 10 );
                PCF8583_alarm_weekly();
                break;
            case 52:
                wysw_skok( 1 );
                PCF8583_alarm_monthly();
            case 59:
                PCF8583_alarm_flag_off();
                break;

            case 41:
                PCF8583_set_time( 23, 59, 55, 1 );
                PCF8583_set_date( 13, 3, 8, 2015 );
                break;
            case 15:
            hsek = EEPROM_read(10);
            sek = EEPROM_read(11);
            min = EEPROM_read(12);
            godz = EEPROM_read(13);
            dzien = EEPROM_read(14);
            miesiac = EEPROM_read(15);
            rok = EEPROM_read_word(16);
            dzien_tygodnia = EEPROM_read(18);
            PCF8583_set_weekly_alarm( EEPROM_read(18),  EEPROM_read(13), EEPROM_read(12), EEPROM_read(11), EEPROM_read(10));

                break;
            case 17:
//                pwm1 -= zwiekszanie;
                break;
            case 16:
//                pwm1 += zwiekszanie;
                break;
            case 32:
//                pwm2 += zwiekszanie;
                break;
            case 33:
//                pwm2 -= zwiekszanie;
                break;
            }
            wysw( *men );
            break;
        }
//komendy wspólne dla wszystkich podprogramów

        switch ( com )
        {
        case 38:
            LCD_Clear();
            LCD_WriteText( "Na poczatek" );
            LCD_GoTo( 0, 1 );
            LCD_WriteText( "ekranu..." );
            delay_ms_var_double( 250 );
            LCD_PageUpScreen();
            LCD_PageDownScreen();
            LCD_Clear();
            wysw( *men );
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
            switch(tog)
            {
            case 0:
                break;
            case 1:
                start = 1;//oznaka wyjścia z podprogramów
                break;
            }
            break;
        }
    }

// funkcja obsługująca menu dwupoziomowe

    void pilot( int * const men , int com, int tog )//
    {
        TCCR1B &= ~( ( 1 << CS12 ) | ( 1 << CS11 ) | ( 1 << CS10 ) ); //wyłączenie Timera1 (prescaler na zero)

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
                    wysw( *men );//niepotrzebne, gdy mają być wywoływane jakieś przyciski
                    break;

                case 2:
                    rozmiar = 4;
                    u = -1;//wymuszenie wykonania animacji z kreskami
                    t = 0;
                    TCCR0 |= ( 1 << CS02 ) | ( 1 << CS00 ); // timer włączony od wyświetlacza alfanumerycznego
                    wysw( *men );//niepotrzebne, gdy mają być wywoływane jakieś przyciski
                    break;

                case 3:
                    czynnosc( men, 50, tog );
                    //wysw( *men, com );//niepotrzebne, gdy mają być wywoływane jakieś przyciski
                    break;
                }
            }
        }

        if( !start ) czynnosc( men, com, tog );//jeśli jest się już w menu głównym, a nie idzie się właśnie do jakiegoś podprogramu

        if ( start == 1 )//jeśli było się w jakimś podprogramie i właśnie przechodzimy do podprogramu głównego
        {
            start = 0;//informacja, że zaraz będziemy "chwilę" w menu głównym
            *men = 0;//
            TCCR0 &= ~( ( 1 << CS02 ) | ( 1 << CS00 ) ); // timer 0 od wyświetlacza alfanumerycznego wyłączony
            wybor( *men );
            wysw ( *men );// wyświetlenie ekranu
        }
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

// funkcja odpowiedzialna za odczytanie komend z pilota i przekazaniu ich do fukcji pilot, dopóki nie zostaną wykonane wszystkie rozkazy, nie będzie można odzczytać innego przysisku

    void zczytaj_komende( void )
    {
        if( interr && (menu == 3))
        {
            wysw(menu);
            interr = 0;
            cnt = 0;
        }
        if ( stop_button())
        {
            delay_ms_var_double(30);
            if (stop_button()) pilot( &menu, 100, 0 );//wywołanie funkcji pilot przez naciśnięcie przycisku
            delay_ms_var_double(100);
        }

        if( Ir_key_press_flag )
        {
            if( !address )
            {
                //TCCR1B &= ~( ( 1 << CS12 ) | ( 1 << CS11 ) | ( 1 << CS10 ) ); //wyłączenie Timera1 (prescaler na zero)
                t = 0;//zmienna pomocnicza
                if(toggle != 2)//jeśli zmiaenna "toggle" ma wartość inną niż na samym początku programu
                {
                    if(toggle == toggle_bit)//jeśli stara zapamiętana wartość zmiennej "toggle" jest taka sama jak "toggle_bit", to oznacza to, że przycisk pilota zotał dłużej przytrzymany
                    {
                        t = 1;//przypisanie jednynki - przycisk zostal dłużej przyciśnięty
                    }
                    toggle = toggle_bit;//przypiwanie obecnej wartości do zmiennej "toggle"
                }
                else toggle = toggle_bit;//przypisanie obecnej wartości do zmiennej "toggle"

                pilot( &menu, command, t );//wywołanie funkcji pilot
                Ir_key_press_flag = 0;
                command = 0xff;
                address = 0xff;
            }
        }
    }


//Koniec definicji metod

//Inicjalizacja

    i2cSetBitrate(100);//inicjalizacja i2c - utawienie częstotliwości w kHz
    PCF8583_init();//inicjlalizacja wyświetlacza


    DDRD |= ( 1 << PD7 );// PORTD7 jako wyjście do buzzera
    ds18b20_temperature();//zmierzenie temperatury
    random_generator_init();//włączenie losowaniacyfr
    LCD_Initalize();//inicjalizacja wyświetlacza
    ir_init();//inicjalizacja odbioru sygnału z pilota
    d_led_init();//inicjalizacja wyświetlacza alfanumerycznego
    lockers_init();//inicjalizacja przycisku wejściowego oraz wejścia i wyjcia

    sei();//włącza przerwania

    pilot( &menu, 0, 0 );//rozpoczęcie programu od głównego menu

    //główna pętla programu

    while( 1 )
    {
        zczytaj_komende();
        lockers_check_events();
    }

    return 0;
}
