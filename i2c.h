/**
	Biblioteka sprzętowej obsługi interfejsu TWI w mikrokontrolerach atmel.
*/
#ifndef I2C_H_INCLUDED
#define I2C_H_INCLUDED

#include <avr/io.h>

#define ACK 1
#define NOACK 0

/**
    Inicjalizacja TWI
*/
void i2cSetBitrate(uint16_t bitrateKHz);

/**
    Procedura transmisji sygnału START
*/
 void i2cStart(void);
/**
    Procedura transmisji sygnału STOP
*/
 void i2cStop(void);
/**
    Procedura transmisji bajtu danych
*/
 void i2cWrite(char data);
/**
    Procedura odczytu bajtu danych
*/
 char i2cRead(char ack);
#endif // I2C_H_INCLUDED
