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
static void i2cInit(void)
{
#if defined(TWPS0)
  TWSR = 0;
#endif
  TWBR = (F_CPU / 100000UL - 16)/2;
}
/**
    Procedura transmisji sygnału START
*/
static void i2cStart(void)
{
    TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
    while (!(TWCR & (1<<TWINT)));
}
/**
    Procedura transmisji sygnału STOP
*/
static void i2cStop(void)
{
    TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
    while ((TWCR & (1<<TWSTO)));
}
/**
    Procedura transmisji bajtu danych
*/
static void i2cWrite(char data)
{
    TWDR = data;
    TWCR = (1<<TWINT) | (1<<TWEN);
    while (!(TWCR & (1<<TWINT)));
}
/**
    Procedura odczytu bajtu danych
*/
static char i2cRead(char ack)
{
    TWCR = ack ? ((1 << TWINT) | (1 << TWEN) | (1 << TWEA)) : ((1 << TWINT) | (1 << TWEN)) ;
    while (!(TWCR & (1<<TWINT)));
    return TWDR;
}

#endif // I2C_H_INCLUDED
