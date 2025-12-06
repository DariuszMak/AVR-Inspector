/**
    Biblioteka obsługi zegara czasu rzeczywistego z magistralą I2C typu PCF8583.
*/
#ifndef __PCF8583_H__
#define __PCF8583_H__

/* użyte biblioteki */
#include "i2c.h"//sprzętowa obsługa magistrali TWI (I2C)

#define PCF8583_A0  0


#ifndef PCF8583_A0
#error " Nie zdefiniowałeś stanu lini A0 układu. użyj #define PCF8583_A0  0(1)"
#endif
/**
 Bajt statusu układu
*/
static volatile uint8_t PCF8583_status;

/**
 Bajt alarmu układu
*/
static volatile uint8_t PCF8583_alarm;


/**-------------------------------------------------------------------------------------------------

  Name         :  uint8_t bcd2bin(uint8_t bcd)

  Description  :  Zamiana kodu BCD na binarny, bity BCD: 7654 - dziesiatki, 3210 -jednosci

  Argument(s)  :  bcd - wartość w kodzie BCD

  Return value :  kod binarny z kodu BCD

--------------------------------------------------------------------------------------------------*/
static uint8_t bcd2bin(uint8_t bcd)
{
    return (10*(bcd>>4) + (bcd&0x0f));
}
/**-------------------------------------------------------------------------------------------------

  Name         :  uint8_t bcd2bin(uint8_t bcd);

  Description  :  Zamiana kodu binarnego na BCD

  Argument(s)  :  bin - wartość binarna z zakresu 0-99

  Return value :  kod BCD, bity: 7654 - dziesiatki, 3210 -jednosci

--------------------------------------------------------------------------------------------------*/
static uint8_t bin2bcd(uint8_t bin)
{
    return ( ( ( bin / 10 ) << 4) | ( bin % 10 ) );
}

/**
 Czyta bajt z układu
 \param address adres komórki w układzie
 \return odczytany bajt
*/
static uint8_t PCF8583_read(uint8_t address)
{
    uint8_t a;
    a = (PCF8583_A0 << 1) | 0xa0;
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
static void PCF8583_write(uint8_t address,uint8_t data)
{
    i2cStart();
    i2cWrite((PCF8583_A0 << 1) | 0xa0);
    i2cWrite(address);
    i2cWrite(data);
    i2cStop();
}


/**
 Czyta bajt z układu w formacie BCD
 \param address adres komórki w układzie
 \return odczytany bajt
*/
static uint8_t PCF8583_read_bcd(uint8_t address)
{
    return bcd2bin(PCF8583_read(address));
}

/**
 Zapisuje bajt do układu w formacie BCD
 \param address adres komórki w układzie
 \param data bajt do wpisania
*/
static void PCF8583_write_bcd(uint8_t address,uint8_t data)
{
    PCF8583_write(address,bin2bcd(data));
}

/**
 Czyta status układu
 \return status układu
*/
static uint8_t PCF8583_get_status(void)
{
    PCF8583_status = PCF8583_read(0);
    PCF8583_alarm = (PCF8583_status & 2);
    return PCF8583_status;
}


/**
 Inicjalizuje układu
*/
static void PCF8583_init(void)
{
    PCF8583_status=0;
    PCF8583_alarm=0;
    PCF8583_write(0, 0);
    PCF8583_write(4, PCF8583_read(4) & 0x3f);
    PCF8583_write(8, 0x90);
}

/**
 Zatrzymuje układu
*/
static void PCF8583_stop(void)
{
    PCF8583_get_status();
    PCF8583_status |= 0x80;
    PCF8583_write(0, PCF8583_status);
}

/**
 Startuje układu
*/
static void PCF8583_start(void)
{
    PCF8583_get_status();
    PCF8583_status &= 0x7f;
    PCF8583_write(0, PCF8583_status);
}

/**
 Odwiesza układu
*/
static void PCF8583_hold_off(void)
{
  PCF8583_get_status();
  PCF8583_status &= 0xbf;
  PCF8583_write(0, PCF8583_status);
}

/**
 Zawiesza układu
*/
static void PCF8583_hold_on(void)
{
  PCF8583_get_status();
  PCF8583_status |= 0x40;
  PCF8583_write(0, PCF8583_status);
}


/**
 Wyłącza alarm
*/
static void PCF8583_alarm_off(void)
{
    PCF8583_get_status();
    PCF8583_status &= 0xfb;
    PCF8583_write(0, PCF8583_status);
}

/**
 Załącza alarm
*/
static void PCF8583_alarm_on(void)
{
  PCF8583_get_status();
  PCF8583_status |= 0x04;
  PCF8583_write(0, PCF8583_status);
}


/**
 Zapisuje słowo do układu
 \param address adres komórki w układzie
 \param data słowo do wpisania
*/
static void PCF8583_write_word(uint8_t address,uint16_t data)
{
  PCF8583_write(address, (uint8_t)(data & 0xff));
  PCF8583_write(++address, (uint8_t)(data >> 8));
}


/**
 Ustawia datę w układzie
 \param address adres komórki w układzie
 \param day dzień
 \param year rok
*/
static void PCF8583_write_date(uint8_t address,uint8_t day,uint16_t year)
{
    PCF8583_write(address, bin2bcd(day) | ( ( (uint8_t)year & 0x03) << 6 ) );
}

/**
 Czyta czas z układu
 \param hour godzina
 \param min minuta
 \param sec sekunda
 \param hsec setne części sekundy
*/
static void PCF8583_get_time(uint8_t *hour,uint8_t *min,uint8_t *sec,uint8_t *hsec)
{
    PCF8583_hold_on();
    *hsec=PCF8583_read_bcd(1);
    *sec=PCF8583_read_bcd(2);
    *min=PCF8583_read_bcd(3);
    *hour=PCF8583_read_bcd(4);
    PCF8583_hold_off();
}

/**
 Czyta czas z układu
 \param hour godzina w formanie BCD
 \param min minuta w formanie BCD
 \param sec sekunda w formanie BCD
 \param hsec setne części sekundy w formanie BCD
*/
static void PCF8583_get_time_bcd(uint8_t *hour,uint8_t *min,uint8_t *sec,uint8_t *hsec)
{
    PCF8583_hold_on();
    *hsec=PCF8583_read(1);
    *sec=PCF8583_read(2);
    *min=PCF8583_read(3);
    *hour=PCF8583_read(4) & 0b00111111;
    PCF8583_hold_off();
}
/**
 Ustawia czas w układzie
 \param hour godzina
 \param min minuta
 \param sec sekunda
 \param hsec setne części sekundy
*/
static void PCF8583_set_time(uint8_t hour,uint8_t min,uint8_t sec,uint8_t hsec)
{
    PCF8583_stop();
    PCF8583_write_bcd(1,hsec);
    PCF8583_write_bcd(2,sec);
    PCF8583_write_bcd(3,min);
    PCF8583_write_bcd(4,hour);
    PCF8583_start();
}
/**
 Czyta datę z układu
 \param day dzień
 \param month miesiąc
 \param year rok
*/
static void PCF8583_get_date(uint8_t *day,uint8_t *month,uint16_t *year)
{
    uint8_t dy;
    uint16_t y1;
    PCF8583_hold_on();
    dy = PCF8583_read(5);
    *month = bcd2bin(PCF8583_read(6) & 0x1f);
    PCF8583_hold_off();
    *day = bcd2bin(dy & 0x3f);
    dy >>= 6;
    y1 = PCF8583_read(16) | ( (uint16_t)PCF8583_read(17) << 8);
    if ( ( (uint8_t) y1 & 3 ) != dy )
            PCF8583_write_word(16, ++y1);
    *year = y1;
}

/**
 Ustawia datę w układzie
 \param day dzień
 \param month miesiąc
 \param year rok
*/
static void PCF8583_set_date(uint8_t day,uint8_t month,uint16_t year)
{
    PCF8583_write_word(16, year);
    PCF8583_stop();
    PCF8583_write_date(5, day, year);
    PCF8583_write_bcd(6, month);
    PCF8583_start();
}

/**
 Czyta czas alarmu z układu
 \param hour godzina
 \param min minuta
 \param sec sekunda
 \param hsec setne części sekundy
*/
static void PCF8583_get_alarm_time(uint8_t *hour, uint8_t *min, uint8_t *sec, uint8_t *hsec)
{
  *hsec=PCF8583_read_bcd(0x9);
  *sec=PCF8583_read_bcd(0xa);
  *min=PCF8583_read_bcd(0xb);
  *hour=PCF8583_read_bcd(0xc);
}

/**
 Ustawia czas alarmu w układzie
 \param hour godzina
 \param min minuta
 \param sec sekunda
 \param hsec setne części sekundy
*/
static void PCF8583_set_alarm_time(uint8_t hour, uint8_t min, uint8_t sec, uint8_t hsec)
{
  PCF8583_write_bcd(0x9, hsec);
  PCF8583_write_bcd(0xa, sec);
  PCF8583_write_bcd(0xb, min);
  PCF8583_write_bcd(0xc, hour);
}

/**
 Czyta datê alarmu z układu
 \param day dzień
 \param month miesiąc
*/
static void PCF8583_get_alarm_date(uint8_t *day, uint8_t *month)
{
  *day = bcd2bin( PCF8583_read(0xd) & 0x3f );
  *month = bcd2bin( PCF8583_read(0xe) & 0x1f );
}

/**
 Ustawia datę alarmu w układzie
 \param day dzień
 \param month miesiąc
*/
static void PCF8583_set_alarm_date (uint8_t day, uint8_t month )
{
  PCF8583_write_date( 0xd, day, 0 );
  PCF8583_write_bcd( 0xe, month );
}

/*@}*/

#endif // __PCF8583_H__
