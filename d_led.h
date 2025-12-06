#ifndef _d_led_h
#define _d_led_h

#define LED_DATA_SEG_A_DIR DDRD
#define LED_DATA_SEG_A_PORT	PORTD
#define LED_DATA_SEG_A_PIN PIND
#define SEG_A (1 << PD5)

#define LED_DATA_SEG_B_DIR DDRD
#define LED_DATA_SEG_B_PORT	PORTD
#define LED_DATA_SEG_B_PIN PIND
#define SEG_B (1 << PD4)

#define LED_DATA_SEG_C_DIR DDRC
#define LED_DATA_SEG_C_PORT	PORTC
#define LED_DATA_SEG_C_PIN PINC
#define SEG_C (1 << PC2)

#define LED_DATA_SEG_D_DIR DDRC
#define LED_DATA_SEG_D_PORT	PORTC
#define LED_DATA_SEG_D_PIN PINC
#define SEG_D (1 << PC3)

#define LED_DATA_SEG_E_DIR DDRC
#define LED_DATA_SEG_E_PORT	PORTC
#define LED_DATA_SEG_E_PIN PINC
#define SEG_E (1 << PC4)

#define LED_DATA_SEG_F_DIR DDRC
#define LED_DATA_SEG_F_PORT	PORTC
#define LED_DATA_SEG_F_PIN PINC
#define SEG_F (1 << PC5)

#define LED_DATA_SEG_G_DIR DDRC
#define LED_DATA_SEG_G_PORT	PORTC
#define LED_DATA_SEG_G_PIN PINC
#define SEG_G (1 << PC6)

#define LED_DATA_SEG_DP_DIR DDRC
#define LED_DATA_SEG_DP_PORT PORTC
#define LED_DATA_SEG_DP_PIN PINC
#define SEG_DP (1 << PC7)



#define LED_ANODY_CA1_DIR DDRB
#define LED_ANODY_CA1_PORT PORTB
#define LED_ANODY_CA1_PIN PINB
#define CA1 (1 << PB0)

#define LED_ANODY_CA2_DIR DDRB
#define LED_ANODY_CA2_PORT	PORTB
#define LED_ANODY_CA2_PIN PINB
#define CA2 (1 << PB1)

#define LED_ANODY_CA3_DIR DDRB
#define LED_ANODY_CA3_PORT	PORTB
#define LED_ANODY_CA3_PIN PINB
#define CA3 (1 << PB2)

#define LED_ANODY_CA4_DIR DDRB
#define LED_ANODY_CA4_PORT	PORTB
#define LED_ANODY_CA4_PIN PINB
#define CA4 (1 << PB3)


extern volatile uint8_t cy1;
extern volatile uint8_t cy2;
extern volatile uint8_t cy3;
extern volatile uint8_t cy4;

void d_led_init( void );
void d_led_Int ( int );

#endif // _d_led_h
