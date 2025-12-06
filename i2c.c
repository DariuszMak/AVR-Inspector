/**
	Biblioteka sprzętowej obsługi interfejsu TWI w mikrokontrolerach atmel.
*/

#include "i2c.h"
/**
    Inicjalizacja TWI
*/
void i2cSetBitrate(uint16_t bitrateKHz)
{
    uint8_t bitrate_div;

    bitrate_div = ((F_CPU/1000l)/bitrateKHz);
    if(bitrate_div >= 16)
        bitrate_div = (bitrate_div-16)/2;

    TWBR = bitrate_div;
}
/**
    Procedura transmisji sygnału START
*/
 void i2cStart(void)
{
    TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
    while (!(TWCR & (1<<TWINT)));
}
/**
    Procedura transmisji sygnału STOP
*/
 void i2cStop(void)
{
    TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
    while ((TWCR & (1<<TWSTO)));
}
/**
    Procedura transmisji bajtu danych
*/
 void i2cWrite(char data)
{
    TWDR = data;
    TWCR = (1<<TWINT) | (1<<TWEN);
    while (!(TWCR & (1<<TWINT)));
}
/**
    Procedura odczytu bajtu danych
*/
 char i2cRead(char ack)
{
    TWCR = ack ? ((1 << TWINT) | (1 << TWEN) | (1 << TWEA)) : ((1 << TWINT) | (1 << TWEN)) ;
    while (!(TWCR & (1<<TWINT)));
    return TWDR;
}

