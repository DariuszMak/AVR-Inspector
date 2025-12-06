#ifndef LOCKERS_H_
#define LOCKERS_H_

#include <avr/io.h>
#include "delay_lib.h"
#include "PCF8583.h"
#include "EEPROM.h"
#include "stdlib.h"

#define AMOUNT_OF_LOCKERS 2//liczba szafek - nie załatwia wszystkich problemów w kodzie

#define LOCKER_1_BUTTON_DIR DDRD
#define LOCKER_1_BUTTON_PORT PORTD
#define LOCKER_1_BUTTON_PIN PIND
#define LOCKER_1_BUTTON_IN (1 << PD2)

#define LOCKER_2_BUTTON_DIR DDRD
#define LOCKER_2_BUTTON_PORT PORTD
#define LOCKER_2_BUTTON_PIN PIND
#define LOCKER_2_BUTTON_IN (1 << PD3)

void lockers_init();

char states_table[AMOUNT_OF_LOCKERS];
char save_info_table[AMOUNT_OF_LOCKERS];

int locker_1_button(void);//przycisk fizycznie umieszczony na płytce
int locker_2_button(void);//przycisk fizycznie umieszczony na płytce

int lockers_state_of_single_button( int );

void lockers_check_events(void);

void lockers_save_events(void);

#endif // LOCKERS_H_
