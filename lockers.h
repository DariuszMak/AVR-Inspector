#ifndef LOCKERS_H_
#define LOCKERS_H_

#include <avr/io.h>
#include "delay_lib.h"
#include "PCF8583.h"
#include "EEPROM.h"
#include "stdlib.h"

#define AMOUNT_OF_LOCKERS 2//liczba szafek - nie załatwia wszystkich problemów w kodzie

#define EEPROM_MAX_ADDRESS 255

#define SIZE_OF_FRAME 8//ilość biajtów pojedynczej strony danych

#define LOCKER_1_BUTTON_DIR DDRD
#define LOCKER_1_BUTTON_PORT PORTD
#define LOCKER_1_BUTTON_PIN PIND
#define LOCKER_1_BUTTON_IN (1 << PD2)

#define LOCKER_2_BUTTON_DIR DDRD
#define LOCKER_2_BUTTON_PORT PORTD
#define LOCKER_2_BUTTON_PIN PIND
#define LOCKER_2_BUTTON_IN (1 << PD3)

void lockers_init();
void lockers_beginning_actions(void);

struct frame
{
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;

    uint8_t day;
    uint8_t month;
    uint16_t year;
    uint8_t information;
} frame; //8

uint8_t lockers_address_of_frame;

uint8_t states_table[AMOUNT_OF_LOCKERS];
uint8_t save_info_table[AMOUNT_OF_LOCKERS];

int locker_1_button(void);//przycisk fizycznie umieszczony na płytce
int locker_2_button(void);//przycisk fizycznie umieszczony na płytce

void lockers_find_latest_data(void);//funkcja zwracająca adres gotowy do zapisania nowych danych

int lockers_state_of_single_button( int );

uint8_t lockers_index_of_current_frame(void);

uint8_t lockers_number_of_frames(void);

void lockers_check_events(void);

void lockers_save_events(void);

void lockers_read_frame(uint8_t );

uint8_t lockers_convert_address_to_index_of_frame(uint8_t );



void buzzer();

#endif // LOCKERS_H_
