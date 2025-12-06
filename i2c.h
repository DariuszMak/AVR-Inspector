/**
	Biblioteka sprzętowej obsługi interfejsu TWI w mikrokontrolerach atmel.
*/
#ifndef I2C_H_INCLUDED
#define I2C_H_INCLUDED

#include <avr/io.h>

#define ACK 1
#define NOACK 0

#define buffer 0

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
 void i2cWrite(uint8_t);
/**
    Procedura odczytu bajtu danych
*/
 uint8_t i2cRead(uint8_t);

#if buffer == 1
void TWI_write_buf( uint8_t SLA, uint8_t adr, uint8_t len, uint8_t *buf );
void TWI_read_buf(uint8_t SLA, uint8_t adr, uint8_t len, uint8_t *buf);
#endif // buffer

#endif // I2C_H_INCLUDED
