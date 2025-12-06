#ifndef IR_DECODE_H_
#define IE_DECODE_H_

#define IR_DIR		DDRD
#define IR_PORT	PORTD
#define IR_PIN PIND
#define IR_IN (1 << PD6)

//#define IR_PORT D
//#define IR_PIN 6
//#define IR_IN (1<<IR_PIN)

#define TIMER1_PRESCALER 64

#define ir_micro_s(num) ((num)*(F_CPU/1000000)/TIMER1_PRESCALER)

#define TOLERANCE 200
#define MIN_HALF_BIT ir_micro_s(889 - TOLERANCE)
#define MAX_HALF_BIT ir_micro_s(889 + TOLERANCE)
#define MAX_BIT ir_micro_s ((889+889) + TOLERANCE)

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

#endif // IR_DECODE_H_
