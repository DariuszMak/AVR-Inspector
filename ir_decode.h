#ifndef IR_DECODE_H_
#define IE_DECODE_H_

#include <avr/io.h>
#include <avr/interrupt.h>

#define IR_DIR DDRD
#define IR_PORT	PORTD
#define IR_PIN PIND
#define IR_IN (1 << PD6)

#define STOP_BUTTON_DIR DDRB
#define STOP_BUTTON_PORT PORTB
#define STOP_BUTTON_PIN PINB
#define STOP_BUTTON_IN (1 << PB4)
#define STOP_BUTTON_PLACE PB4


//#define IR_PORT D // te dyrektywy s¹ autorstwa Miros³wa Kardasia
//#define IR_PIN 6
//#define IR_IN (1<<IR_PIN)

#define TIMER1_PRESCALER 1024

#define ir_micro_s(num) ((num)*(F_CPU/1000000)/TIMER1_PRESCALER)

#define TOLERANCE 200
#define MIN_HALF_BIT ir_micro_s(889 - TOLERANCE)
#define MAX_HALF_BIT ir_micro_s(889 + TOLERANCE)
#define MAX_BIT ir_micro_s ((889+889) + TOLERANCE)

#define FRAME_RESTART 0
#define FRAME_OK 1
#define FRAME_END 2
#define FRAME_ERROR 3

//#define PORT(x) XPORT(x)
//#define XPORT(x) (PORT##x)
//
//#define PIN(x) XPIN(x)
//#define XPIN(x) (PIN##x)
//
//#define DDR(x) XDDR(x)
//#define XDDR(x) (DDR##x)

extern volatile uint8_t toggle_bit;
extern volatile uint8_t address;
extern volatile uint8_t command;

extern volatile uint8_t Ir_key_press_flag;

void ir_init( void );

void pilot_on(void);

void pilot_off(void);

uint8_t stop_button();//funkcja zwraca "1" gdy przycisk jest naciśnięty (nie sprawdza drgań styków)

void pilot_reset(void);


#endif // IR_DECODE_H_
