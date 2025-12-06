/**
    Biblioteka obsługi pamięci eeprom
*/


#include "EEPROM.h"
/**
 Czyta bajt z układu
 \param address adres komórki w układzie
 \return odczytany bajt
*/
uint8_t EEPROM_read(uint8_t address)
{
    uint8_t a;
    a = EEPROM_ADDRESS;
    i2cStart();
    i2cWrite(a);
    i2cWrite(address);
    i2cStart();
    i2cWrite(a | 1);
    a = i2cRead(NOACK);
    i2cStop();
    return a;
}


/**
 Zapisuje bajt do układu
 \param address adres komórki w układzie
 \param data bajt do wpisania
*/
void EEPROM_write(uint8_t address,uint8_t data)
{
    i2cStart();
    i2cWrite(EEPROM_ADDRESS);
    i2cWrite(address);
    i2cWrite(data);
    i2cStop();
    delay_ms_var(5);
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

void EEPROM_clear_all_memory(void)
{
    uint16_t i = 0;
    for(; i <= EEPROM_MAX_ADDRESS; ++i)
    {
        EEPROM_write((uint8_t) i, 0);
    }
}

/*@}*/

