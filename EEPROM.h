/**
    Biblioteka obsługi pamięci eeprom
*/
#ifndef __EEPROM_H__
#define __EEPROM_H__
#include "delay_lib.h"

#include "i2c.h"//sprzętowa obsługa magistrali TWI (I2C)

#define EEPROM_MAX_ADDRESS 255

#define EEPROM_ADDRESS 0xAC

/**
 Czyta bajt z układu
 \param address adres komórki w układzie
 \return odczytany bajt
*/
uint8_t EEPROM_read(uint8_t address);

/**
 Zapisuje bajt do układu
 \param address adres komórki w układzie
 \param data bajt do wpisania
*/
 void EEPROM_write(uint8_t address,uint8_t data);

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

 void EEPROM_clear_all_memory(void);


/*@}*/

#endif // __EEPROM_H__
