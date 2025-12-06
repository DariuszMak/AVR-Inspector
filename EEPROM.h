/**
    Biblioteka obsługi pamięci eeprom
*/
#ifndef __EEPROM_H__
#define __EEPROM_H__
#include "delay_lib.h"

#include "i2c.h"//sprzętowa obsługa magistrali TWI (I2C)

#define EEPROM_ADDRESS 0xAC

/**
 Zapisuje bajt do układu
 \param address adres komórki w układzie
 \param data bajt do wpisania
*/
void EEPROM_write(uint8_t address,uint8_t data);

/**
 Czyta bajt z układu
 \param address adres komórki w układzie
 \return odczytany bajt
*/
uint8_t EEPROM_read(uint8_t address);

void EEPROM_write_buf(uint8_t adr, uint8_t len, uint8_t *buf );

void EEPROM_read_buf(uint8_t adr, uint8_t len, uint8_t *buf);

/**
 Zapisuje słowo do układu
 \param address adres komórki w układzie
 \param data słowo do wpisania
*/
 void EEPROM_write_word(uint8_t address,uint16_t data);

/**
 Wczytuje słowo z układu
 \param address adres komórki w układzie
*/
 uint16_t EEPROM_read_word(uint8_t address);


/*@}*/

#endif // __EEPROM_H__
