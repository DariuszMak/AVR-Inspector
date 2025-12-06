#ifndef TERMOMETER_H
#define TERMOMETER_H

#include <avr/io.h>
#include "delay_lib.h"

/* DS18B20 przyłączony do portu  PD7 AVRa  */
#define SET_ONEWIRE_PORT     PORTD  |=  (1 << PD3)
#define CLR_ONEWIRE_PORT     PORTD  &= ~(1 << PD3)
#define IS_SET_ONEWIRE_PIN   PIND   &   (1 << PD3)
#define SET_OUT_ONEWIRE_DDR  DDRD   |=  (1 << PD3)
#define SET_IN_ONEWIRE_DDR   DDRD   &= ~(1 << PD3)

unsigned char ds18b20_ConvertT(void);
int ds18b20_Read();
void OneWireStrong(char);
unsigned char OneWireReset(void);
void OneWireWriteByte(unsigned char);
unsigned char OneWireReadByte(void);

unsigned char ds18b20_pad[2];

double ds18b20_temperature(void);

#endif
