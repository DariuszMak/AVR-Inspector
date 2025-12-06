#ifndef TERMOMETER_H
#define TERMOMETER_H

#include <avr/io.h>
#include "delay_lib.h"

/* DS18B20 przyłączony do portu  PD7 AVRa  */
#define SET_ONEWIRE_PORT     PORTD  |=  _BV(3)
#define CLR_ONEWIRE_PORT     PORTD  &= ~_BV(3)
#define IS_SET_ONEWIRE_PIN   PIND   &   _BV(3)
#define SET_OUT_ONEWIRE_DDR  DDRD   |=  _BV(3)
#define SET_IN_ONEWIRE_DDR   DDRD   &= ~_BV(3)

unsigned char ds18b20_ConvertT(void);
int ds18b20_Read(unsigned char []);
void OneWireStrong(char);
unsigned char OneWireReset(void);
void OneWireWriteByte(unsigned char);
unsigned char OneWireReadByte(void);

unsigned char ds18b20_pad[2];

double ds18b20_temperature(void);

#endif
