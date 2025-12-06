#ifndef LOCKERS_H_
#define LOCKERS_H_

#define BAUD 9600
#define MYUBRR  F_CPU/BAUD/16-1

#include <stdio.h>
#include <avr/io.h>
#include "delay_lib.h"
#include "PCF8583.h"
//#include "EEPROM.h"
#include "avr/eeprom.h"
#include "stdlib.h"
#include "main.h"

#define AMOUNT_OF_LOCKERS 10//liczba szafek - nie załatwia wszystkich problemów w kodzie

//jeśli chce się wyłączyć całą pamięć z użytku, to należy utawić maksymalny index o jeden mniej niż minimalny index
#define INTERNAL_EEPROM_MAX_INDEX 1023

#define INTERNAL_EEPROM_MIN_INDEX 0

#define PCF8583_TAIL 254//komórka i sąsienia komórka (o adresie o jeden większym) jako adres

#define PCF8583_HEAD 252//komórka głowy

#define PCF8583_SAFETY_CELL 251

#define PCF8583_TEMPERATURE_CELLS 248

#define SIZE_OF_FRAME 8//ilość biajtów pojedynczej strony danych

#define LOCKER_1_BUTTON_DIR DDRD
#define LOCKER_1_BUTTON_PORT PORTD
#define LOCKER_1_BUTTON_PIN PIND
#define LOCKER_1_BUTTON_IN (1 << PD2)
#define LOCKER_1_BUTTON_PLACE PD2

#define LOCKER_2_BUTTON_DIR DDRD
#define LOCKER_2_BUTTON_PORT PORTD
#define LOCKER_2_BUTTON_PIN PIND
#define LOCKER_2_BUTTON_IN (1 << PD3)
#define LOCKER_2_BUTTON_PLACE PD3

#define LOCKER_3_BUTTON_DIR DDRD
#define LOCKER_3_BUTTON_PORT	PORTD
#define LOCKER_3_BUTTON_PIN PIND
#define LOCKER_3_BUTTON_IN (1 << PD5)
#define LOCKER_3_BUTTON_PLACE PD5

#define LOCKER_4_BUTTON_DIR DDRD
#define LOCKER_4_BUTTON_PORT	PORTD
#define LOCKER_4_BUTTON_PIN PIND
#define LOCKER_4_BUTTON_IN (1 << PD4)
#define LOCKER_4_BUTTON_PLACE PD4

#define LOCKER_5_BUTTON_DIR DDRC
#define LOCKER_5_BUTTON_PORT	PORTC
#define LOCKER_5_BUTTON_PIN PINC
#define LOCKER_5_BUTTON_IN (1 << PC2)
#define LOCKER_5_BUTTON_PLACE PC2

#define LOCKER_6_BUTTON_DIR DDRC
#define LOCKER_6_BUTTON_PORT	PORTC
#define LOCKER_6_BUTTON_PIN PINC
#define LOCKER_6_BUTTON_IN (1 << PC3)
#define LOCKER_6_BUTTON_PLACE PC3

#define LOCKER_7_BUTTON_DIR DDRC
#define LOCKER_7_BUTTON_PORT	PORTC
#define LOCKER_7_BUTTON_PIN PINC
#define LOCKER_7_BUTTON_IN (1 << PC4)
#define LOCKER_7_BUTTON_PLACE PC4

#define LOCKER_8_BUTTON_DIR DDRC
#define LOCKER_8_BUTTON_PORT	PORTC
#define LOCKER_8_BUTTON_PIN PINC
#define LOCKER_8_BUTTON_IN (1 << PC5)
#define LOCKER_8_BUTTON_PLACE PC5

#define LOCKER_9_BUTTON_DIR DDRC
#define LOCKER_9_BUTTON_PORT	PORTC
#define LOCKER_9_BUTTON_PIN PINC
#define LOCKER_9_BUTTON_IN (1 << PC6)
#define LOCKER_9_BUTTON_PLACE PC6

#define LOCKER_10_BUTTON_DIR DDRC
#define LOCKER_10_BUTTON_PORT PORTC
#define LOCKER_10_BUTTON_PIN PINC
#define LOCKER_10_BUTTON_IN (1 << PC7)
#define LOCKER_10_BUTTON_PLACE PC7

uint8_t state_temp;

//uint16_t lockers_queue_tail;

//uint16_t lockers_queue_head;


/* Inicjuje port szeregowy AVRa */
void USART_init(uint16_t myubrr);

/* Wysyła znak do portu szeregowego */
void USART_Transmit(uint8_t c, FILE *stream);

uint8_t USART_Recieve(FILE *stream);

uint8_t USART_Recieve_without_waiting(void);

void lockers_safety_bit_on(void);

void lockers_safety_bit_off(void);

uint8_t lockers_is_safety_bit(void);

void lockers_init();//inicjalizacja wejść

void lockers_beginning_actions(void);//przypisanie wartości początkowych dla przycisków

void lockers_print_date_of_report();

void lockers_print_entire_frame(void);

void lockers_print_all_memory(void);

void lockers_print_temperature(void);

void lockers_print_latest_data(void);

uint8_t lockers_tail(void);

uint8_t lockers_head(void);

void lockers_queue_empty(void);

uint8_t lockers_is_queue_empty(void);

uint8_t lockers_queue_number_of_records(void);

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
uint8_t locker_3_button(void);//przycisk fizycznie umieszczony na płytce
uint8_t locker_4_button(void);//przycisk fizycznie umieszczony na płytce
uint8_t locker_5_button(void);//przycisk fizycznie umieszczony na płytce
uint8_t locker_6_button(void);//przycisk fizycznie umieszczony na płytce
uint8_t locker_7_button(void);//przycisk fizycznie umieszczony na płytce
uint8_t locker_8_button(void);//przycisk fizycznie umieszczony na płytce
uint8_t locker_9_button(void);//przycisk fizycznie umieszczony na płytce
uint8_t locker_10_button(void);//przycisk fizycznie umieszczony na płytce

uint8_t lockers_state_of_single_button( uint8_t );//funkcja zwracająca stan poszczególnych wejść do szafek (zwraca 0 albo 1)

void lockers_check_events(void);//nasłuchiwanie zdarzeń, stanów logicznych wejść, jeśli wykryje jakieś zmiany, tworzy tablicę zmian i nakazuje zapis

uint8_t lockers_is_queue_full(void);

void lockers_queue_enque(void);//zapis zdarzeń do pamięci EEPROM na podstawie tablicy ze zdarzeniemi

void lockers_queue_dequeue(void);

void lockers_queue_read(uint8_t index);

void lockers_read_frame(uint8_t);//wczytywanie ramki o ustalonym indeksie i zapis do struktury

uint8_t lockers_convert_address_to_index_of_frame(uint16_t );//funkcja podająca indek ramki danych w zależności od aldresu podanej komórki danych

uint16_t lockers_convert_index_of_frame_to_address(uint8_t index);

uint8_t lockers_number_of_frames(void);//liczba ramek danych dla pamięci liczona bez zera (np. 32 dla 8-bajtowych ramek o pamięci 256 bajtów)

void lockers_clear_all_memory(void);

#endif // LOCKERS_H_
