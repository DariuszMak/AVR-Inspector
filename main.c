//Pomiar napiêcia przetwornikiem A/C i prezentacja wyniku na LCD 2x16 HD44780

#include "main.h"

//UWAGA!!! PONIŻSZE CHARAKTERYZACJE ZMIENNYCH SĄ BARDZO ISTOTNE W CELU POPRAWNEGO ICH UŻYWANIA W PROGRAMIE

//zmienne zarezerwowane - nie można ich używać do innych celów niż wskazane
//zmienne zarezerwowane globalnie dla całego programu
const int liczbaPodprogramow = 6;
uint8_t menu = 0;// zmienna odpowiedzialna za przebywanie w danym podprogramie
int8_t start = 1; // zmienna pomocna do stwierdzenia, czy jest się już w glownym menu = 0, czy właśnie wyszło się z podprogramu i trzeba np. zatrzymać jakiś timer = 1
int8_t toggle = 2;//zmienna odpowiedzialna za świadomość dłuższego przytrzymania przycisku pilota (wartość 2 jest wartością początkową w celu późniejszego skalibrowania ze stanem pilota)
uint8_t moveStep = 0;//zmienna do przesunięcia wyświetlanych partii danych (dla daty)
uint8_t pilot_state = 0;//zmienna odpowiedzialna za działanie, bądź niedziałanie timera od odczytu pilota
uint8_t checking_lockers_state = 0;//zmienna odpowiedzialna za sprawdzanie stanów wejść
//zmienne zarezerwowane dla podprogramu nr 2:
uint8_t pozycja = 0;//zminna dodatkowa (pomocnicza) pamiętająca wylosowaną pozycję cyfry na wyświetlaczu alfanumerycznym
int8_t	cyfra = 0; // zmienna przechowująca wartość wyświetlaną póżniej na wyświetlaczu alfanumerycznym


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

void wysw_skok( uint16_t number ) // funkcja wyświetlająca numer kroku o danej wartości
{
    uint8_t d = 1;
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
        LCD_Int( zwiekszanie );
        LCD_GoTo( t, 1 );
        LCD_Int( zwiekszanie );
    }
    delay_ms_var_double( 500 );
    wysw();
}

void step_increase(void)
{
    zwiekszanie *= 10;
    if(zwiekszanie > 1000) zwiekszanie = 1000;
    wysw_skok(zwiekszanie);
}

void step_decrease(void)
{
    zwiekszanie /= 10;
    if (zwiekszanie < 1 ) zwiekszanie = 1;
    wysw_skok(zwiekszanie);
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

void show_day_of_week( uint8_t day)
{
    if(day == 0) LCD_WriteText("Pn.");
    else if(day == 1) LCD_WriteText("Wt.");
    else if(day == 2) LCD_WriteText("Sr.");
    else if(day == 3) LCD_WriteText("Cz.");
    else if(day == 4) LCD_WriteText("Pt.");
    else if(day == 5) LCD_WriteText("So.");
    else if(day == 6) LCD_WriteText("Nd.");
}

void show_time_only_format(void)
{
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

}

void show_time_format(void)
{
    show_time_only_format();

    LCD_GoTo( 0 + moveStep, 1 );
    if(dzien < 10) LCD_Int(0);
    LCD_Int(dzien);
    LCD_WriteText(":");
    if(miesiac < 10) LCD_Int(0);
    LCD_Int(miesiac);
    LCD_WriteText(":");
    LCD_Int(rok);

    LCD_GoTo(13,1);
    show_day_of_week(dzien_tygodnia);
}

void show_alarm_format(uint8_t case_of_format)
{

    if(case_of_format == 2)
    {
        if(miesiac == 0)
        {
            LCD_WriteText("-------");
        }
        else
        {
            for(w = 0; w < 7; ++w)
            {
                if(miesiac & (1 << w))
                {
                    LCD_WriteText("|");
                    LCD_Int(w+1);
                }
            }
            LCD_WriteText("|");
        }
    }
    else if ( case_of_format == 3)
    {
        if(dzien < 10) LCD_Int(0);
        LCD_Int(dzien);
        LCD_WriteText(":");
        if(miesiac < 10) LCD_Int(0);
        LCD_Int(miesiac);
    }
}

void setting_information(uint8_t case_of_time, uint8_t u)
{
    if(u == 0) LCD_WriteText("GODZINY");
    else if(u == 1) LCD_WriteText("MINUTY");
    else if(u == 2) LCD_WriteText("SEKUNDY");
    else if(u == 3) LCD_WriteText("SETNE SEKUND");

    if(case_of_time == 0 || case_of_time == 3)
    {
        if(u == 4) LCD_WriteText("DZIEN");
        else if(u == 5) LCD_WriteText("MIESIAC");
        if(case_of_time == 0)
        {
            if(u == 6) LCD_WriteText("ROK");
            else if(u == 7) LCD_WriteText("DZIEN TYGODNIA");
        }
    }
    if( case_of_time == 2)
    {
        if(u == 4) LCD_WriteText("PONIEDZIALEK");
        else if(u == 5) LCD_WriteText("WTOREK");
        else if(u == 6) LCD_WriteText("SRODA");
        else if(u == 7) LCD_WriteText("CZWARTEK");
        else if(u == 8) LCD_WriteText("PIATEK");
        else if(u == 9) LCD_WriteText("SOBOTA");
        else if(u == 10) LCD_WriteText("NIEDZIELA");
    }

}

void set_appropriate_values_of_time(uint8_t case_of_time, uint8_t u, uint8_t s)
{
    int16_t temp = 0;
    if(s == 2) temp -= zwiekszanie;
    else if (s == 1) temp = zwiekszanie;


    if(u == 0) godz += temp;
    else if(u == 1) min += temp;
    else if(u == 2) sek += temp;
    else if(u == 3) hsek += temp;

    if(case_of_time == 0 || case_of_time == 3)
    {
        if(u == 4) dzien += temp;
        else if(u == 5) miesiac += temp;
        if(case_of_time == 0)
        {
            if(u == 6)rok += temp;
            else if(u == 7) dzien_tygodnia += temp;
        }
    }
    if( case_of_time == 2)
    {
        if(u == 4)
        {
            if(s == 1) miesiac |= (1 << 0);
            else if(s == 2) miesiac &= ~(1 << 0);
        }
        if(u == 5)
        {
            if(s == 1) miesiac |= ~(1 << 1);
            else if(s == 2) miesiac &= ~(1 << 1);
        }
        if(u == 6)
        {
            if(s == 1) miesiac |= ~(1 << 2);
            else if(s == 2) miesiac &= ~(1 << 2);
        }
        if(u == 7)
        {
            if(s == 1) miesiac |= ~(1 << 3);
            else if(s == 2) miesiac &= ~(1 << 3);
        }
        if(u == 8)
        {
            if(s == 1) miesiac |= ~(1 << 4);
            else if(s == 2) miesiac &= ~(1 << 4);
        }
        if(u == 9)
        {
            if(s == 1) miesiac |= ~(1 << 5);
            else if(s == 2) miesiac &= ~(1 << 5);
        }
        if(u == 10)
        {
            if(s == 1) miesiac |= ~(1 << 6);
            else if(s == 2) miesiac &= ~(1 << 6);
        }
    }
}

void show_alarm_options(uint8_t index)
{

    if(index == 0)
    {
        LCD_WriteText("Alarm wylaczony");
    }
    else if(index == 1)
    {
        LCD_WriteText("Alarm codzienny");
    }
    else if(index == 2)
    {
        LCD_WriteText("Alarm tygodniowy");
    }
    else if(index == 3)
    {
        LCD_WriteText("Alarm miesieczny");
    }
}

void correction_of_time(void)
{
    if(godz < 0) godz = 23;
    else if(godz > 23) godz = 0;
    if(min < 0) min = 59;
    else if(min > 59) min = 0;
    if(sek < 0) sek = 59;
    else if(sek > 59) sek = 0;
    if(hsek < 0) hsek = 99;
    else if(hsek > 99) hsek = 0;
}

void correction_of_date(uint8_t check_with_year)//uwzględnianie dnia miesiąca względem roku
{

    if(miesiac < 1) miesiac = 12;
    else if(miesiac > 12) miesiac = 1;

    uint8_t case_of_day = 0;
    if(miesiac == 1 || miesiac == 3 || miesiac == 5 || miesiac == 7 || miesiac == 8 || miesiac == 10 || miesiac == 12) case_of_day = 31;
    else if (miesiac == 4 || miesiac == 6 || miesiac == 9 || miesiac == 11) case_of_day = 30;
    else if (miesiac == 2 && (rok % 4) != 0 && check_with_year == 1) case_of_day = 28;
    else if ((miesiac == 2 && (rok % 4) == 0) || check_with_year == 0) case_of_day = 29;

    if(dzien < 1) dzien = case_of_day;
    else if(dzien > case_of_day) dzien = 1;

    if(rok < -9999) rok = 9999;
    else if(rok > 9999) rok = -9999;

    if(dzien_tygodnia < 0) dzien_tygodnia = 6;
    else if(dzien_tygodnia > 6) dzien_tygodnia = 0;
}

void show_frame( int8_t number)
{
    LCD_Int(number);
    LCD_WriteText(". ");
    if(frame.hours < 10) LCD_Int(0);
    LCD_Int(frame.hours);
    LCD_WriteText(":");
    if(frame.minutes < 10) LCD_Int(0);
    LCD_Int(frame.minutes);
    LCD_WriteText(":");
    if(frame.seconds < 10) LCD_Int(0);
    LCD_Int(frame.seconds);
    LCD_WriteText(" ");

    if(frame.day < 10) LCD_Int(0);
    LCD_Int(frame.day);
    LCD_WriteText(":");
    if(frame.month < 10) LCD_Int(0);
    LCD_Int(frame.month);
    LCD_WriteText(":");
    LCD_Int(frame.year);

    LCD_WriteText(" ");

    number = frame.information % 100;
    if(number != 0)
    {

        LCD_WriteText("NR: ");
        LCD_Int(number);
        LCD_WriteText(" ");
        t = frame.information / 100;
        if(t == 1) LCD_WriteText("OTWARCIE");
        else if(t == 2) LCD_WriteText("ZAMKNIECIE");
    }
}

void show_list(uint16_t current_index, uint16_t max_index)
{

    if ( current_index != 0)
    {
        LCD_GoTo(0, 0);
        if(menu == 6)
        {
            lockers_read_frame(current_index - 1);
            show_frame(current_index);
        }
        else if (menu == 5)
        {
            show_alarm_options(current_index);
        }
    }

    if(current_index != max_index)
    {
        LCD_GoTo(0, 1);
        if(menu == 6)
        {
            lockers_read_frame(current_index);
            show_frame(current_index + 1);
        }
        else if (menu == 5)
        {
            show_alarm_options(current_index);
        }

    }
}

void wysw( void ) // funkcja wyświetlająca - interfejs dla każdego z podprogramów
{
    switch ( menu )
    {
    case 0:
        LCD_EraseAll();
        LCD_GoTo( 0, 0 );
        LCD_WriteText( "Wybierz:" );
        LCD_GoTo( 0, 1 );
        LCD_WriteText( "1 - " );
        LCD_Int(liczbaPodprogramow);

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
        LCD_Int( cyfra );

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
                    if (s == 0) cy1 = cyfra;
                    if (s == 1) cy2 = cyfra;
                    if (s == 2) cy3 = cyfra;
                    if (s == 3) cy4 = cyfra;
                }
                else
                {
                    if (s == 0 || cyfra == 0) cy1 = 10;
                    if (s == 1 || cyfra == 0) cy2 = 10;
                    if (s == 2 || cyfra == 0) cy3 = 10;
                    if (s == 3 || cyfra == 0) cy4 = 10;
                }
            }
            if ( u == -1 )//koniec animacji - wyświetlenie kresek poziomych
            {
                for(s = 0; s < rozmiar; ++s)
                {
                    if(tablicaTemp[s] == '0' || cyfra == 0 )
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
        moveStep=0;
        PCF8583_get_wall_time();
        LCD_EraseAll();
        LCD_GoTo(11,0);
        if(pilot_state == 0) LCD_WriteText("!");
        else if(pilot_state == 1) LCD_WriteText("|");

        show_time_format();

        PCF8583_get_wall_alarm();//wczytanie wartości umieszczonych w alarmie

        moveStep=21;

        if(u != 0)
        {
            LCD_GoTo(18, 0);
            LCD_WriteText("|");

            LCD_GoTo(18, 1);
            LCD_WriteText("|");

            show_time_only_format();
        }

        LCD_GoTo( 0 + moveStep, 1 );

        show_alarm_format(u);


        LCD_GoTo(12, 0);
        LCD_Double(ds18b20_temperature(),1);

        //LCD_Int( pwm1 );
        //LCD_Int( pwm2 );
        //OCR0 = pwm1;//zmienna przepełnienia Timera 0
        break;
    case 4:
        LCD_EraseAll();

        if (u < 0) u = 0;

        if(u == 8)
        {
            PCF8583_set_time(godz,min,sek,hsek);
            PCF8583_set_date(dzien,dzien_tygodnia,miesiac,rok);
            LCD_Clear();
            LCD_GoTo(0,0);
            LCD_WriteText("ZAPISANO!");
            delay_ms_var(500);
            start = 1;
        }

        if( w == 1 )
        {
            LCD_EraseAll();
            LCD_GoTo(moveStep, 0);
            setting_information(0, u);
            w = 0;
            delay_ms_var(400);
            LCD_EraseAll();
        }

        if((u == 0 || u == 1 || u == 2 || u == 3 || u == 4 || u == 5 )&& zwiekszanie > 10) wysw_skok(10);
        else if(u == 7 && zwiekszanie > 1) wysw_skok(1);

        if( s != 0 )
        {
            set_appropriate_values_of_time(0, u, s);

            s = 0;
        }

        correction_of_time();

        correction_of_date(1);

        moveStep = 0;
        show_time_format();
        break;
    case 5:
        LCD_EraseAll();
        if(c < 0) c = 4;
        else if(c > 4) c = 0;
        show_list(c, 4);


        break;
    case 6:
        LCD_EraseAll();
        if(zwiekszanie > 10) wysw_skok(10);
        if(u < 0) u = lockers_number_of_frames();
        else if(u > lockers_number_of_frames()) u = 0;
        show_list(u, lockers_number_of_frames());
        break;
    }
}


void czynnosc( int com, int tog ) //funkcja odpowiedzialna za wywołanie odpowiedniej czynności (pierwszy argument musi być przez wskaźnik, ponieważ, może być dokonana zmiana zmiennej "menu")
{
    buzzer();

    switch( menu )//warianty w zależności od zmiennej menu, na końcu każdego wywoływana jest funkcja wyświetlająca
    {

    case 0:
        switch ( com )
        {
        case 41:
            pilot_off();
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
            pilot_on();

            break;
        case 12:
            pilot_off();
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
            pilot_on();
            break;
        }
        wysw();
        break;
    case 1:
        LCD_Displaying( com );
        if ( com != 100 ) wysw();
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
                wysw ();
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
                    cyfra = w;
                    wysw ();
                    buzzer_time(0.8);
                }
                --t;
            }
            t++;//przywrócenie efektu z ostatniej tury
            if( w < 7 )delay_ms_var_double((1+2500/t)/(7-w));//jeszcze jedno opóźnienie
            u = -1;//tryb wyświetlania
            wysw();
            //cy1 = 8;
            break;
        case 59:
            cyfra = 0;
            pozycja = 0;
            u = -1;
            wysw();
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
        wysw();
        break;
    case 3:
        switch ( com )
        {


        case 59:
            PCF8583_alarm_flag_off();
            break;
        case 100:
            if(pilot_state == 1)
            {
                pilot_state = 0;
            }
            else if(pilot_state == 0)
            {
                pilot_state = 1;
            }
            break;

        }
        wysw();
        break;


    case 4:
        switch ( com )
        {
        case 16:
            ++u;
            w = 1;//wymuszenie wyświetlenia komunikatu
            break;
        case 17:
            --u;
            w = 1;//wymuszenie wyświetlenia komunikatu
            break;
        case 32://zwiększenie
            s = 1;
            break;
        case 33://zmniejszenie
            s = 2;
            break;
        case 59:
            u = 8;
            break;
        }
        wysw();
        break;

    case 5:
        switch ( com )
        {
        case 16:
            ++u;
            w = 1;//wymuszenie wyświetlenia komunikatu
            break;
        case 17:
            --u;
            w = 1;//wymuszenie wyświetlenia komunikatu
            break;
        case 32://zwiększenie
            if(u != 0) s = 1;
            u -= zwiekszanie;
            break;
        case 33://zmniejszenie
            if(u != 0) s = 2;
            u += zwiekszanie;
            break;
        case 59:
            u = 8;
            break;
        }
        wysw();
        break;

    case 6:
        switch ( com )
        {
        case 12:

            switch(tog)
            {
            case 0:
                break;
            case 1:
                EEPROM_clear_all_memory();
                PCF8583_write(PCF8583_CELL, 0);
                u = 0;
                break;
            }
            break;
        case 32:
            u -= zwiekszanie;
            break;
        case 33:
            u += zwiekszanie;
            break;
        }

        wysw();
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
        wysw();
        break;
    case 46:
        LCD_ShiftRightScreen();
        break;
    case 34:
        LCD_ShiftLeftScreen();
        break;
    case 36:
        LCD_PageUpScreen();
        break;
    case 35:
        LCD_PageDownScreen();
        break;
    case 44:
        step_increase();
        break;
    case 45:
        step_decrease();
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

void pilot( int com, int tog )//
{
    if(pilot_state == 1) pilot_off();
    if( menu == 0 )//jeśli wyszliśmy z podprogramu lub weszliśmy do podprogramu
    {
//ważne opcje przy wchodzeniu/wychodzeniu z podprogramów

        if( com > 0 && com <= liczbaPodprogramow )//jeśli komenda była z zakresu numerów podprogramów
        {
            menu = com;//przypisanie zmiennej menu nowej wartości
            wybor( menu );

            switch( menu )//można podać tu komendy które mają wykonać się podczas wchodzenia do podprogramu
            {
            case 1:
                wysw();//niepotrzebne, gdy mają być wywoływane jakieś przyciski
                break;

            case 2:
                rozmiar = 4;
                u = -1;//wymuszenie wykonania animacji z kreskami
                t = 0;
                TCCR0 |= ( 1 << CS02 ) | ( 1 << CS00 ); // timer włączony od wyświetlacza alfanumerycznego
                wysw();//niepotrzebne, gdy mają być wywoływane jakieś przyciski
                break;

            case 3:
                //czynnosc( men, 50, tog );
                lockers_beginning_actions();
                TCCR2 |= ( 1 << CS20 ) | ( 1 << CS21 ) | ( 1 << CS22 ); // preskaler 1024, timer do odświeżania
                checking_lockers_state = 1;
                u = PCF8583_recognise_type_of_alarm();
                wysw();//niepotrzebne, gdy mają być wywoływane jakieś przyciski
                break;
            case 4:
                PCF8583_get_wall_time();
                u = 0;
                w = 1;//wymuszenie wyświetlenia komunikatu
                s = 0;
                czynnosc( 52, tog );
                //wysw( *men );//niepotrzebne, gdy mają być wywoływane jakieś przyciski
                break;
            case 5:
                //czynnosc( men, 50, tog );
                c = PCF8583_recognise_type_of_alarm();
                wysw();//niepotrzebne, gdy mają być wywoływane jakieś przyciski
                break;
            case 6:
                //czynnosc( men, 50, tog );
                lockers_find_latest_data();
                u = lockers_convert_address_to_index_of_frame(PCF8583_read(PCF8583_CELL));
                wysw();//niepotrzebne, gdy mają być wywoływane jakieś przyciski
                break;
            }
        }
    }

    if( !start ) czynnosc( com, tog );//jeśli jest się już w menu głównym, a nie idzie się właśnie do jakiegoś podprogramu

    if ( start == 1 )//jeśli było się w jakimś podprogramie i właśnie przechodzimy do podprogramu głównego
    {
        start = 0;//informacja, że zaraz będziemy "chwilę" w menu głównym
        menu = 0;//
        zwiekszanie = 1;
        checking_lockers_state = 0;
        TCCR0 &= ~( ( 1 << CS02 ) | ( 1 << CS00 ) ); // timer 0 od wyświetlacza alfanumerycznego wyłączony
        TCCR2 &= ~( 1 << CS20 ) | ( 1 << CS21 ) | ( 1 << CS22 ); // preskaler 1024, timer do odświeżania
        wybor( menu );
        wysw ();// wyświetlenie ekranu

    }
    if(pilot_state == 1) pilot_on();
}

// funkcja odpowiedzialna za odczytanie komend z pilota i przekazaniu ich do fukcji pilot, dopóki nie zostaną wykonane wszystkie rozkazy, nie będzie można odzczytać innego przysisku

void zczytaj_komende( void )
{
    if( interr && (menu == 3))
    {
        wysw();
        interr = 0;
        cnt = 0;
    }

    if ( stop_button())
    {
        delay_ms_var_double(30);
        if (stop_button()) pilot( 100, 0 );//wywołanie funkcji pilot przez naciśnięcie przycisku
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

            pilot( command, t );//wywołanie funkcji pilot
            Ir_key_press_flag = 0;
            command = 0xff;
            address = 0xff;
        }
    }
}


//Koniec definicji metod


//Program glowny:





int main( void )
{

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
    PCF8583_alarm_weekly();
    PCF8583_set_weekly_alarm(0b11110101,5,30,45,25);
    //PCF8583_alarm_monthly();

    sei();//włącza przerwania

    pilot( 0, 0 );//rozpoczęcie programu od głównego menu - konieczny krok
    pilot( 3, 0 );//przejście do podprogramu nr 3

    pilot_on();
    pilot_state = 1;

    //główna pętla programu

    while( 1 )
    {
        zczytaj_komende();
        if( checking_lockers_state == 1) lockers_check_events();
    }

    return 0;
}
