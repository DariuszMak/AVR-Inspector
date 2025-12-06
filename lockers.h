#ifndef LOCKERS_H_
#define LOCKERS_H_

#include <avr/io.h>
#include "delay_lib.h"
#include "PCF8583.h"
#include "EEPROM.h"
#include "avr/eeprom.h"
#include "stdlib.h"
#include "main.h"

#define AMOUNT_OF_LOCKERS 2//liczba szafek - nie załatwia wszystkich problemów w kodzie

#define INTERNAL_EEPROM_MAX_INDEX 1023//jeśli ustawi się tą zmienną na -1, to wylączy się tę pamięć z użytku

#define EXTERNAL_EEPROM_MAX_INDEX 255//wartość ostatniej największej komórki zewnętrznej pamięci eeprom do wykorzystania

#define PCF8583_CELL 254//komórka i sąsienia komórka (o adresie o jeden większym) jako adres

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

uint8_t lockers_state_of_single_button( uint8_t );//funkcja zwracająca stan poszczególnych wejść do szafek (zwraca 0 albo 1)

void lockers_check_events(void);//nasłuchiwanie zdarzeń, stanów logicznych wejść, jeśli wykryje jakieś zmiany, tworzy tablicę zmian i nakazuje zapis

void lockers_save_events(void);//zapis zdarzeń do pamięci EEPROM na podstawie tablicy ze zdarzeniemi

void lockers_read_frame(uint16_t);//wczytywanie ramki o ustalonym indeksie i zapis do struktury

uint16_t lockers_convert_address_to_index_of_frame(uint16_t );//funkcja podająca indek ramki danych w zależności od aldresu podanej komórki danych

uint16_t lockers_number_of_frames(void);//liczba ramek danych dla pamięci liczona bez zera (np. 32 dla 8-bajtowych ramek o pamięci 256 bajtów)

uint8_t lockers_number_of_frames_exteral_EEPROM(void);

uint16_t lockers_number_of_frames_internal_EEPROM(void);

void lockers_clear_all_memory(void);

#endif // LOCKERS_H_
