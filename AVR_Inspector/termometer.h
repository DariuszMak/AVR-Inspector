#ifndef TERMOMETER_H
#define TERMOMETER_H

#include <avr/io.h>
#include "delay_lib.h"
#include <avr/interrupt.h>
#include "timer_0.h"
//#include "lockers.h"
//#include "ir_decode.h"

#define ONEWIRE_DIR DDRA
#define ONEWIRE_PORT PORTA
#define ONEWIRE_PIN PINA
#define ONEWIRE (1 << PA0)

double termometer_temperature;

/* DS18B20 przyłączony do portu  PD7 AVRa  */

unsigned char ds18b20_ConvertT(void);
int ds18b20_Read();
void OneWireStrong(char);
unsigned char OneWireReset(void);
void OneWireWriteByte(unsigned char);
unsigned char OneWireReadByte(void);

unsigned char ds18b20_pad[2];

void ds18b20_temperature(void);

#endif
