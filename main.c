//Pomiar napiêcia przetwornikiem A/C i prezentacja wyniku na LCD 2x16 HD44780

#include "main.h"

//UWAGA!!! PONIŻSZE CHARAKTERYZACJE ZMIENNYCH SĄ BARDZO ISTOTNE W CELU POPRAWNEGO ICH UŻYWANIA W PROGRAMIE

//zmienne zarezerwowane - nie można ich używać do innych celów niż wskazane
//zmienne zarezerwowane globalnie dla całego programu
int8_t switch_menu = 0;//zmienna służąca do wchodzenia do poszczególnych podprogramów
const int liczbaPodprogramow = 7;
uint8_t refresh_screen = 0;
uint8_t menu = 0;// zmienna odpowiedzialna za przebywanie w danym podprogramie
int8_t start = 1; // zmienna pomocna do stwierdzenia, czy jest się już w glownym menu = 0, czy właśnie wyszło się z podprogramu i trzeba np. zatrzymać jakiś timer = 1
int8_t toggle = 2;//zmienna odpowiedzialna za świadomość dłuższego przytrzymania przycisku pilota (wartość 2 jest wartością początkową w celu późniejszego skalibrowania ze stanem pilota)
uint8_t moveStep = 0;//zmienna do przesunięcia wyświetlanych partii danych (dla daty)
uint8_t pilot_state = 0;//zmienna odpowiedzialna za działanie, bądź niedziałanie timera od odczytu pilota
int8_t backlight_of_lcd = 0;
//uint8_t checking_lockers_state = 0;//zmienna odpowiedzialna za sprawdzanie stanów wejść
//zmienne zarezerwowane dla podprogramu nr 2:
uint8_t pozycja = 0;//zminna dodatkowa (pomocnicza) pamiętająca wylosowaną pozycję cyfry na wyświetlaczu alfanumerycznym
int8_t	cyfra = 0; // zmienna przechowująca wartość wyświetlaną póżniej na wyświetlaczu alfanumerycznym


//definicje funkcji

double round_double(float number, uint8_t precision)
{
    uint32_t ten = 1;
    for(t = 0; t < precision; ++ t)
    {
        ten *= 10;
    }
    double value = number * ten;

    double val1;
    if (value < 0.0)
        val1 = value - 0.5;
    else
        val1 = value + 0.5;
    return ((int32_t)val1) / (double)ten;
}


uint8_t number_of_digits(int32_t number)
{
    int32_t temp_number = abs(number);
    uint32_t ten2 = 10;

    int d = 1;
    while ( temp_number >= ten2 )
    {
        d += 1;
        ten2 *= 10;
    }
    if (number < 0) ++d;
    return d;
}

void show_double(double number, uint8_t approximation)
{
    struct double_format temp_double_format = set_double_format(number, approximation);
    LCD_Int (temp_double_format.integer_number);
    LCD_WriteText(".");
    for(t = 0; t < approximation - number_of_digits(temp_double_format.decimal_number); ++t) LCD_WriteText("0");
    LCD_Int (temp_double_format.decimal_number);
}

double get_double_form_double_format(struct double_format temp_double_format)//funkcja konwertująca strukturę do doubli na zmienną typu double
{
    return temp_double_format.integer_number + temp_double_format.decimal_number / 100.0;
}

struct double_format set_double_format( double value, uint8_t approximation)
{
    value = round_double(value,approximation);
    struct double_format double_format_temp;
    if((int16_t) abs(value) > 300)
    {
        double_format_temp.integer_number = 0;
        double_format_temp.decimal_number = 0;
        return double_format_temp;
    }

    if(approximation > 2) approximation = 2;
    double_format_temp.integer_number = (int16_t)value;

    uint16_t value_temp = abs(value);
    uint16_t ten = 1;
    //uint8_t d  = 0;
    for(t = 0; t < approximation; ++t)
    {
        ten *= 10;
    }
    value *= ten;//liczba przesunięta o liczbę miejsc
    //printf("%lf, ", value);
    value = abs(value);//liczba na pewno jest dodatnia
    //printf("%lf\n", value);

    value -= ten * value_temp;//usuniecie czesci dużej liczby, zostaje liczba po przecinku, tylko w formie całkowitej
    //if(approximation)//wyświetlanie liczb po przecinku
    //{


    //d = number_of_digits((uint16_t)value);//liczba cyfr powstałej liczby

    //uint8_t f = number_of_digits(ten);//liczba cyfr

    /*for(a = 1; (int16_t)a < (int16_t)(f-d); ++a)
    {
        LCD_Int(0);
    }*/

    /*ten = 1;

    for(t = 0; t < approximation - d - 1 ; ++t)
    {
        ten *= 10;
    }*/

    double_format_temp.decimal_number = (uint8_t) value /* * (ten)*/;

    return double_format_temp;

    //LCD_Int((uint16_t)value);
    //}
}

void change_color_RGB(void)
{
    static uint8_t temp = 0;
    static uint8_t left_right = 0;
    static uint8_t inversion = 0;
    temp -= 64;
    //RGB_Red = temp;
    RGB_Red = temp + 128;
    //RGB_Blue = temp;

    if(inversion == 0)
    {
        RGB_Green = temp + 64;
        RGB_Blue = temp;
    }
    else
    {
        RGB_Green = temp;
        RGB_Blue = temp + 64;
    }

    if(temp == 128)
    {
        if(left_right == 0)left_right = 1;
        else if(left_right == 1) left_right = 2;

        if(left_right == 1)
        {
            if(inversion == 0)RGB_Green = 0;
            else RGB_Blue = 0;
        }
        else if(left_right == 2)
        {
            if(inversion == 0)RGB_Blue = 0;
            else RGB_Green = 0;
        }

        if(left_right == 2)
        {
            left_right = 3;
        }
        else if(left_right == 3)
        {
            left_right = 0;
            if(inversion == 0) inversion = 1;
            else inversion = 0;
        }
    }
    //printf("%d,%d,%d\n",RGB_Red,RGB_Green,RGB_Blue);
}

void all_colors_RGB(void)
{
    RGB_Red = 255;
    RGB_Green = 255;
    RGB_Blue = 255;
    //printf("%d,%d,%d\n",RGB_Red,RGB_Green,RGB_Blue);
}

void no_colors_RGB(void)
{
    RGB_Red = 0;
    RGB_Green = 0;
    RGB_Blue = 0;
    //printf("%d,%d,%d\n",RGB_Red,RGB_Green,RGB_Blue);
}

void backlight(int8_t state)
{
    if(state == 0) backlight_of_lcd = 0;
    else
    {
        LCD_BacklightOn();

        if(state == 1)
        {
            buzzer_time(3);
            backlight_of_lcd = -1;
        }
        else if(state == 2 && backlight_of_lcd != -1) backlight_of_lcd = 50;
    }
}

void buzzer()//funkcja odpowiedzialna za sygnał dźwiękowy (trwa jedną milisekundę)
{
    BUZZER_PORT |= BUZZER;
    delay_ms_var_double( 1 );
    BUZZER_PORT &= ~BUZZER;
}

void buzzer_time( double time )//funkcja odpowiedzialna za sygnał dźwiękowy (trwa podaną liczbę milisekund)
{
    BUZZER_PORT |= BUZZER;
    delay_ms_var_double( time );
    BUZZER_PORT &= ~BUZZER;
}

void wysw_skok( uint16_t number ) // funkcja wyświetlająca numer kroku o danej wartości
{
    uint8_t d = number_of_digits(number);

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
    pilot_reset();
    refresh_screen = 1;
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
    refresh_screen = 0;
    if(start_program != 1)
    {
        LCD_Clear();
        LCD_WriteText( "Program: " );
        LCD_Int( number );
        LCD_GoTo(0,1);
        if(number == 0) LCD_WriteText("PROGRAM GLOWNY");
        else if(number == 1) LCD_WriteText("PROGRAM TESTOWY");
        else if(number == 2) LCD_WriteText("CZUWANIE");
        else if(number == 3) LCD_WriteText("DANE - EEPROM");
        else if(number == 4) LCD_WriteText("NASTAWA GODZINY");
        else if(number == 5) LCD_WriteText("NASTAWA ALARMU");
        else if(number == 6) LCD_WriteText("USTAWIENIA FLAG");
        else if(number == 7) LCD_WriteText("TEMP. KRYTYCZNA");

        for ( t = 0; t < 5; ++t )
        {
            delay_ms_var_double( 30 );
            buzzer_time(5);
        }
        delay_ms_var_double( 500 );
        LCD_PageUpScreen();
        LCD_Clear();

    }
    pilot_reset();
    refresh_screen = 1;
}

void show_day_of_week( uint8_t day)
{
    if(day == 0) LCD_WriteText("Pn");
    else if(day == 1) LCD_WriteText("Wt");
    else if(day == 2) LCD_WriteText("Sr");
    else if(day == 3) LCD_WriteText("Cz");
    else if(day == 4) LCD_WriteText("Pt");
    else if(day == 5) LCD_WriteText("So");
    else if(day == 6) LCD_WriteText("Nd");
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

    LCD_GoTo(moveStep, 1 );
    if(dzien < 10) LCD_Int(0);
    LCD_Int(dzien);
    LCD_WriteText(":");
    if(miesiac < 10) LCD_Int(0);
    LCD_Int(miesiac);
    LCD_WriteText(":");
    for(t = 0; t < 4 - number_of_digits(rok); ++t)
    {
        LCD_WriteText("_");
    }
    LCD_Int(rok);
    LCD_WriteText("|");
    show_day_of_week(dzien_tygodnia);
    LCD_WriteText("|");
    if(timer < 10) LCD_Int(0);
    LCD_Int(timer);
}

void show_timer_alarm_format(void)
{
    LCD_GoTo(moveStep, 0 );
    if(timer < 10) LCD_Int(0);
    LCD_Int(timer);
}

void show_alarm_format(uint8_t case_of_format)
{
    if(case_of_format != 0) show_time_only_format();
    LCD_GoTo(moveStep, 1);
    if(case_of_format == 2)
    {
        if(miesiac == 0)
        {
            LCD_WriteText("-------");
        }
        else
        {
            for(t = 0; t < 7; ++t)
            {
                if(miesiac & (1 << t))
                {
                    LCD_WriteText("|");
                    LCD_Int(t+1);
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

void setting_information()
{
    refresh_screen = 0;
    LCD_EraseAll();
    LCD_GoTo(0, 0);

    if(u == -1)
    {
        LCD_WriteText("WYBIERZ STAN:");
    }
    else
    {
        if(menu == 6)
        {
            if(u == end_of_settings())
            {
                LCD_WriteText("ZAPISANO");
                /*if(e == 0)
                {
                    if(c == 0) LCD_WriteText("ALARM F. WYL");
                    else if(c == 1) LCD_WriteText("ALARM F. WL.");
                }
                else if(e == 1)
                {
                    if(c == 0) LCD_WriteText("TIMER F. WYL");
                    else if( c == 1) LCD_WriteText("TIMER F. WL");
                }
                else if(e == 2)
                {
                    if(c == 0) LCD_WriteText("ODLICZANIE CZASU");
                    else if(c == 1) LCD_WriteText("ZEGAR STOP");
                }
                else if(e == 3)
                {
                    if(c == 0) LCD_WriteText("")
                }*/
            }
        }
        else
        {
            if( e == 0 || menu == 4)
            {
                if(u == end_of_settings())
                {
                    if(c == 0)
                    {
                        if(menu == 4)LCD_WriteText("ZAPISANO GODZINE");
                        else if( menu == 5 ) LCD_WriteText("WYLACZONO ALARM");
                    }
                    else LCD_WriteText("ZAPISANO ALARM");
                }
                else
                {
                    if(u == 0) LCD_WriteText("GODZINY");
                    else if(u == 1) LCD_WriteText("MINUTY");
                    else if(u == 2) LCD_WriteText("SEKUNDY");
                    else if(u == 3) LCD_WriteText("SETNE SEKUND");

                    if(c == 0 || c == 3)
                    {
                        if(u == 4) LCD_WriteText("DZIEN");
                        else if(u == 5) LCD_WriteText("MIESIAC");
                        if(c == 0)
                        {
                            if(u == 6) LCD_WriteText("ROK");
                            else if(u == 7) LCD_WriteText("DZIEN TYGODNIA");
                            else if(u == 8) LCD_WriteText("TIMER");
                        }
                    }
                    else if( c == 2)
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
            }
            else if(e == 1)
            {
                if(u == end_of_settings())
                {
                    if(c == 0) LCD_WriteText("ALARM TIM. WYL.");
                    else if(c == 1) LCD_WriteText("ALARM TIM. WL.");
                }
                else
                {
                    if(u == 0) LCD_WriteText("TIMER");
                }
            }
        }
    }
    delay_ms_var(400);
    pilot_reset();
    refresh_screen = 1;
}

void set_appropriate_values_of_time()
{
    int16_t temp = 0;
    if(s == 2) temp -= zwiekszanie;
    else if (s == 1) temp = zwiekszanie;

    if(u == -2) e -= temp;
    else
    {
        if(menu == 6)
        {
            if(u == -1) c -= temp;
        }
        else
        {
            if(e == 0 || menu == 4)
            {
                if(u == -1) c -= temp;
                else
                {
                    if(u == 0) godz += temp;
                    else if(u == 1) min += temp;
                    else if(u == 2) sek += temp;
                    else if(u == 3) hsek += temp;

                    if(c == 0 || c == 3)
                    {
                        if(u == 4) dzien += temp;
                        else if(u == 5) miesiac += temp;
                        if(c == 0)
                        {
                            if(u == 6)rok += temp;
                            else if(u == 7) dzien_tygodnia += temp;
                            else if(u == 8) timer += temp;
                        }
                    }
                    else if( c == 2)
                    {
                        if(u == 4)
                        {
                            if(s == 1) miesiac |= (1 << 0);
                            else if(s == 2) miesiac &= ~(1 << 0);
                        }
                        else if(u == 5)
                        {
                            if(s == 1) miesiac |= (1 << 1);
                            else if(s == 2) miesiac &= ~(1 << 1);
                        }
                        else if(u == 6)
                        {
                            if(s == 1) miesiac |= (1 << 2);
                            else if(s == 2) miesiac &= ~(1 << 2);
                        }
                        else if(u == 7)
                        {
                            if(s == 1) miesiac |= (1 << 3);
                            else if(s == 2) miesiac &= ~(1 << 3);
                        }
                        else if(u == 8)
                        {
                            if(s == 1) miesiac |= (1 << 4);
                            else if(s == 2) miesiac &= ~(1 << 4);
                        }
                        else if(u == 9)
                        {
                            if(s == 1) miesiac |= (1 << 5);
                            else if(s == 2) miesiac &= ~(1 << 5);
                        }
                        else if(u == 10)
                        {
                            if(s == 1) miesiac |= (1 << 6);
                            else if(s == 2) miesiac &= ~(1 << 6);
                        }
                    }
                }
            }

            else if(e == 1)
            {
                if(u == -1) c -= temp;
                else
                {
                    if(u == 0) timer += temp;
                }
            }
        }
    }


}

void check_step_value(void)
{
    if((u == -2 || u == -1) && zwiekszanie > 1) wysw_skok(1);

    if(e == 0 || menu == 4)
    {
        if((u == 0 || u == 1 || u == 2 || u == 3 ) && zwiekszanie > 10) wysw_skok(10);

        if(c == 0 || c == 3)
        {
            if(( u == 4 || u == 5 )&& zwiekszanie > 10) wysw_skok(10);
            if(c == 0)
            {
                if(u == 7 && zwiekszanie > 1) wysw_skok(1);
                if(u == 8 && zwiekszanie > 10) wysw_skok(10);
            }
        }
    }
    else if(e == 1)
    {
        if(u == 0 && zwiekszanie > 10) wysw_skok(10);
    }
}

uint8_t end_of_settings(void)
{
    if(menu == 4)
    {
        return 9;
    }
    else if(menu == 5)
    {
        if(e == 0)
        {
            if(c == 0) return 0;
            else if(c == 1) return 4;
            else if(c == 2) return 11;
            else if(c == 3) return 6;
        }
        else if(e == 1)
        {
            if(c == 0) return 0;
            else if(c == 1) return 1;
        }
    }
    else if(menu == 6) return 0;
    return 0;
}

void show_setting_alarm_case(uint8_t index)
{
    if(index == 0)
    {
        LCD_WriteText("Ustaw alarm");
    }
    else if(index == 1)
    {
        LCD_WriteText("Ustaw alarm tim.");
    }
}

void show_alarm_flag_options(uint8_t index)
{
    if(index == 0)
    {
        LCD_WriteText("Flaga alarmu OFF");
    }
    else if(index == 1)
    {
        LCD_WriteText("Flaga alarmu ON");
    }
}

void show_timer_flag_options(uint8_t index)
{
    if(index == 0)
    {
        LCD_WriteText("Flaga tim. OFF");
    }
    else if(index == 1)
    {
        LCD_WriteText("Flaga tim. ON");
    }
}

void show_clock_options(uint8_t index)
{
    if(index == 0)
    {
        LCD_WriteText("Rezonator OFF");
    }
    else if(index == 1)
    {
        LCD_WriteText("Rezonator ON");
    }
}

void show_timer_mode_options(uint8_t index)
{
    if(index == 0)
    {
        LCD_WriteText("Timer stop");
    }
    else if(index == 1)
    {
        LCD_WriteText("Timer s. sekund");
    }
    else if(index == 2)
    {
        LCD_WriteText("Timer sekund");
    }
    else if(index == 3)
    {
        LCD_WriteText("Timer minut");
    }
    else if(index == 4)
    {
        LCD_WriteText("Timer godzin");
    }
    else if(index == 5)
    {
        LCD_WriteText("Timer dni");
    }
    else if(index == 6)
    {
        LCD_WriteText("Timer test");
    }
}

void show_alarm_interrupt_options(uint8_t index)
{
    if(index == 0)
    {
        LCD_WriteText("Alarm int. OFF");
    }
    else if(index == 1)
    {
        LCD_WriteText("Alarm int. ON");
    }
}

void show_timer_interrupt_options(uint8_t index)
{
    if(index == 0)
    {
        LCD_WriteText("Timer int. OFF");
    }
    else if(index == 1)
    {
        LCD_WriteText("Timer int. ON");
    }
}

void show_setting_flags_case(uint8_t index)
{
    if(index == 0)
    {
        LCD_WriteText("Flaga alarmu");
    }
    else if(index == 1)
    {
        LCD_WriteText("Flaga timera");
    }
    else if(index == 2)
    {
        LCD_WriteText("Rezonator");
    }
    else if(index == 3)
    {
        LCD_WriteText("Tryby Timera");
    }
    else if(index == 4)
    {
        LCD_WriteText("Przerw. alarmu");
    }
    else if(index == 5)
    {
        LCD_WriteText("Przerw. timera");
    }
}

void show_timer_options(uint8_t index)
{
    if(index == 0)
    {
        LCD_WriteText("Alarm Tim. OFF");
    }
    else if(index == 1)
    {
        LCD_WriteText("Alarm Tim. ON");
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

void correction_of_date(void)//uwzględnianie dnia miesiąca względem roku
{
    if(miesiac < 1 && c != 2) miesiac = 12;
    else if(miesiac > 12 && c != 2) miesiac = 1;

    uint8_t case_of_day = 0;
    if(miesiac == 1 || miesiac == 3 || miesiac == 5 || miesiac == 7 || miesiac == 8 || miesiac == 10 || miesiac == 12) case_of_day = 31;
    else if (miesiac == 4 || miesiac == 6 || miesiac == 9 || miesiac == 11) case_of_day = 30;
    else if (miesiac == 2 && (rok % 4) != 0 && c == 0) case_of_day = 28;
    else case_of_day = 29;

    if(dzien < 1) dzien = case_of_day;
    else if(dzien > case_of_day) dzien = 1;

    if(rok < -999) rok = 9999;
    else if(rok > 9999) rok = -999;

    if(dzien_tygodnia < 0) dzien_tygodnia = 6;
    else if(dzien_tygodnia > 6) dzien_tygodnia = 0;

    if(timer < 0) timer = 99;
    else if(timer > 99) timer = 0;
}

void show_frame( int16_t number )
{
    LCD_Int(number);
    LCD_WriteText(".");
    for(t = 0; t < 4 - number_of_digits(frame.year); ++t)
    {
        LCD_WriteText("_");
    }
    LCD_Int(frame.year);
    LCD_WriteText(":");
    if(frame.month < 10) LCD_Int(0);
    LCD_Int(frame.month);
    LCD_WriteText(":");
    if(frame.day < 10) LCD_Int(0);
    LCD_Int(frame.day);
    LCD_WriteText(" ");

    if(frame.hours < 10) LCD_Int(0);
    LCD_Int(frame.hours);
    LCD_WriteText(":");
    if(frame.minutes < 10) LCD_Int(0);
    LCD_Int(frame.minutes);
    LCD_WriteText(":");
    if(frame.seconds < 10) LCD_Int(0);
    LCD_Int(frame.seconds);

    LCD_WriteText(" ");

    number = frame.information % 100;
    if(number != 0)
    {
        LCD_WriteText("NR ");
        LCD_Int(number);
        LCD_WriteText(" ");
        t = frame.information / 100;
        if(t == 1) LCD_WriteText("OTWARCIE");
        else if(t == 2) LCD_WriteText("ZAMKNIECIE");
    }
}

void show_list_case(index)
{
    if(menu == 3)
    {
        lockers_queue_read(index);
        show_frame(index+1);
    }
    else if (menu == 5)
    {
        if(u == -2)
        {
            show_setting_alarm_case(index);
        }
        else
        {
            if(e == 0)
            {
                if(u == -1)
                {
                    show_alarm_options(index);
                }
            }
            else if(e == 1)
            {
                if(u == -1)
                {
                    show_timer_options(index);
                }
            }
        }
    }
    else if(menu == 6)
    {
        if(u == -2)
        {
            show_setting_flags_case(index);
        }
        else
        {
            if(u == -1)
            {
                if(e == 0)
                {
                    show_alarm_flag_options(index);
                }
                else if(e == 1)
                {
                    show_timer_flag_options(index);
                }
                else if(e == 2)
                {
                    show_clock_options(index);
                }
                else if(e == 3)
                {
                    show_timer_mode_options(index);
                }
                else if(e == 4)
                {
                    show_alarm_interrupt_options(index);
                }
                else if(e == 5)
                {
                    show_timer_interrupt_options(index);
                }
            }
        }
    }
}


void show_list(int16_t current_index, int16_t max_index)
{
    if(max_index == -1)
    {
        LCD_GoTo(0, 0);
        LCD_WriteText("Brak danych");
    }
    else
    {
        if ( current_index != -1)
        {
            LCD_GoTo(0, 0);
            show_list_case(current_index);
        }

        if(current_index != max_index)
        {
            LCD_GoTo(0, 1);
            show_list_case(current_index + 1);
        }
    }
}

void wysw0( void )// funkcja wyświetlająca - interfejs dla każdego z podprogramów
{
    if(zwiekszanie > 1) wysw_skok(1);
    if (switch_menu > liczbaPodprogramow) switch_menu = 0;
    else if (switch_menu < 0) switch_menu = liczbaPodprogramow;
    LCD_EraseAll();
    LCD_GoTo( 0, 0 );
    LCD_WriteText( "Wybierz program:" );
    LCD_GoTo( 4, 1 );
    LCD_WriteText(">>>");
    LCD_Int(switch_menu);
    LCD_WriteText("<<<");

    //LCDWriteToBuffer( 0, 0, "napis" );
    //LCD_WriteText ( "Pierwszy Napis abcdefghijklmnopqrstuvwxyz" );
    //LCD_WriteText ( "Drugi" );
}

void wysw1( void )// funkcja wyświetlająca - interfejs dla każdego z podprogramów
{
    LCD_EraseAll();
    LCD_GoTo( 0, 0 );
    LCD_Int( command );
    LCD_GoTo( 6, 0 );
    LCD_Int( address );
    LCD_GoTo( 0, 1 );
    LCD_Int( toggle_bit );
}

void wysw2( void )// funkcja wyświetlająca - interfejs dla każdego z podprogramów
{
    moveStep=0;
    PCF8583_get_wall_time();
    LCD_EraseAll();
    LCD_GoTo(11,0);
    if(pilot_state == 0) LCD_WriteText("!");
    else if(pilot_state == 1) LCD_WriteText("|");

    show_time_format();

    if(u != 0 || w != 0) PCF8583_get_wall_alarm();

    if(u != 0)
    {
        moveStep=19;
        LCD_GoTo(moveStep - 2, 0);
        LCD_WriteText("|");

        LCD_GoTo(moveStep - 2, 1);
        LCD_WriteText("|");
        LCD_GoTo(moveStep, 1 );
        show_alarm_format(u);
    }

    if(w != 0)
    {
        moveStep = 37;
        LCD_GoTo(moveStep - 2, 0);
        LCD_WriteText("|");

        LCD_GoTo(moveStep - 2, 1);
        LCD_WriteText("|");
        show_timer_alarm_format();
    }

    LCD_GoTo(12, 0);
    show_double(termometer_temperature,1);

    /*LCD_Double(-23.301,2);
    LCD_Double(-23.3015,2);
    LCD_Double(-299.9015,4);
    LCD_Double(-299.901,3);
    LCD_Double(299.901,3);
    LCD_Double(200.324,3);*/


    //LCD_Int( pwm1 );
    //LCD_Int( pwm2 );
    //OCR0 = pwm1;//zmienna przepełnienia Timera 0
}

void wysw3( void )// funkcja wyświetlająca - interfejs dla każdego z podprogramów
{
    LCD_EraseAll();
    if(zwiekszanie > 10) wysw_skok(10);

    if(c < -1) c = lockers_queue_number_of_records() - 1;
    else if(c > lockers_queue_number_of_records() - 1) c = -1;
    show_list(c, lockers_queue_number_of_records() -1);
}

void wysw4( void )// funkcja wyświetlająca - interfejs dla każdego z podprogramów
{
    if (u < 0) u = 0;
    check_step_value();
    if( s != 0 )
    {
        set_appropriate_values_of_time();

        s = 0;
    }
    LCD_EraseAll();

    if(u == end_of_settings())
    {
        PCF8583_set_time(godz,min,sek,hsek,dzien,dzien_tygodnia,miesiac,rok,timer,rano_wieczor);
        refresh_screen = 0;
        LCD_Clear();
        w = 1;
        start = 1;
    }
    else
    {
        correction_of_time();

        correction_of_date();

        moveStep = 0;
        show_time_format();
    }

    if( w == 1 )
    {
        setting_information();
        w = 0;
    }
}

void wysw5( void )// funkcja wyświetlająca - interfejs dla każdego z podprogramów
{
    if (u < -2) u = -2;
    check_step_value();

    if( s != 0 )
    {
        set_appropriate_values_of_time();
        s = 0;
    }

    if( w == 1)//zabezpieczenie przed wyborem niewłaściwej wartości z menu
    {
        if( (u == -1 && e < 0) || (u == 0 && c < 0))
        {
            u -= 1;
            w = 0;
        }
    }

    LCD_EraseAll();


    if(u == -2)
    {
        if(e < -1) e = 1;
        else if(e > 1) e = -1;

        PCF8583_get_wall_alarm();
        if(e == 0) c = PCF8583_recognise_type_of_alarm();//zmienna odpowiedzialna za typ alarmu
        else if( e == 1) c = PCF8583_recognise_type_of_timer_alarm();

        show_list(e, 1);
    }
    else
    {
        if(e == 0)
        {
            if(u == -1)
            {
                if(c < -1) c = 3;
                else if(c > 3) c = -1;
                show_list(c, 3);
            }
            else
            {
                if(u == end_of_settings() || c == 0)
                {
                    start = 1;
                    w = 1;

                    PCF8583_set_alarm_time(godz,min,sek,hsek,dzien,miesiac,timer,rano_wieczor);
                    PCF8583_set_type_of_alarm(c);

                    if(c == 0)
                    {
                        PCF8583_alarm_flag_off();
                    }
                }
                else
                {
                    correction_of_time();

                    correction_of_date();

                    moveStep = 0;
                    show_alarm_format(c);
                }
            }
        }
        else if( e == 1)
        {
            if(u == -1)
            {
                if(c < -1) c = 1;
                else if(c > 1) c = -1;
                show_list(c, 1);
            }
            else
            {
                if(u == end_of_settings() || c == 0)
                {
                    start = 1;
                    w = 1;

                    PCF8583_set_alarm_time(godz,min,sek,hsek,dzien,miesiac,timer,rano_wieczor);
                    if(c == 0)
                    {
                        PCF8583_timer_alarm_off();
                        PCF8583_timer_flag_off();
                    }
                    else if(c == 1)
                    {

                        PCF8583_timer_alarm_on();
                    }
                }
                else
                {
                    correction_of_date();
                    moveStep = 0;
                    show_timer_alarm_format();
                }
            }
        }
    }

    if( w == 1  && u >= -1)
    {
        setting_information();
        w = 0;
    }
}

void wysw6( void )// funkcja wyświetlająca - interfejs dla każdego z podprogramów
{
    if (u < -2) u = -2;
    check_step_value();//zrobic
    if( s != 0 )
    {
        set_appropriate_values_of_time();
        s = 0;
    }

    if( w == 1)//zabezpieczenie przed wyborem niewłaściwej wartości z menu
    {
        if( (u == -1 && e < 0) || (u == 0 && c < 0) )
        {
            u -= 1;
            w = 0;
        }
    }

    LCD_EraseAll();

    if(u == -2)
    {
        if(e < -1) e = 5;
        else if(e > 5) e = -1;
        if(e == 0) c = PCF8583_is_alarm_flag_set();//zmienna odpowiedzialna za typ alarmu
        else if(e == 1) c = PCF8583_is_timer_flag_set();
        else if(e == 2) c = PCF8583_is_clock_counting();
        else if(e == 3) c = PCF8583_timer_mode();
        else if(e == 4) c = PCF8583_is_alarm_interrupt();
        else if(e == 5) c = PCF8583_is_timer_interrupt();
        /*LCD_GoTo(0,0);
        LCD_WriteText("CZYTANIE: ");
        LCD_Int(e);
        LCD_WriteText(" ");
        LCD_Int(c);
        delay_ms_var(1000);*/
        show_list(e, 5);
    }
    else if(u == -1)
    {
        if(e == 0 || e == 1 || e == 2 || e == 4 || e == 5)
        {
            if(c < -1) c = 1;
            else if(c > 1) c = -1;
            show_list(c, 1);
        }
        else if(e == 3)
        {
            if(c < -1) c = 6;
            else if(c > 6) c = -1;
            show_list(c, 6);
        }
    }
    else
    {
        if(u == end_of_settings())
        {
            start = 1;
            w = 1;

            if(e == 0)
            {
                if(c == 0) PCF8583_alarm_flag_off();
                else if(c == 1) PCF8583_alarm_flag_on();
            }
            else if(e == 1)
            {
                if(c == 0) PCF8583_timer_flag_off();
                else if(c == 1) PCF8583_timer_flag_on();
            }
            else if(e == 2)
            {
                if(c == 0) PCF8583_stop();
                else if(c == 1) PCF8583_start();
            }
            else if(e == 3)
            {
                PCF8583_set_timer_mode(c);
            }
            else if(e == 4)
            {
                if(c == 0) PCF8583_alarm_interrupt_off();
                else if(c == 1) PCF8583_alarm_interrupt_on();
            }
            else if(e == 5)
            {
                if(c == 0) PCF8583_timer_interrupt_off();
                else if(c == 1) PCF8583_timer_interrupt_on();
            }
        }
    }

    if( w == 1  && u >= -1)
    {
        setting_information();
        w = 0;
    }
}

void wysw7( void )// funkcja wyświetlająca - interfejs dla każdego z podprogramów
{
    LCD_EraseAll();
}

void wysw( void ) // funkcja wyświetlająca - interfejs dla każdego z podprogramów
{
    if(start_program == 3)
    {
        if(lockers_is_safety_bit() == 1)
        {
            LCD_EraseAll();
            LCD_WriteText("USB - Potwierdz");
            LCD_GoTo(0, 1);
            LCD_WriteText("przechwytywanie");
        }
        else if(lockers_is_safety_bit() == 0)
        {
            LCD_EraseAll();
            LCD_WriteText("RESTART");
        }
    }
    else
    {
        if( menu == 0 )
        {
            wysw0();
        }
        else if( menu == 1 )
        {
            wysw1();
        }
        else if( menu == 2 )
        {
            wysw2();
        }
        else if( menu == 3 )
        {
            wysw3();
        }
        else if( menu == 4 )
        {
            wysw4();
        }
        else if( menu == 5 )
        {
            wysw5();

        }
        else if( menu == 6 )
        {
            wysw6();
        }
        else if( menu == 7 )
        {
            wysw7();
        }
    }
}

void czynnosc0( int com, int tog )
{
    //ważne opcje przy wchodzeniu/wychodzeniu z podprogramów

    if( com == 59 )//wybieramy środkowy przycisk
    {
        menu = switch_menu;//przypisanie zmiennej menu nowej wartości
        wybor( menu );

        if ( menu == 1 )
        {
            refresh_screen = 1;//niepotrzebne, gdy mają być wywoływane jakieś przyciski
        }
        else if ( menu == 2 )
        {
            //TCCR2 |= ( 1 << CS20 ) | ( 1 << CS21 ) | ( 1 << CS22 ); // preskaler 1024, timer do odświeżania
//                checking_lockers_state = 1;

            u = PCF8583_recognise_type_of_alarm();
            w = PCF8583_recognise_type_of_timer_alarm();
            //czynnosc( 100, tog );
            refresh_screen = 1;//niepotrzebne, gdy mają być wywoływane jakieś przyciski
        }
        else if ( menu == 3 )
        {
            //czynnosc( men, 50, tog );
            //lockers_find_latest_data();
            c = 0;
            refresh_screen = 1;//niepotrzebne, gdy mają być wywoływane jakieś przyciski

        }
        else if ( menu == 4 )
        {
            u = 0;
            c = 0;
            w = 1;//wymuszenie wyświetlenia komunikatu
            s = 0;
            PCF8583_get_wall_time();
            //czynnosc( 52, tog );
            refresh_screen = 1;//niepotrzebne, gdy mają być wywoływane jakieś przyciski
        }
        else if ( menu == 5 )
        {
            //czynnosc( men, 50, tog );
            u = -2;//przechodzenie przez poziomy w prawo w lewo
            w = 1;//wymuszenie wyświetlenia komunikatu
            s = 0;//
            e = 0;//zmienna odpowiedzialna za wybór ustawiania albo alarmu alarmu albo alarmu timera
            //d = 0;
            c = 0;//zmienna odpowiedzialna za typ alarmu
            refresh_screen = 1;//niepotrzebne, gdy mają być wywoływane jakieś przyciski
        }
        else if ( menu == 6 )
        {
            u = -2;//przechodzenie przez poziomy w prawo w lewo
            w = 1;//wymuszenie wyświetlenia komunikatu
            s = 0;//
            e = 0;//zmienna odpowiedzialna za wybór ustawiania albo alarmu alarmu albo alarmu timera
            //d = 0;
            c = 0;//zmienna odpowiedzialna za typ alarmu
            refresh_screen = 1;
        }
    }

    if( com == 32 )
    {
        switch_menu += zwiekszanie;
    }
    if( com == 33 )
    {
        switch_menu -= zwiekszanie;
    }

    refresh_screen = 1;
}

void czynnosc1( int com, int tog )
{
    if ( com == 41 )
    {
        refresh_screen = 0;
        LCD_Clear();
        rozmiar = LCD_CHARSPERLINE;
        //char original_text_static[2][40] = { {"ATmega32 programabcdefghijklmnopqrstuvwx"}, {"Dariusz M. proj.yz1234567890987654321!@$"}};

        char** original_text = ( char** ) malloc( 2 * sizeof (*original_text) );//tablica dwuwymiarowa z testowym napisem

        for ( t = 0; t < 2; ++t )
        {
            original_text[t] = ( char* ) malloc ( LCD_CHARSPERLINE * sizeof ( *original_text ) );
            for (u = 0; u < rozmiar; ++u)
            {
                original_text[t][u] = rand() % 256;
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

        u = 1;

        while( u )
        {
            //pilot_off();
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

            //pilot_on();

            if ( stop_button() || Ir_key_press_flag)//jeśli przycisk zatrzymania został wciśnięty
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

    }
    if ( com == 12 )
    {
        pilot_off();
        refresh_screen = 0;
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
    }
    if( com >= 0 && com <=  5 ) LCD_Displaying( com );
    wysw();
}

void czynnosc2( int com, int tog )
{
    /*if( com == 42 )
    {
        PCF8583_alarm_flag_on();
    }

    if( com == 41 )
    {
        PCF8583_timer_flag_on();
    }
    if ( com == 59 )
    {
        PCF8583_alarm_flag_off();
    }*/

    refresh_screen = 1;
}

void czynnosc3( int com, int tog )
{
    if ( com == 12 )
    {
        if( tog == 0)
        {
        }
        if( tog == 1)
        {
            LCD_Clear();
            LCD_WriteText("Czyszczenie...");
            lockers_clear_all_memory();
            //c = 0;
        }
    }
    if( com == 41 )
    {
        lockers_print_all_memory();
    }
    /*if( com == 55)
    {
        lockers_queue_dequeue();
    }*/
    if ( com == 32 )
    {
        c -= zwiekszanie;
    }
    if ( com == 33 )
    {
        c += zwiekszanie;
    }

    refresh_screen = 1;
}

void czynnosc4( int com, int tog )
{
    if ( com == 16 )
    {
        ++u;
        w = 1;//wymuszenie wyświetlenia komunikatu
    }
    if ( com == 17 )
    {
        --u;
        w = 1;//wymuszenie wyświetlenia komunikatu
    }
    if ( com == 32 )
    {
        s = 1;
    }
    if ( com == 33 )
    {
        s = 2;
    }
    if ( com == 59 )
    {
        u = end_of_settings();
    }
    refresh_screen = 1;
}

void czynnosc5( int com, int tog )
{
    if ( com == 16 )
    {
        ++u;
        w = 1;//wymuszenie wyświetlenia komunikatu
    }
    if ( com == 17 )
    {
        --u;
        w = 1;//wymuszenie wyświetlenia komunikatu
    }
    if ( com == 32 )
    {
        s = 1;
    }
    if ( com == 33 )
    {
        s = 2;
    }
    if ( com == 59 )
    {
        u = end_of_settings();
    }
    refresh_screen = 1;
}

void czynnosc6( int com, int tog )
{
    if ( com == 16 )
    {
        ++u;
        w = 1;//wymuszenie wyświetlenia komunikatu
    }
    if ( com == 17 )
    {
        --u;
        w = 1;//wymuszenie wyświetlenia komunikatu
    }
    if ( com == 32 )
    {
        s = 1;
    }
    if ( com == 33 )
    {
        s = 2;
    }
    if ( com == 59 )
    {
        u = end_of_settings();
    }
    refresh_screen = 1;
}

void czynnosc7( int com, int tog )
{

    if ( com == 59 )
    {
        u = end_of_settings();
    }
    refresh_screen = 1;
}

void czynnosc( int com, int tog ) //funkcja odpowiedzialna za wywołanie odpowiedniej czynności (pierwszy argument musi być przez wskaźnik, ponieważ, może być dokonana zmiana zmiennej "menu")
{
    buzzer();
    if( menu == 0)
    {
        czynnosc0(com, tog);
    }
    else if( menu == 1 )
    {
        czynnosc1(com, tog);
    }
    else if( menu == 2 )
    {
        czynnosc2(com, tog);
    }
    else if( menu == 3 )
    {
        czynnosc3(com, tog);
    }
    else if( menu == 4 )
    {
        czynnosc4(com, tog);
    }
    else if( menu == 5 )
    {
        czynnosc5(com, tog);
    }
    else if( menu == 6 )
    {
        czynnosc6(com, tog);
    }

//komendy wspólne dla wszystkich podprogramów

    if ( com == 38 )
    {
        refresh_screen = 0;
        LCD_Clear();
        LCD_WriteText( "Na poczatek" );
        LCD_GoTo( 0, 1 );
        LCD_WriteText( "ekranu..." );
        delay_ms_var_double( 250 );
        LCD_PageUpScreen();
        LCD_PageDownScreen();
        refresh_screen = 1;
    }
    if( com == 15 )
    {
        if( tog == 0)
        {
            backlight_of_lcd = 0;
            backlight(2);
        }
        if( tog == 1)
        {
            backlight(1);
        }
    }
    if ( com == 46 )
    {
        LCD_ShiftRightScreen();
    }
    if ( com == 34 )
    {
        LCD_ShiftLeftScreen();
    }
    if ( com == 36 )
    {
        LCD_PageUpScreen();
    }
    if ( com == 35 )
    {
        LCD_PageDownScreen();
    }
    if ( com == 44 )
    {
        step_increase();
    }
    if ( com == 45 )
    {
        step_decrease();
    }
    if ( com == 14 )
    {
        if( tog == 0)
        {
        }
        if( tog == 1)
        {
            start = 1;//oznaka wyjścia z podprogramów
        }
    }
    if ( com == 100 )
    {
        if(pilot_state == 1)
        {
            pilot_state = 0;
            buzzer_time(500);
            pilot_off();
            backlight(0);
        }
        else if(pilot_state == 0)
        {
            pilot_state = 1;
            pilot_on();
            backlight(2);
        }

    }
}

// funkcja obsługująca menu dwupoziomowe

void pilot( int com, int tog )//
{
//    if(pilot_state == 1) pilot_off();
    if(backlight_of_lcd >= 0) backlight(2);

    if( start_program == 3 && lockers_is_safety_bit() == 1)
    {
        start_program = 0;
        backlight(2);
    }

    if(start_program != 3) czynnosc( com, tog );//jeśli jest się już w menu głównym, a nie idzie się właśnie do jakiegoś podprogramu

    // if(pilot_state == 1) pilot_on();
}

// funkcja odpowiedzialna za odczytanie komend z pilota i przekazaniu ich do fukcji pilot, dopóki nie zostaną wykonane wszystkie rozkazy, nie będzie można odzczytać innego przysisku

void sczytaj_komende( void )
{
    if(refresh_screen == 1 )
    {
        refresh_screen = 0;
        wysw();
    }

    if( interr == 1 )
    {
        overflow_timer_2 = 0;
        interr = 0;

        uint8_t temp_char;

        static uint8_t temp = 0;

        if(start_program == 3 && temp == 0)
        {
            if(lockers_is_safety_bit() == 1)
            {
                buzzer();
                backlight(2);
                printf("#");

                temp_char = USART_Recieve_without_waiting();

                if(temp_char == 'r')
                {
                    printf("\nOczekiwanie na restart.");
                    lockers_safety_bit_off();
                    backlight(1);
                    temp = 1;
                }
            }
            else if(lockers_is_safety_bit() == 0)
            {
                lockers_safety_bit_on();
                start_program = 0;
            }
        }
        else if(temp == 1)
        {
            buzzer_time(100);
        }

        if(start_program == 0)
        {
            printf("\nPrzechwytywanie USB potwierdzone... ");
            lockers_print_date_of_report();

            if(lockers_is_queue_full() == 1 )
            {
                printf("UWAGA!!! STARSZE DANE PRZEPADLY!!!\n");
                lockers_print_latest_data();
            }
            start_program = 2;
        }

        if(start_program == 2 || start_program == 3)
        {
            if(menu != 4 && menu != 5)
            {
                change_color_RGB();
                ds18b20_temperature();
                lockers_check_events();
                if(start_program == 2)
                {
                    temp_char = USART_Recieve_without_waiting();

                    if(temp_char == 'R') lockers_print_all_memory();
                    else if(temp_char == 'r') lockers_print_latest_data();
                    if(temp_char != 0) refresh_screen = 1;

                    if(PCF8583_is_timer_flag_set() == 1)
                    {
                        backlight(2);
                        buzzer();
                        lockers_print_latest_data();
                        PCF8583_timer_flag_off();
                    }

                    struct double_format double_format_temp_from_pcf;
                    i2c_read_buf(PCF8583_address(), PCF8583_TEMPERATURE_CELLS, 3, (uint8_t*)&double_format_temp_from_pcf);
                    double current_temp_temperature = get_double_form_double_format( set_double_format(termometer_temperature, 2));
                    double maximum_temp_temperature = get_double_form_double_format( double_format_temp_from_pcf );

                    if( PCF8583_is_alarm_flag_set() == 1 || current_temp_temperature > maximum_temp_temperature)
                    {
                        backlight(2);
                        if(current_temp_temperature > maximum_temp_temperature)
                        {
                            buzzer_time(10);
                            printf("\nTEMPERATURA PRZEKROCZONA!");
                        }
                        else if(PCF8583_is_alarm_flag_set() == 1)
                        {
                            buzzer();
                        }
                        lockers_print_temperature();
                        PCF8583_alarm_flag_off();
                        //PCF8583_alarm_flag_off();
                    }
                }
            }
            else
            {
                lockers_beginning_actions();
                no_colors_RGB();
            }
        }

        if( menu == 2 )
        {
            refresh_screen = 1;
        }

        if(backlight_of_lcd > 0) --backlight_of_lcd;
        if(backlight_of_lcd == 0) LCD_BacklightOff();

        if(lockers_is_queue_full() == 1) all_colors_RGB();

    }

    if ( start == 1 )//jeśli było się w jakimś podprogramie i właśnie przechodzimy do podprogramu głównego
    {
        start = 0;//informacja, że zaraz będziemy "chwilę" w menu głównym
        LCD_ScreenOn();
        u = menu;
        menu = 0;
        switch_menu = 0;
        zwiekszanie = 1;
//        checking_lockers_state = 0;
//        refreshing_interrupt_off();
        pilot(59,0);
        switch_menu = u;
        refresh_screen = 1;// wyświetlenie ekranu
    }

    if ( stop_button())
    {
        delay_ms_var(30);
        if (stop_button())
        {
            pilot( 100, 0 );//wywołanie funkcji pilot przez naciśnięcie przycisku
            delay_ms_var(100);
        }
    }

    if( Ir_key_press_flag )
    {
        if( !address )
        {
            //TCCR1B &= ~( ( 1 << CS12 ) | ( 1 << CS11 ) | ( 1 << CS10 ) ); //wyłączenie Timera1 (prescaler na zero)
            toggle_action();
            Ir_key_press_flag = 0;
            pilot( command, t );//wywołanie funkcji pilot
            pilot_reset();
        }
    }
}

void toggle_action(void)
{
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
}

//Koniec definicji metod

//Program glowny:

int main( void )
{

//Inicjalizacja

    BUZZER_DIR |= BUZZER;// PORTD7 jako wyjście do buzzera
    delay_ms_var(1000);
    LCD_Initalize();//inicjalizacja wyświetlacza

    i2cSetBitrate(100);//inicjalizacja i2c - utawienie częstotliwości w kHz
    PCF8583_init();//inicjlalizacja wyświetlacza

    PCF8583_alarm_flag_off();
    PCF8583_timer_flag_off();

    PCF8583_24h_format();

    RGB_init();

    //PCF8583_write_word(PCF8583_HEAD, 3000);

    ds18b20_temperature();//zmierzenie temperatury
    timer_2_init();//włączenie losowaniacyfr
    refreshing_interrupt_on();
    ir_init();//inicjalizacja odbioru sygnału z pilota

    lockers_init();//inicjalizacja przycisku wejściowego oraz wejścia i wyjcia

    printf("\nInicjalizacja w toku...\n");

    //PCF8583_alarm_monthly();

    //eeprom_write_word((uint16_t*)257,5);

    pilot_on();
    pilot_state = 1;

    //backlight(1);

    start_program = 1;

    double test_of_double = -310.0;

    while(test_of_double < 310.0)
    {
        LCD_Clear();

        show_double(test_of_double, 2);
        LCD_GoTo(0,1);
        show_double(test_of_double, 1);
        struct double_format temp_doub = set_double_format(test_of_double,2);
        printf("%d.%02d\n", temp_doub.integer_number, temp_doub.decimal_number);
        test_of_double += 0.11;
        //delay_ms_var(2);
    }


    LCD_WriteText("AVR INSPECTOR");
    delay_ms_var(1500);
    LCD_GoTo(6,1);
    LCD_WriteText("Dariusz M.");
    delay_ms_var(1000);
    LCD_PageUpScreen();
    LCD_Clear();

    sczytaj_komende();
    switch_menu = 2;
    pilot( 59, 0 );//przejście do podprogramu nr 2

    start_program = 3;

    //PCF8583_write_word(254, 1256);

    //główna pętla programu
//    start_program = 0;

    lockers_beginning_actions();

    struct double_format maximum_temperature;

    maximum_temperature.integer_number = 26;
    maximum_temperature.decimal_number = 15;

    i2c_write_buf(PCF8583_address(), PCF8583_TEMPERATURE_CELLS, 3, (uint8_t*)&maximum_temperature);

    printf("Inicjalizacja zakonczona.\n");

    sei();//włącza przerwania

    while( 1 )
    {
        sczytaj_komende();
    }

    return 0;
}
