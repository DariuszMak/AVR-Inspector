#ifndef _d_led_h
#define _d_led_h

#define LED_DATA PORTC
#define LED_DATA_DIR DDRC
#define ANODY_PORT PORTB
#define ANODY_DIR DDRB

#define CA1 (1<<PB0)
#define CA2 (1<<PB1)
#define CA3 (1<<PB2)
#define CA4 (1<<PB3)

#define SEG_A (1<<0)
#define SEG_B (1<<1)
#define SEG_C (1<<2)
#define SEG_D (1<<3)
#define SEG_E (1<<4)
#define SEG_F (1<<5)
#define SEG_G (1<<6)
#define SEG_DP (1<<7)

extern volatile uint8_t cy1;
extern volatile uint8_t cy2;
extern volatile uint8_t cy3;
extern volatile uint8_t cy4;

#endif // _d_led_h
