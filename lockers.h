#ifndef LOCKERS_H_
#define LOCKERS_H_

#include <avr/io.h>
#include "delay_lib.h"
#include "PCF8583.h"
#include "EEPROM.h"
#include "stdlib.h"

#define AMOUNT_OF_LOCKERS 2//liczba szafek - nie załatwia wszystkich problemów w kodzie

#define EEPROM_MAX_ADDRESS 255

#define PCF8583_CELL 255

#define SIZE_OF_FRAME 8//ilość biajtów pojedynczej strony danych

#define LOCKER_1_BUTTON_DIR DDRD
#define LOCKER_1_BUTTON_PORT PORTD
#define LOCKER_1_BUTTON_PIN PIND
#define LOCKER_1_BUTTON_IN (1 << PD2)

#define LOCKER_2_BUTTON_DIR DDRD
#define LOCKER_2_BUTTON_PORT PORTD
#define LOCKER_2_BUTTON_PIN PIND
#define LOCKER_2_BUTTON_IN (1 << PD3)

void lockers_init();//inicjalizacja wejść

void lockers_beginning_actions(void);//przypisanie wartości początkowych dla przycisków

struct frame//struktura służąca do zapisu danych z ramki - to właściwie jest ramka danych
{
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;
    uint8_t day;
    uint8_t month;
    int16_t year;
    uint8_t information;
} frame; //8 bajtów

//uint8_t lockers_address_of_frame;//adres następnej gotowej do użycia komórki pamięci, bardzo ważna zmienna

uint8_t states_table[AMOUNT_OF_LOCKERS];//tablica stanów logicznych poszczególnych przycisków, na jej podstawie określa się, czy została dokonana zmiana od ostatniego sprawdzania stanów wejściowych
uint8_t save_info_table[AMOUNT_OF_LOCKERS];//tablica pomocna do jednokrotnego zapisu danych, informacja czy szafka jest zamknięta, czy otwarta

uint8_t locker_1_button(void);//przycisk fizycznie umieszczony na płytce
uint8_t locker_2_button(void);//przycisk fizycznie umieszczony na płytce

void lockers_find_latest_data(void);//funkcja zwracająca adres gotowy do zapisania nowych danych

uint8_t lockers_state_of_single_button( uint8_t );//funkcja zwracająca stan poszczególnych

uint8_t lockers_number_of_frames(void);//liczba ramek danych dla pamięci

void lockers_check_events(void);//nasłuchiwanie zdarzeń, stanów logicznych wejść, jeśli wykryje jakieś zmiany, tworzy tablicę zmian i nakazuje zapis

void lockers_save_events(void);//zapis zdarzeń do pamięci EEPROM na podstawie tablicy ze zdarzeniemi

void lockers_read_frame(uint8_t);//wczytywanie ramki o ustalonym indeksie i zapis do struktury

uint8_t lockers_convert_address_to_index_of_frame(uint8_t );//funkcja podająca indek ramki danych w zależności od aldresu podanej komórki danych

void buzzer();

#endif // LOCKERS_H_
