/**
    Biblioteka obsługi zegara czasu rzeczywistego z magistralą I2C typu PCF8583.
*/
#ifndef __PCF8583_H__
#define __PCF8583_H__

#include "i2c.h"//sprzętowa obsługa magistrali TWI (I2C)

#define PCF8583_ADDRESS 0xA0

#define PCF8583_A0 1//gdy pin A0 jest zwarty do GND należy wpisać zero (będzie adres 0xA0), a jeśli jest zwarty z VCC należy wpisać jeden (będzie adres 0xA2)

#define buffer 1

#ifndef PCF8583_A0
#error " Nie zdefiniowałeś stanu lini A0 układu. użyj #define PCF8583_A0  0(1)"
#endif
/**
 Bajt statusu układu
*/
// volatile uint8_t PCF8583_status;

/**
 Bajt alarmu układu
*/
// volatile uint8_t PCF8583_alarm;


//zmienne te mają jedynie charakter bufora, przed każdym użyciem należy upewnić się, czy ich wartości nie uległy zmianie

int8_t godz, min, sek, hsek;

int8_t dzien,dzien_tygodnia, miesiac, timer;
int16_t rok;

struct time_frame
{
    uint8_t hseconds;
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;
    uint8_t days;
    uint8_t months;
    uint8_t timer;
};


/**-------------------------------------------------------------------------------------------------

  Name         :  uint8_t bcd2bin(uint8_t bcd)

  Description  :  Zamiana kodu BCD na binarny, bity BCD: 7654 - dziesiatki, 3210 -jednosci

  Argument(s)  :  bcd - wartość w kodzie BCD

  Return value :  kod binarny z kodu BCD

--------------------------------------------------------------------------------------------------*/
uint8_t bcd2bin(uint8_t bcd);
/**-------------------------------------------------------------------------------------------------

  Name         :  uint8_t bcd2bin(uint8_t bcd);

  Description  :  Zamiana kodu binarnego na BCD

  Argument(s)  :  bin - wartość binarna z zakresu 0-99

  Return value :  kod BCD, bity: 7654 - dziesiatki, 3210 -jednosci

--------------------------------------------------------------------------------------------------*/
uint8_t bin2bcd(uint8_t bin);

/**
 Czyta bajt z układu
 \param address adres komórki w układzie
 \return odczytany bajt
*/
uint8_t PCF8583_read(uint8_t address);


/**
 Zapisuje bajt do układu
 \param address adres komórki w układzie
 \param data bajt do wpisania
*/
void PCF8583_write(uint8_t address,uint8_t data);

void PCF8583_write_buf(uint8_t adr, uint8_t len, uint8_t *buf );

void PCF8583_read_buf(uint8_t adr, uint8_t len, uint8_t *buf);


/**
 Czyta bajt z układu w formacie BCD
 \param address adres komórki w układzie
 \return odczytany bajt
*/
uint8_t PCF8583_read_bcd(uint8_t address);

/**
 Zapisuje bajt do układu w formacie BCD
 \param address adres komórki w układzie
 \param data bajt do wpisania
*/
void PCF8583_write_bcd(uint8_t address,uint8_t data);

/**
 Inicjalizuje układ
*/
void PCF8583_init(void);


/**
 Odwiesza układ
*/
void PCF8583_hold_off(void);

/**
 Zawiesza układu
*/
void PCF8583_hold_on(void);

/**
Odwiesza układ
*/
void PCF8583_mask_off(void);//wyłącza maskę - dostępne są wszystkie rejestry

/**
 Zawiesza układu
*/
void PCF8583_mask_on(void);//maskuje dane - można bezpośrednio odczytywać


/**
 Zapisuje słowo do układu
 \param address adres komórki w układzie
 \param data słowo do wpisania
*/
void PCF8583_write_word(uint8_t address,uint16_t data);

/**
 Ustawia datę w układzie
 \param address adres komórki w układzie
 \param day dzień
 \param year rok
*/
/**
 Zapisuje słowo z układu
 \param address adres komórki w układzie

*/

uint16_t PCF8583_read_word(uint8_t address);


/**
 Czyta czas z układu
 \param hour godzina
 \param min minuta
 \param sec sekunda
 \param hsec setne części sekundy
*/
void PCF8583_get_time(uint8_t *hour, uint8_t *min, uint8_t *sec, uint8_t *hsec, uint8_t *day, uint8_t *day_of_week, uint8_t *month, int16_t *year, uint8_t *timer);

/**
 Ustawia czas w układzie
 \param hour godzina
 \param min minuta
 \param sec sekunda
 \param hsec setne części sekundy
*/
void PCF8583_set_time(uint8_t hour, uint8_t min, uint8_t sec, uint8_t hsec, uint8_t day, uint8_t day_of_week, uint8_t month, int16_t year, uint8_t timer);

/**
 Czyta czas alarmu z układu
 \param hour godzina
 \param min minuta
 \param sec sekunda
 \param hsec setne części sekundy
*/
void PCF8583_get_alarm_time(uint8_t *hour, uint8_t *min, uint8_t *sec, uint8_t *hsec, uint8_t *day, uint8_t *month, uint8_t *timer);

/**
 Ustawia czas alarmu w układzie
 \param hour godzina
 \param min minuta
 \param sec sekunda
 \param hsec setne części sekundy
*/
void PCF8583_set_alarm_time(uint8_t hour, uint8_t min, uint8_t sec, uint8_t hsec, uint8_t day, uint8_t month, uint8_t timer, uint8_t type_of_alarm);




/*****************************PRZYDATNE FUNKCJE ZEWNĘTRZNE********************************/

//flagi

//flaga timera

void PCF8583_timer_flag_off(void);

void PCF8583_timer_flag_on(void);

uint8_t PCF8583_is_timer_flag_set(void);

//flaga alarmu

void PCF8583_alarm_flag_off(void);

void PCF8583_alarm_flag_on(void);

uint8_t PCF8583_is_alarm_flag_set(void);


//odliczanie zegara

void PCF8583_stop(void);

void PCF8583_start(void);

uint8_t PCF8583_is_clock_counting(void);

//tryby odliczania timera

uint8_t PCF8583_timer_mode(void);

void PCF8583_set_timer_mode(uint8_t mode);

//przerwanie timera

void PCF8583_timer_interrupt_off(void);

void PCF8583_timer_interrupt_on(void);

uint8_t PCF8583_is_timer_interrupt(void);

//alarm alarmu

uint8_t PCF8583_recognise_type_of_alarm(void);

void PCF8583_set_type_of_alarm(uint8_t);


//alarm timera

void PCF8583_timer_alarm_off(void);

void PCF8583_timer_alarm_on(void);

uint8_t PCF8583_recognise_type_of_timer_alarm(void);

//przerwanie alarmu

void PCF8583_alarm_interrupt_off(void);

void PCF8583_alarm_interrupt_on(void);

uint8_t PCF8583_is_alarm_interrupt(void);


//

void PCF8583_get_wall_alarm(void);

void PCF8583_get_wall_time(void);

/*@}*/

#endif // __PCF8583_H__
