#ifndef _MAIN_H_
#define _MAIN_H_

//Pomiar napiêcia przetwornikiem A/C i prezentacja wyniku na LCD 2x16 HD44780

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#include "delay_lib.h"
#include "HD44780.h"
#include "ir_decode.h"
#include "timer_2.h"
#include "lockers.h"
#include "PCF8583.h"
//#include "EEPROM.h"
#include "i2c.h"
#include "termometer.h"

#define BUZZER_DIR	DDRD
#define BUZZER_PORT	PORTD
#define BUZZER_PIN	PIND
#define BUZZER		(1 << PD7)

//Program glowny:

//UWAGA!!! PONIŻSZE CHARAKTERYZACJE ZMIENNYCH SĄ BARDZO ISTOTNE W CELU POPRAWNEGO ICH UŻYWANIA W PROGRAMIE

//zmienne zarezerwowane - nie można ich używać do innych celów niż wskazane
//zmienne zarezerwowane globalnie dla całego programu
const int liczbaPodprogramow;
uint8_t menu;// zmienna odpowiedzialna za przebywanie w danym podprogramie
int8_t start; // zmienna pomocna do stwierdzenia, czy jest się już w glownym menu = 0, czy właśnie wyszło się z podprogramu i trzeba np. zatrzymać jakiś timer = 1
int8_t toggle;//zmienna odpowiedzialna za świadomość dłuższego przytrzymania przycisku pilota (wartość 2 jest wartością początkową w celu późniejszego skalibrowania ze stanem pilota)
uint16_t zwiekszanie; // zmienna potrzebna do zmiany wartości liczby na wyświetlaczu alfanumerycznym (przyjmuje wartości 1,10,100,1000)
uint8_t moveStep;//zmienna do przesunięcia wyświetlanych partii danych (dla daty)
uint8_t pilot_state;//zmienna odpowiedzialna za działanie, bądź niedziałanie timera od odczytu pilota
uint8_t checking_lockers_state;//zmienna odpowiedzialna za sprawdzanie stanów wejść
int8_t backlight_of_lcd;//zmienna odpowiedzialna za podświetlenie ekranu LCD: 0 - ekran wyłączony, -1 - ekran włączony na stałe, wartość dodatnia będzie dekrementowana aż do wartości 0 przy każdorazowym odwiedzeniu fragmentu kodu wywoływanego przez timer
uint8_t start_program;//zmienna odpowiedzialna za poszczególne etapy podczas włączania się projektu: 1 - program jest w stadium bez wyświetlania informacji o wchodzeniu do poszczególnych podprogramów, 3 - okres oczekiwania na jakikolwiek przycisk, 0 - stan, kiedy mogą zostać wysłane awaryjne raporty, 2 - normalny stan programu
//zmienne zarezerwowane dla podprogramu nr 2:

//zmienne spełniające określone funkcje
int rozmiar; // zmienna odpowiedzialna za rozmiar tablicy dynamicznej
int16_t t; // zmienna pomocnicza wykorzystana w pętlach for do iteracji, może być używana do przeróżnych innych operacji w programie, nie można polegać na globalnej wartości tej zmiennej, ponieważ bardzo często ulega zmianie
//inne zmienne pomocnicze do wykorzystywania w innch podprogramach (wymaga to wcześniejszego zapoznania się z kodem)

int8_t u; //inna (dodatkowa) zmienna pomocnicza
int8_t w; //inna (dodatkowa) zmienna pomocnicza
int8_t s;//inna (dodatowa zmienna)
int16_t c;//inna (dodatowa zmienna)
//int8_t d;//inna (dodatkowa zmienna pmocnicza)
int8_t e;//inna (dodatkowa zmienna)

uint8_t refresh_screen;

struct double_format
{
    int16_t integer_number;
    uint8_t decimal_number;
}double_format_global;

int main( void );

uint8_t number_of_digits(uint32_t);

void show_current_temperature(void);

int8_t put_double_format(double, unsigned int);// wyświetla liczby (pobiera liczbę zmiennoprzecinkową i wyświetla w systemie dziesiętnym z dokładnością do podajego miejsca po pawej stronie przecinka - maksymalna ilość miejsc to cztery)

void change_color_RGB(void);

void all_colors_RGB(void);

void no_colors_RGB(void);

void backlight(int8_t);

void buzzer(void);//funkcja odpowiedzialna za sygnał dźwiękowy (trwa jedną milisekundę)

void buzzer_time( double time );//funkcja odpowiedzialna za sygnał dźwiękowy (trwa podaną liczbę milisekund)

void wysw_skok( uint16_t number ); // funkcja wyświetlająca numer kroku o danej wartości

void step_increase(void);

void step_decrease(void);

void wybor( int number ); // funkcja wyświetlająca podczas wchodenia w dany podprogram numeru podprogramu

void show_day_of_week( uint8_t day);

void show_time_only_format(void);

void show_time_format(void);

void show_timer_alarm_format(void);

void show_alarm_format(uint8_t case_of_format);

void show_setting_alarm_case(uint8_t index);

void show_alarm_flag_options(uint8_t index);

void show_timer_flag_options(uint8_t index);

void show_clock_options(uint8_t index);

void show_timer_mode_options(uint8_t index);

void show_alarm_interrupt_options(uint8_t index);

void show_timer_interrupt_options(uint8_t index);

void show_setting_flags_case(uint8_t index);

void show_alarm_options(uint8_t index);

void correction_of_time(void);

void correction_of_date(void);//uwzględnianie dnia miesiąca względem roku

void setting_information(void);

void set_appropriate_values_of_time(void);

void check_step_value(void);

uint8_t end_of_settings(void);

void show_frame( int16_t number);

void show_list(int16_t current_index, int16_t max_index);

void wysw( void );// funkcja wyświetlająca - interfejs dla każdego z podprogramów

void wysw0( void );// funkcja wyświetlająca - interfejs dla każdego z podprogramów

void wysw1( void );// funkcja wyświetlająca - interfejs dla każdego z podprogramów

void wysw2( void );// funkcja wyświetlająca - interfejs dla każdego z podprogramów

void wysw3( void );// funkcja wyświetlająca - interfejs dla każdego z podprogramów

void wysw4( void );// funkcja wyświetlająca - interfejs dla każdego z podprogramów

void wysw5( void );// funkcja wyświetlająca - interfejs dla każdego z podprogramów

void wysw6( void );// funkcja wyświetlająca - interfejs dla każdego z podprogramów

void czynnosc( int com, int tog ); //funkcja odpowiedzialna za wywołanie odpowiedniej czynności (pierwszy argument musi być przez wskaźnik, ponieważ, może być dokonana zmiana zmiennej "menu")

void czynnosc0( int com, int tog );//tutaj są inicjowane wartości przy wchodzeniu do podprogramów

void czynnosc1( int com, int tog );

void czynnosc2( int com, int tog );

void czynnosc3( int com, int tog );

void czynnosc4( int com, int tog );

void czynnosc5( int com, int tog );

void czynnosc6( int com, int tog );

// funkcja obsługująca menu dwupoziomowe

void pilot( int com, int tog );//

void toggle_action(void);

// funkcja odpowiedzialna za odczytanie komend z pilota i przekazaniu ich do fukcji pilot, dopóki nie zostaną wykonane wszystkie rozkazy, nie będzie można odzczytać innego przysisku

void sczytaj_komende( void );
#endif
