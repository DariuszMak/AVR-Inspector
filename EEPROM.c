/**
    Biblioteka obsługi pamięci eeprom
*/


#include "EEPROM.h"

/**
 Zapisuje bajt do układu
 \param address adres komórki w układzie
 \param data bajt do wpisania
*/
void EEPROM_write(uint8_t address,uint8_t data)
{
    EEPROM_write_buf(address, 1, &data );
}

/**
 Czyta bajt z układu
 \param address adres komórki w układzie
 \return odczytany bajt
*/
uint8_t EEPROM_read(uint8_t address)
{
    uint8_t temp;
    EEPROM_read_buf( address, 1, &temp );
    return temp;
}

void EEPROM_write_buf(uint8_t adr, uint8_t len, uint8_t *buf )
{
    i2cStart();
    i2cWrite(EEPROM_ADDRESS);
    i2cWrite(adr);
    while (len--)
    {
        i2cWrite(*buf++);

        delay_ms_var(5);
    }
    i2cStop();
        delay_ms_var(5);
}

void EEPROM_read_buf(uint8_t adr, uint8_t len, uint8_t *buf)
{
    uint8_t a;
    a = EEPROM_ADDRESS;
    i2cStart();
    i2cWrite(a);
    i2cWrite(adr);
    i2cStart();
    i2cWrite(a + 1);
    while (len--) *buf++ = i2cRead( len ? ACK : NOACK );
    i2cStop();
}

/**
 Zapisuje słowo do układu
 \param address adres komórki w układzie
 \param data słowo do wpisania
*/
void EEPROM_write_word(uint8_t address,uint16_t data)
{
    EEPROM_write(address, (uint8_t)(data & 0xFF));
    EEPROM_write(++address, (uint8_t)(data >> 8));
}

/**
 Wczytuje słowo z układu
 \param address adres komórki w układzie
*/
uint16_t EEPROM_read_word(uint8_t address)
{
    uint16_t temp;
    temp = EEPROM_read(address) & 0xFF;
    temp |= EEPROM_read(++address) << 8;
    return temp;
}



/*@}*/

