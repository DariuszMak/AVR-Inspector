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
    TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTA);
    while (!(TWCR&(1<<TWINT)));
}
/**
    Procedura transmisji sygnału STOP
*/
void i2cStop(void)
{
    TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
    while ( (TWCR&(1<<TWSTO)) );
}

/**
    Procedura transmisji bajtu danych
*/
void i2cWrite(uint8_t bajt)
{
    TWDR = bajt;
    TWCR = (1<<TWINT)|(1<<TWEN);
    while ( !(TWCR&(1<<TWINT)));
}
/**
    Procedura odczytu bajtu danych
*/
uint8_t i2cRead(uint8_t ack)
{
    TWCR = (1<<TWINT)|(ack<<TWEA)|(1<<TWEN);
    while ( !(TWCR & (1<<TWINT)));
    return TWDR;
}

void i2c_write_buf(uint8_t dev, uint8_t adr, uint8_t len, uint8_t *buf )
{
    i2cStart();
    i2cWrite(dev);
    i2cWrite(adr);
    while (len--) i2cWrite(*buf++);
    i2cStop();
}

void i2c_read_buf(uint8_t dev, uint8_t adr, uint8_t len, uint8_t *buf)
{
    i2cStart();
    i2cWrite(dev);
    i2cWrite(adr);
    i2cStart();
    i2cWrite(dev + 1);
    while (len--) *buf++ = i2cRead( len ? ACK : NOACK );
    i2cStop();
}
